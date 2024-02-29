namespace SecSignerSoapExample
{
    partial class SecSignerSoapExampleForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SecSignerSoapExampleForm));
            this.bSign = new System.Windows.Forms.Button();
            this.bVerify = new System.Windows.Forms.Button();
            this.bInit = new System.Windows.Forms.Button();
            this.bDestroy = new System.Windows.Forms.Button();
            this.lInit = new System.Windows.Forms.Label();
            this.lSign = new System.Windows.Forms.Label();
            this.lVerify = new System.Windows.Forms.Label();
            this.lDestroy = new System.Windows.Forms.Label();
            this.bOpenFileToSign = new System.Windows.Forms.Button();
            this.bSign100 = new System.Windows.Forms.Button();
            this.bOpenDocToVerify = new System.Windows.Forms.Button();
            this.bOpenSig = new System.Windows.Forms.Button();
            this.lHeading = new System.Windows.Forms.Label();
            this.lLogo = new System.Windows.Forms.Label();
            this.labelOpenFile = new System.Windows.Forms.Label();
            this.labelOpenSigFile = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // bSign
            // 
            this.bSign.Location = new System.Drawing.Point(283, 209);
            this.bSign.Name = "bSign";
            this.bSign.Size = new System.Drawing.Size(110, 30);
            this.bSign.TabIndex = 2;
            this.bSign.Text = "Sign once";
            this.bSign.UseVisualStyleBackColor = true;
            this.bSign.Click += new System.EventHandler(this.sign_Click);
            // 
            // bVerify
            // 
            this.bVerify.Location = new System.Drawing.Point(283, 316);
            this.bVerify.Name = "bVerify";
            this.bVerify.Size = new System.Drawing.Size(110, 30);
            this.bVerify.TabIndex = 6;
            this.bVerify.Text = "Verify";
            this.bVerify.UseVisualStyleBackColor = true;
            this.bVerify.Click += new System.EventHandler(this.verify_Click);
            // 
            // bInit
            // 
            this.bInit.Location = new System.Drawing.Point(17, 108);
            this.bInit.Name = "bInit";
            this.bInit.Size = new System.Drawing.Size(110, 30);
            this.bInit.TabIndex = 0;
            this.bInit.Text = "Create SecSigner";
            this.bInit.UseVisualStyleBackColor = true;
            this.bInit.Click += new System.EventHandler(this.createSecSigner_Click);
            // 
            // bDestroy
            // 
            this.bDestroy.Location = new System.Drawing.Point(17, 426);
            this.bDestroy.Name = "bDestroy";
            this.bDestroy.Size = new System.Drawing.Size(110, 30);
            this.bDestroy.TabIndex = 7;
            this.bDestroy.Text = "Destroy SecSigner";
            this.bDestroy.UseVisualStyleBackColor = true;
            this.bDestroy.Click += new System.EventHandler(this.endSecSigner_Click);
            // 
            // lInit
            // 
            this.lInit.AutoSize = true;
            this.lInit.Font = new System.Drawing.Font("Candara", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lInit.Location = new System.Drawing.Point(12, 70);
            this.lInit.Name = "lInit";
            this.lInit.Size = new System.Drawing.Size(123, 26);
            this.lInit.TabIndex = 4;
            this.lInit.Text = "Initialization";
            // 
            // lSign
            // 
            this.lSign.AutoSize = true;
            this.lSign.Font = new System.Drawing.Font("Candara", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lSign.Location = new System.Drawing.Point(12, 171);
            this.lSign.Name = "lSign";
            this.lSign.Size = new System.Drawing.Size(175, 26);
            this.lSign.TabIndex = 5;
            this.lSign.Text = "Signature creation";
            // 
            // lVerify
            // 
            this.lVerify.AutoSize = true;
            this.lVerify.Font = new System.Drawing.Font("Candara", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lVerify.Location = new System.Drawing.Point(12, 278);
            this.lVerify.Name = "lVerify";
            this.lVerify.Size = new System.Drawing.Size(201, 26);
            this.lVerify.TabIndex = 6;
            this.lVerify.Text = "Signature verification";
            // 
            // lDestroy
            // 
            this.lDestroy.AutoSize = true;
            this.lDestroy.Font = new System.Drawing.Font("Candara", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lDestroy.Location = new System.Drawing.Point(12, 388);
            this.lDestroy.Name = "lDestroy";
            this.lDestroy.Size = new System.Drawing.Size(119, 26);
            this.lDestroy.TabIndex = 7;
            this.lDestroy.Text = "Termination";
            // 
            // bOpenFileToSign
            // 
            this.bOpenFileToSign.Location = new System.Drawing.Point(17, 209);
            this.bOpenFileToSign.Name = "bOpenFileToSign";
            this.bOpenFileToSign.Size = new System.Drawing.Size(110, 30);
            this.bOpenFileToSign.TabIndex = 1;
            this.bOpenFileToSign.Text = "Open file to sign";
            this.bOpenFileToSign.UseVisualStyleBackColor = true;
            this.bOpenFileToSign.Click += new System.EventHandler(this.bOpenFileToSign_Click);
            // 
            // bSign100
            // 
            this.bSign100.Location = new System.Drawing.Point(410, 209);
            this.bSign100.Name = "bSign100";
            this.bSign100.Size = new System.Drawing.Size(110, 30);
            this.bSign100.TabIndex = 3;
            this.bSign100.Text = "Sign 25 times";
            this.bSign100.UseVisualStyleBackColor = true;
            this.bSign100.Click += new System.EventHandler(this.bSign100_Click);
            // 
            // bOpenDocToVerify
            // 
            this.bOpenDocToVerify.Location = new System.Drawing.Point(17, 316);
            this.bOpenDocToVerify.Name = "bOpenDocToVerify";
            this.bOpenDocToVerify.Size = new System.Drawing.Size(110, 30);
            this.bOpenDocToVerify.TabIndex = 4;
            this.bOpenDocToVerify.Text = "Open document";
            this.bOpenDocToVerify.UseVisualStyleBackColor = true;
            this.bOpenDocToVerify.Click += new System.EventHandler(this.bOpenDocToVerify_Click);
            // 
            // bOpenSig
            // 
            this.bOpenSig.Location = new System.Drawing.Point(145, 316);
            this.bOpenSig.Name = "bOpenSig";
            this.bOpenSig.Size = new System.Drawing.Size(110, 30);
            this.bOpenSig.TabIndex = 5;
            this.bOpenSig.Text = "Open signature";
            this.bOpenSig.UseVisualStyleBackColor = true;
            this.bOpenSig.Click += new System.EventHandler(this.bOpenSig_Click);
            // 
            // lHeading
            // 
            this.lHeading.AutoSize = true;
            this.lHeading.Font = new System.Drawing.Font("Candara", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lHeading.Location = new System.Drawing.Point(12, 18);
            this.lHeading.Name = "lHeading";
            this.lHeading.Size = new System.Drawing.Size(271, 26);
            this.lHeading.TabIndex = 12;
            this.lHeading.Text = "SecSigner SOAP call example";
            // 
            // lLogo
            // 
            this.lLogo.Image = ((System.Drawing.Image)(resources.GetObject("lLogo.Image")));
            this.lLogo.Location = new System.Drawing.Point(323, 9);
            this.lLogo.Name = "lLogo";
            this.lLogo.Size = new System.Drawing.Size(197, 44);
            this.lLogo.TabIndex = 13;
            // 
            // labelOpenFile
            // 
            this.labelOpenFile.AutoSize = true;
            this.labelOpenFile.Font = new System.Drawing.Font("Candara", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelOpenFile.Location = new System.Drawing.Point(14, 253);
            this.labelOpenFile.Name = "labelOpenFile";
            this.labelOpenFile.Size = new System.Drawing.Size(0, 15);
            this.labelOpenFile.TabIndex = 14;
            // 
            // labelOpenSigFile
            // 
            this.labelOpenSigFile.AutoSize = true;
            this.labelOpenSigFile.Font = new System.Drawing.Font("Candara", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelOpenSigFile.Location = new System.Drawing.Point(14, 359);
            this.labelOpenSigFile.Name = "labelOpenSigFile";
            this.labelOpenSigFile.Size = new System.Drawing.Size(0, 15);
            this.labelOpenSigFile.TabIndex = 15;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(536, 476);
            this.Controls.Add(this.bInit);
            this.Controls.Add(this.labelOpenSigFile);
            this.Controls.Add(this.labelOpenFile);
            this.Controls.Add(this.lLogo);
            this.Controls.Add(this.lHeading);
            this.Controls.Add(this.bOpenSig);
            this.Controls.Add(this.bOpenDocToVerify);
            this.Controls.Add(this.bSign100);
            this.Controls.Add(this.bOpenFileToSign);
            this.Controls.Add(this.lDestroy);
            this.Controls.Add(this.lVerify);
            this.Controls.Add(this.lSign);
            this.Controls.Add(this.lInit);
            this.Controls.Add(this.bDestroy);
            this.Controls.Add(this.bVerify);
            this.Controls.Add(this.bSign);
            this.Name = "Form1";
            this.Text = "SecSigner SOAP call example";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button bSign;
        private System.Windows.Forms.Button bVerify;
        private System.Windows.Forms.Button bInit;
        private System.Windows.Forms.Button bDestroy;
        private System.Windows.Forms.Label lInit;
        private System.Windows.Forms.Label lSign;
        private System.Windows.Forms.Label lVerify;
        private System.Windows.Forms.Label lDestroy;
        private System.Windows.Forms.Button bOpenFileToSign;
        private System.Windows.Forms.Button bSign100;
        private System.Windows.Forms.Button bOpenDocToVerify;
        private System.Windows.Forms.Button bOpenSig;
        private System.Windows.Forms.Label lHeading;
        private System.Windows.Forms.Label lLogo;
        private System.Windows.Forms.Label labelOpenFile;
        private System.Windows.Forms.Label labelOpenSigFile;
    }
}

