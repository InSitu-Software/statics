using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.ServiceModel;
using System.Threading;
using System.Windows.Forms;
using SecSignerSoapExample.SecSignerNs;

namespace SecSignerSoapExample
{
    /**
     * C# example code how to call the SecSigner's sign() and verify() methods via SOAP.
     *
     * To run this example please copy
     *  - SecSigner.jar
     *  - SecSigner.selfchecksig
     *  - SecSignerExt.jar and
     *  - secsigner.properties
     * to the working directory of this application, i.e. for example developer/SOAP-CSharp/bin/Debug.
     *
     * To add a service reference using SecSigner's SOAP API in another C# project first
     * start SecSigner in SOAP mode using this command line. You can choose any port number:
     * 
     *   java.exe -Xmx500m -classpath SecSigner.jar;SecSignerExt.jar seccommerce.secsigner.soap.SecSignerSoap -port 48204
     * 
     * Then add a service reference in the C# project using this URL with your chosen port number:
     *
     *   http://localhost:48204/
     *
     * Before you can send SOAP requests to the SecSigner your application has to start 
     * the SecSigner in SOAP mode. You will find example code in this class. When your
     * application ends or if you want to create a new SecSigner instance make sure to
     * kill the old SecSigner process first. 
     */
    public partial class SecSignerSoapExampleForm : Form
    {
        // the document to sign or verify
        private byte[] document;

        // the signature to verify
        private byte[] signature;

        // the file name of the document to sign or verify
        private string docFileNameWithPath;

        // the file name of the signature to verify
        private string sigFileNameWithPath;

        // SecSigner's Java process
        private Process secSignerProcess;

        // true as soon as SecSigner has started sucessfully
        private bool secSignerStarted = false;

        // true as soon as the Java VM has written anything to stderr
        private bool secSignerStartFailed = false;

        // SecSigner via SOAP
        private SecSignerNs.SecSignerSoapPortTypeClient secSigner;

        /**
         * Constructor of the example dialog.
         */
        public SecSignerSoapExampleForm()
        {
            InitializeComponent();

            // kill SecSigner's Java process when this application ends
            Application.ApplicationExit += new EventHandler(this.OnApplicationExit);

            // create the SOAP client for SecSigner
            secSigner = new SecSignerSoapPortTypeClient();

            // allow large SOAP response messages
            BasicHttpBinding secSignerHttpBinding = (BasicHttpBinding)secSigner.Endpoint.Binding;
            secSignerHttpBinding.MaxReceivedMessageSize = 1000000000; // example value, in reality depends on the size of your document batches
            secSignerHttpBinding.SendTimeout = TimeSpan.MaxValue;
        }

        /**
         * Creates SecSigner's Java process.
         */
        private void createSecSigner_Click(object sender, EventArgs eventArgs)
        {
            if (null != secSignerProcess)
            {
                MessageBox.Show("SecSigner process already running.");
                return;
            }

            secSignerStarted = false;
            secSignerStartFailed = false;

            // These parameters may be configured in your application which uses SecSigner.
            string secSignerSoapLogFileName = "secSignerSoapLog.txt";
            string secSignerPath = ".";
            string debugFilesPath = null;  // "soapDebug"; // optional
            int javaHeapSizeMB = 500;
            int secSignerSoapPort = 48204;

            ProcessStartInfo secSignerProcessStartInfo = new ProcessStartInfo();
            secSignerProcessStartInfo.FileName = "java.exe";
            secSignerProcessStartInfo.Arguments = "-Xmx" + javaHeapSizeMB + "m -classpath " + secSignerPath + "\\SecSigner.jar;" + secSignerPath +
                "\\SecSignerExt.jar seccommerce.secsigner.soap.SecSignerSoap -port " + secSignerSoapPort;

            if (null != debugFilesPath)
            {
                secSignerProcessStartInfo.Arguments += " -debugfolder " + debugFilesPath + " -debugsoap";
            }

            secSignerProcessStartInfo.UseShellExecute = false;
            secSignerProcessStartInfo.CreateNoWindow = true;

            // redirect the stdout and stderr output of the Java VM and SecSigner
            secSignerProcessStartInfo.RedirectStandardOutput = true;
            secSignerProcessStartInfo.RedirectStandardError = true;

            try
            {
                secSignerProcess = Process.Start(secSignerProcessStartInfo);
            }
            catch (Exception e)
            {
                MessageBox.Show("Could not create a SecSigner process: " + e);
                return;
            }

            // collect SecSigner's log output lines in case an error appears in it during the start-up
            StringBuilder logLines = new StringBuilder();

            // handler for stdout
            secSignerProcess.OutputDataReceived += new DataReceivedEventHandler((senderStdout, e) =>
            {
                String stdoutLine = e.Data;
                if (!String.IsNullOrEmpty(stdoutLine))
                {
                    // Append SecSigner's log line to the log file.
                    logLines.Append(stdoutLine + "\r\n");
                    File.AppendAllText(secSignerSoapLogFileName, stdoutLine + "\r\n");
                    
                    // Wait for a line containing: "Loading SecSigner properties" since it will only 
                    // appear after SecSigner has opened its SOAP server port.
                    if (0 < stdoutLine.IndexOf("Loading SecSigner properties"))
                    {
                        // The SecSigner is starting and has already bound its SOAP server port.
                        secSignerStarted = true;
                        MessageBox.Show("SecSigner has started and is waiting for SOAP requests.");
                    }
                }
            });

            // handler for stderr
            secSignerProcess.ErrorDataReceived += new DataReceivedEventHandler((senderStderr, e) =>
            {
                String stderrLine = e.Data;
                if (!String.IsNullOrEmpty(stderrLine))
                {
                    // During a normal SecSigner nothing will appear on stderr. If there
                    // is something then it must be a fatal error message.
                    File.AppendAllText(secSignerSoapLogFileName, stderrLine + "\r\n");
                    secSignerStartFailed = true;
                    MessageBox.Show("SecSigner could not be started: " + stderrLine);
                }
            });

            secSignerProcess.BeginOutputReadLine();
            secSignerProcess.BeginErrorReadLine();

            // Wait until we know that SecSigner is either started successfully or failed to start.
            while (!secSignerStarted && !secSignerStartFailed && !secSignerProcess.HasExited)
            {
                Thread.Sleep(100);
            }

            if (!secSignerStarted && !secSignerStartFailed)
            {
                // SecSigner has exited already. There must be an explaination for this error in the log lines.
                secSignerStartFailed = true;
                MessageBox.Show("SecSigner could not start:\r\n" + logLines.ToString());
            }

            if (secSignerStartFailed)
            {
                // In the unlikely event that a fatal error during SecSigner's start up did not terminate
                // the SecSigner's Java process then kill it here.
                if (!secSignerProcess.HasExited)
                {
                    secSignerProcess.Kill();
                }
                secSignerProcess = null;
            }

            logLines.Clear();
        }

        /**
         * Signs a single document.
         */
        private void sign_Click(object sender, EventArgs eventArgs)
        {
            sign(1);
        }

        /**
         * Signs a batch of 25 documents.
         */
        private void bSign100_Click(object sender, EventArgs e)
        {
            sign(25);
        }

        /**
         * Signs the specified number of documents in a batch.
         */
        private void sign(int numberOfDocuments)
        {
            if (null == secSignerProcess)
            {
                MessageBox.Show("SecSigner process needs to be created first.");
                return;
            }

            if (document == null)
            {
                MessageBox.Show("Please load a file to be signed first.");
                return;
            }

            SignReq signReq = new SignReq();

            SignatureRecord[] sigRecords = new SignatureRecord[numberOfDocuments];
            for (int i = 0; i < sigRecords.Length; i++)
            {
                SignatureRecord sigRecord = new SignatureRecord();
                sigRecord.Document = document;
                // Parameters like the signature format, the hash algorithm, the PDF annotation layout 
                // and further more can be set in sigRecord. In case you assign a value to a boolean 
                // parameter do not forget to set the respective "Specified" parameter true. Otherwise
                // the boolean parameter will not appear in the SOAP request.
                sigRecords[i] = sigRecord;
            }
            signReq.SignatureRecord = sigRecords;

            // Optionally add an attribute certificate to the signature.
            // The user can also load an attribute certificate file in the SecSigner dialog.
            //     signReq.AttributeCert = new byte[1][];
            //     signReq.AttributeCert[0] = File.ReadAllBytes(attribCertFileNameWithPath);

            SignRes signRes;
            try
            {
                signRes = secSigner.sign(signReq);
            }
            catch (Exception e)
            {
                MessageBox.Show("SecSigner.sign() failed: " + e);
                return;
            }

            SignatureRecord returnedSignatureRecord = signRes.SignatureRecord[0];
            MessageBox.Show("Signature completed. Writing the signature to " + sigFileNameWithPath);
            File.WriteAllBytes(sigFileNameWithPath, returnedSignatureRecord.Signature);
        }

        /**
         * Verifies a signature.
         */
        private void verify_Click(object sender, EventArgs eventArgs)
        {
            if (null == secSignerProcess)
            {
                MessageBox.Show("SecSigner process needs to be created first.");
                return;
            }

            if (signature == null)
            {
                MessageBox.Show("Please load a signature to be verified first.");
                return;
            }

            if (document == null)
            {
                MessageBox.Show("Please load a document to be verified first.");
                return;
            }

            VerifyReq verifyReq = new VerifyReq();

            SignatureRecord sigRecord = new SignatureRecord();
            sigRecord.Document = File.ReadAllBytes(docFileNameWithPath);
            sigRecord.Signature = File.ReadAllBytes(sigFileNameWithPath);

            SignatureRecord[] sigRecords = new SignatureRecord[1];
            sigRecords[0] = sigRecord;

            verifyReq.SignatureRecord = sigRecords;

            VerifyRes verifyRes;
            try
            {
                verifyRes = secSigner.verify(verifyReq);
            }
            catch (Exception e)
            {
                MessageBox.Show("SecSigner.verify() failed: " + e);
                return;
            }

            SignatureRecord returnedSignatureRecord = verifyRes.SignatureRecord[0];

            int verifyStatus = verifyRes.ResultCode;
            if (verifyStatus == 1) // See the JavaDoc of seccommerce.secsignersigg.SecSignerConstants for the meaning of the return codes
            {
                MessageBox.Show("The signature is valid.");
            }
            else
            {
                MessageBox.Show("The signature verification returned the status " + verifyStatus +
                    ". Please see the JavaDoc of seccommerce.secsignersigg.SignatureConstants for the meaning.");
            }

            // Optionally save the verification report generated by SecSigner
            if (null != returnedSignatureRecord.VerificationReports)
            {
                if (returnedSignatureRecord.VerificationReports.Length > 0)
                {
                    string verificationReportFileNameWithPath = sigFileNameWithPath + "-verify-report.html";
                    MessageBox.Show("Writing the verification report to " + verificationReportFileNameWithPath);
                    File.WriteAllText(verificationReportFileNameWithPath, returnedSignatureRecord.VerificationReports[0]);
                }
            }
        }

        /**
         * Terminates SecSigner's Java process.
         */
        private void endSecSigner_Click(object sender, EventArgs e)
        {
            if (null == secSignerProcess)
            {
                MessageBox.Show("There is no SecSigner process to kill.");
                return;
            }

            secSignerProcess.Kill();
            secSignerProcess = null;

            MessageBox.Show("SecSigner killed.");
        }

        /**
         * Opens a document to be signed.
         */
        private void bOpenFileToSign_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            labelOpenFile.Text = "";

            if (DialogResult.OK == dlg.ShowDialog())
            {
                try
                {
                    foreach (string file in dlg.FileNames)
                    {
                        docFileNameWithPath = file;
                        document = File.ReadAllBytes(docFileNameWithPath);
                    }
                    labelOpenFile.Text = docFileNameWithPath + " loaded";
                    sigFileNameWithPath = docFileNameWithPath + ".pkcs7";
                }
                catch (IOException ex)
                {
                    document = null;
                    MessageBox.Show("File to sign could not be opened: " + ex.Message);
                    labelOpenFile.Text = "";
                }
            }
        }

        /**
         * Opens a signature to be verified.
         */
        private void bOpenSig_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            dlg.Filter = "CAdES signatures (*.pkcs7)|*.pkcs7|All files (*.*)|*.*";
            labelOpenSigFile.Text = "";

            if (DialogResult.OK == dlg.ShowDialog())
            {
                try
                {
                    foreach (string file in dlg.FileNames)
                    {
                       sigFileNameWithPath = file;
                       signature = File.ReadAllBytes(sigFileNameWithPath);
                    }
                    labelOpenSigFile.Text = sigFileNameWithPath + " loaded.";
                }
                catch (IOException ex)
                {
                    document = null;
                    MessageBox.Show("Signature to verify could not be opened: " + ex.Message);
                    labelOpenSigFile.Text = "";
                }
            }
        }

        /**
         * Opens a document to be verified together with a signature.
         */
        private void bOpenDocToVerify_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            labelOpenFile.Text = "";

            if (DialogResult.OK == dlg.ShowDialog())
            {
                try
                {
                    foreach (string file in dlg.FileNames)
                    {
                        docFileNameWithPath = file;
                        document = File.ReadAllBytes(file);
                    }
                    labelOpenFile.Text = docFileNameWithPath + " loaded";
                }
                catch (IOException ex)
                {
                    document = null;
                    MessageBox.Show("Document to verify could not be opened: " + ex.Message);
                    labelOpenFile.Text = "";
                }

            }
        }

        /**
         * Makes sure that the SecSigner Java process is killed when this applications ends.
         * Otherwise the SecSigner Java process would still block the SOAP server port and
         * no new SecSigner process could bind to it until the user ends SecSigner using taskmgr.
         */
        public void OnApplicationExit(object sender, EventArgs e)
        {
            if (null != secSignerProcess)
            {
                secSignerProcess.Kill();
                secSignerProcess = null;
            }
        }
    }
}
