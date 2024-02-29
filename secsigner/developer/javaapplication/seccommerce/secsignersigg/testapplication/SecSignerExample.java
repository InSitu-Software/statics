// $Id: SecSignerExample.java,v 1.64 2015/03/19 20:47:38 tk Exp $
// $Source: /encfs/checkout/antonio/cvs/SecCommerceDev/seccommerce/secsignersigg/testapplication/SecSignerExample.java,v $
package seccommerce.secsignersigg.testapplication;

import seccommerce.pdf.PDFAnnotationDataItem;
import seccommerce.secsignersigg.SecSigner;
import seccommerce.secsignersigg.SecSignerConstants;
import seccommerce.secsignersigg.SecSignerInitResult;
import seccommerce.secsignersigg.SignatureRecord;
import seccommerce.xml.dsig.XPathTransformFilter;

import java.io.*;
import java.util.Properties;

import javax.swing.JFrame;

/**
 * Example for integration of SecSigner in a Java application.
 *
 * @version $Id: SecSignerExample.java,v 1.64 2015/03/19 20:47:38 tk Exp $
 * @author SecCommerce Informationssysteme GmbH, Hamburg
 */
public class SecSignerExample
{
    /**
     * directory where SecSigner.jar and scdriver.jar are found
     */
    private static final String SECSIGNER_PATH = "/X/SecCommerceDev/";

    /**
     * path to SecSigner's property file
     */
    private static final String SECSIGNER_PROPERTY_FILE = "/X/SecCommerceDev/secsigner.properties";

    /**
     * SecSigner
     */
    private SecSigner secSigner;

    /**
     * example data to be signed
     */
    private static final byte[] DOCUMENT;

    /**
     * example data to be signed
     */
    private static final byte[] DOCUMENT_XML;

    /**
     * example data to be signed for multi-signature
     */
    private static final int MULTI_DOCUMENT_COUNT = 10;
    private static final byte[][] MULTI_DOCUMENT = new byte[MULTI_DOCUMENT_COUNT][];
    static
    {
        try
        {
            DOCUMENT = "Dieser Satz wird unterschrieben. Hier koennte\nauch der Inhalt einer Datei stehen.".getBytes("UTF-8");

            DOCUMENT_XML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><test:testdoc xmlns:test=\"http://www.seccommerce.de/namespaceDecl\"><test:car><test:color>blue</test:color><test:speed>100</test:speed></test:car></test:testdoc>".getBytes("UTF-8");

            for (int i=0; i<MULTI_DOCUMENT_COUNT; i++)
            {
                MULTI_DOCUMENT[i] = ("Dieser " + (i+1) + ". Satz wird unterschrieben.\nHier könnte auch der Inhalt einer Datei stehen.").getBytes("UTF-8");
            }
        }
        catch (UnsupportedEncodingException ex)
        {
            // impossible, every JVM must know UTF-8
            throw new IllegalStateException("JVM does not know UTF-8: " + ex.getMessage());
        }
    }

    /**
     * document
     */
    private byte[] testDocument;

    /**
     * signature
     */
    private byte[] testSignature;

    /**
     * encrypted signature
     */
    private byte[] testEncryptedSignature;

    /**
     * encrypted data (without signature)
     */
    private byte[] testEncryptedData;

    /**
     * test data for multi signature verification
     */
    private SignatureRecord[] testSignatureRecordArray;

    /**
     * Constructor for the example.
     *
     * @param parentFrame reference to the parent frame
     * @param secSignerProperties SecSigner properties
     * @param secSignerInstallPath installation path of SecSigner and the application
     */
    public SecSignerExample(JFrame parentFrame, Properties secSignerProperties, String secSignerInstallPath)
    {
        try
        {
            secSigner = new SecSigner(parentFrame, secSignerProperties, secSignerInstallPath);
        }
        catch(Exception e)
        {
            System.out.println("SecSigner Initialisierungsfehler: " + e.toString());
            Thread.dumpStack();
            System.exit(2);
        }
    }

    /**
     * Start point.
     *
     * @param args no command line arguments used
     */
    public static void main(String[] args)
    {
        // load SecSigner's property file
        Properties secSignerProperties = new Properties();
        try
        {
            FileInputStream propertyStream = new FileInputStream(SECSIGNER_PROPERTY_FILE);
            secSignerProperties.load(propertyStream);
        }
        catch (Exception e)
        {
            System.out.println("SecSigner-Property-Datei konnte nicht geladen werden: " + e.toString());
            System.exit(1);
        }

        // create a frame of this example application which will call SecSigner.
        JFrame appFrame = new JFrame("Frame der Applikation, welche SecSigner aufruft.");
        appFrame.setSize(555, 111);
        appFrame.setVisible(true);

        // create SecSigner
        SecSignerExample secSignerExample = new SecSignerExample(appFrame, secSignerProperties, SECSIGNER_PATH);

        // optional: init card readers
        secSignerExample.testInitSignUnits();

        // sign the single example
        secSignerExample.testSign();

        // check the signature of the example
          secSignerExample.testVerify();

        // sign several examples
        secSignerExample.testMultiSign();

        // check the signatures of several examples
        secSignerExample.testMultiVerify();

        // check a certificate
        secSignerExample.testCertVerify();

        // check a timestamp for an unsigned document
        secSignerExample.testTimestampVerify();

        // encrypt data
       secSignerExample.testEncryptData();

        // decrypt data
        secSignerExample.testDecryptData();

        // check whether a PDF document is PDF/A compliant.
        secSignerExample.testPDFACompliance();

        // change the PIN
        secSignerExample.testChangePin();

        // close SecSigner
        secSignerExample.testClose();

        appFrame.dispose();
    }


    /**
     * Optional: initializes the card readers and returns the certificates of
     * the smart card found in the first reader.
     */
    private synchronized void testInitSignUnits()
    {
        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        String sigCertFileName = "/seccom/temp/sigCert.der";
        String authCertFileName = "/seccom/temp/authCert.der";
        String encrCertFileName = "/seccom/temp/encrCert.der";

        try
        {
            // optional: Zufallszahlen von der Karte lesen und damit PRNG initialisieren
            secSigner.setCollectRandom(true);

            SecSignerInitResult secSignerInitResult = secSigner.initSignUnits(modal);

            FileOutputStream certFos = new FileOutputStream(sigCertFileName);
            certFos.write(secSignerInitResult.getSigCert());
            certFos.close();

            certFos = new FileOutputStream(authCertFileName);
            certFos.write(secSignerInitResult.getAuthCert());
            certFos.close();

            certFos = new FileOutputStream(encrCertFileName);
            certFos.write(secSignerInitResult.getDecryptCert());
            certFos.close();
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Creates a signature.
     */
    private synchronized void testSign()
    {
        // Typ der zu signierenden Daten fuer die passende Anzeige im SecSigner-Fenster
        //Integer documentType = SecSignerConstants.SIGNDATATYPE_IMAGE;
        //Integer documentType = SecSignerConstants.SIGNDATATYPE_PDF;
        //Integer documentType = SecSignerConstants.SIGNDATATYPE_ZKS;
    	Integer documentType = SecSignerConstants.SIGNDATATYPE_PLAINTEXT;
        //Integer documentType = SecSignerConstants.SIGNDATATYPE_XML;
        //Integer documentType = SecSignerConstants.SIGNDATATYPE_BINARY;

        // optional: vorhandene PKCS#7-Signatur, der eine weitere Unterschrift für die selben Daten hinzugefuegt werden soll
        byte[] oldSignature = null;

        // optional: MIME-Type der zu signierenden Daten, um den passenden externen Viewer zu starten
        String mimeType = null;

        // Daten, die signiert werden sollen
        byte[] document = DOCUMENT;
        //byte[] document = DOCUMENT_XML;

        // Nur fuer XML-DSig:
        // XPath zu dem XML-Knoten unter dem der Signature-Knoten eingefuegt werden soll.
        // Wenn der Wert null ist, wird der Signature-Knoten direkt unter dem Wurzelknoten eingefuegt.
        String xmlDSigNodePath = "/test:testdoc/test:car";
        String xmlDSigNameSpaceName = "dsig";

        // Nur fuer XML-DSig: Filter-Ausdruck zur Bestimmung der zu signierenden XML-Knoten.
        XPathTransformFilter xPathTransformFilter = new XPathTransformFilter();
        xPathTransformFilter.setTransformMethod(XPathTransformFilter.INTERSECT);
        xPathTransformFilter.setXPathExpression("//*");

        // Name des gewuenschten Hashalgorithmus.
        String hashAlgName = null; // null = "autoselect", "SHA1", "SHA256", "SHA512", ...

        // optional: Die Signatur wird fuer diese Zertifikate verschluesselt
        // read encryption certificate
        byte encryptCert[] = null;
        String encryptCertFileName = "x:/certs/telesec/tkTelesec6Encr.der";
        try
        {
            FileInputStream encrCertFis = new FileInputStream(encryptCertFileName);
            ByteArrayOutputStream encrCertBaos = new ByteArrayOutputStream();
            moveStreamData(encrCertFis, encrCertBaos);
            encryptCert = encrCertBaos.toByteArray();
            encrCertFis.close();

        }
        catch (Exception e)
        {
            System.err.println("Could not read encryption certificate '" + encryptCertFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(3);
        }

        byte encryptCerts[][] = new byte[1][];
        encryptCerts[0] = encryptCert;

        encryptCerts=null; // no encryption

        // optional: Angezeigter Dateiname
        String documentFileName = "doc.xml";
        // optional: URL
        String documentURL = null;

        // Das Zertifikat des Unterzeichners mit in die Signatur aufnehmen. Sehr empfohlen,
        // um die Pruefung zu erleichtern.
        boolean includeSignersCert = true;

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        // Signatur starten
        SignatureRecord signatureRecord = new SignatureRecord();
        signatureRecord.setDocumentType(documentType);
        signatureRecord.setOldSignature(oldSignature);
        signatureRecord.setDocument(document);
        signatureRecord.setDocumentFileName(documentFileName);
        signatureRecord.setDocumentUrl(documentURL);
        signatureRecord.setHashAlgorithm(hashAlgName);
        signatureRecord.setPssPadding(true);
        signatureRecord.setIncludeSignersCertificate(new Boolean(includeSignersCert));
        //signatureRecord.setXmlDSigNameSpaceName(xmlDSigNameSpaceName);
        signatureRecord.setXmlDSigNodePath(xmlDSigNodePath);
        signatureRecord.setXmlDSigTransformFilter(new XPathTransformFilter[]{xPathTransformFilter});

        // optional: PDF Signatur Annotation 
        PDFAnnotationDataItem annotationDataItem = new PDFAnnotationDataItem();
        annotationDataItem.setPdfSigPosition(999996080);
        //signatureRecord.setPDFAnnotation(annotationDataItem);

        // Typ der Signatur: PKCS#7 SignedData oder Signatur im PDF
        //signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PDF);
        signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PKCS7);
        //signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PKCS7_EMBEDDED);
        //signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_XML_DSIG);

        SignatureRecord[] signatureRecordArray = new SignatureRecord[1];
        signatureRecordArray[0] = signatureRecord;

        // optional: Dieser Softwareschluessel (PKCS#8 oder PKCS#12) soll verwendet werden
        byte[] signatureKey = null;

        // optional: Dieses Signaturzertifikat soll verwendet werden
        byte[] signatureCert = null;

        // optional: Attributzertifikate in die Signatur aufnehmen. Der Anwender kann auch
        // selbst Attributzertifikate aus Dateien laden.
        byte[][] attributeCerts = null;

        // get random numbers from a smartcard if data shall be encrypted now or later
        // secSigner.setCollectRandom(true);

        try
        {
            secSigner.sign(signatureRecordArray, encryptCerts, signatureKey, signatureCert, attributeCerts, modal);

            // save returned data for verification
            testDocument = document;
            testSignature = signatureRecord.getSignature();



            testEncryptedSignature = signatureRecord.getSignatureCiphered();
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Creates several signatures.
     */
    private synchronized void testMultiSign()
    {
        SignatureRecord[] signatureRecordArray = new SignatureRecord[MULTI_DOCUMENT_COUNT];
        for (int i=0; i<MULTI_DOCUMENT_COUNT; i++)
        {
            signatureRecordArray[i] = new SignatureRecord();
            signatureRecordArray[i].setDocument(MULTI_DOCUMENT[i]);

            // Typ der zu signierenden Daten fuer die passende Anzeige im SecSigner-Fenster
            signatureRecordArray[i].setDocumentType(SecSignerConstants.SIGNDATATYPE_PLAINTEXT);

            // Typ der Signatur: PKCS#7 SignedData oder Signatur im PDF
            //signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PDF);
            //signatureRecord.setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PKCS7_EMBEDDED);
            signatureRecordArray[i].setSignatureFormatType(SecSignerConstants.SIGNATURE_FORMAT_PKCS7);

            // optional: Angezeigter Dateiname, URL oder sonstiger Names des Dokumentes
            signatureRecordArray[i].setDocumentUrl("Text Nr." + i);

            // optional: Dateiname des Dokumentes
            signatureRecordArray[i].setDocumentFileName("filename" + i + ".txt");

            // optional: vorhandene PKCS#7-Signatur, der eine weitere Unterschrift für die selben Daten hinzugefuegt werden soll
            signatureRecordArray[i].setOldSignature(null);

            // Name des gewuenschten Hashalgorithmus.
            signatureRecordArray[i].setHashAlgorithm(null); // null = "autoselect", "SHA1", "SHA256", "SHA512", ...

            // Paddingverfahren fuer RSA: true fuer RSA-PSS, false fuer PKCS#1 v1.5
            signatureRecordArray[i].setPssPadding(true);

            // Das Zertifikat des Unterzeichners mit in die Signatur aufnehmen. Sehr empfohlen,
            // um die Pruefung zu erleichtern.
            signatureRecordArray[i].setIncludeSignersCertificate(new Boolean(true));
        }

        // optional: Die Signaturen werden fuer diese Zertifikate verschluesselt
        byte[][] encryptCerts = null;

        // optional: Dieser Softwareschluessel (PKCS#8 oder PKCS#12) soll verwendet werden
        byte[] signatureKey = null;

        // optional: Dieses Signaturzertifikat soll verwendet werden
        byte[] signatureCert = null;

        // optional: Diese Attributzertifikate werden allen Signaturen hinzugefuegt
        byte[][] attributeCerts = null;

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        // get random numbers from a smartcard if data shall be encrypted now or later
        // secSigner.setCollectRandom(true);

        try
        {
            // Signatur starten
            secSigner.sign(signatureRecordArray, encryptCerts, signatureKey, signatureCert, attributeCerts, modal);
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }

        // Save returned data for verification.
        testSignatureRecordArray = signatureRecordArray;
    }


    /**
     * Verifies the signature over the example text.
     */
    private synchronized void testVerify()
    {
        SignatureRecord signatureRecord = new SignatureRecord();

        // Unterschriebene Daten oder null, falls in der PKCS#7-Signatur enthalten
        signatureRecord.setDocument(testDocument); // null, wenn schon in Signatur enthalten, z.B. bei Entschluesselung

        // DER kodierte PKCS#7-Signatur
        signatureRecord.setSignature(testSignature); // Test der Verifikation
        //signatureRecord.setSignature(testEncryptedSignature); // Test der Entschluesselung und Verifikation

        // optional: DER kodierte OCSP-Antwort fuer das Signaturzertifikat des ersten Unterzeichners
        signatureRecord.setOCSPResponseFirstSigner(null);

        // optional: Dateinamenserweiterung der unterschriebenen Daten, um den externen Viewer zu starten
        signatureRecord.setDocumentFileNameExtension(null);

        // optional: Dateiname des Dokumentes
        signatureRecord.setDocumentFileName(null);

        // optional: Angezeigter Dateiname, URL oder sonstiger Names des Dokumentes
        signatureRecord.setDocumentUrl("Testdokument");

        // optional: Angezeigter Dateiname der Signaturdatei
        signatureRecord.setSignatureFileName("geheim.pkcs7");

        // optional: Das Zertifikat des Unterzeichners, falls es in der Signatur fehlt
        String sigCertFileName = "x:/certs/telesec/2007-2048/tkTelesec2048-1-Sig.der";
        byte externalSignersCerts[][] = new byte[1][];
        try
        {
            FileInputStream sigCertFis = new FileInputStream(sigCertFileName);
            ByteArrayOutputStream encrCertBaos = new ByteArrayOutputStream();
            moveStreamData(sigCertFis, encrCertBaos);
            externalSignersCerts[0] = encrCertBaos.toByteArray();
            sigCertFis.close();
        }
        catch (Exception e)
        {
            System.err.println("Could not read signature certificate '" + sigCertFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(3);
        }

        // Nur fuer XML-DSig:
        // XPath zum Signature-Knoten. Wenn der Wert null ist, werden alle Signature-Knoten gesucht und geprueft.
        signatureRecord.setXmlDSigNodePath("/test:testdoc/test:car/dsig:Signature");

        // Die Moeglichkeit zur Signatur der verifizierten Daten kann abgeschaltet werden (auch in der Propertiesdatei).
        boolean allowSig = true;

        // Wenn Signatur oder signierte Daten nicht uebergeben wurden, zeigt SecSigner einen Datei-Dialog an
        // oder gibt sofort einen Fehler zurueck.
        boolean offerFileOpenDlg = true;

        // Die OCSP-Pruefung kann ohne Eingriff des Benutzers direkt aufgerufen werden.
        boolean oscpMandatory = false;

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        // Die Verifizierung kann mit mehreren Signaturen aufgerufen werden.
        SignatureRecord[] signatureRecordArray = new SignatureRecord[1];
        signatureRecordArray[0] = signatureRecord;

        try
        {
            // start verification
            int ret = secSigner.verify(signatureRecordArray, externalSignersCerts, allowSig, offerFileOpenDlg, oscpMandatory, modal).getStatus();
            switch (ret)
            {
                case SecSignerConstants.SECSIGNER_VERIFY_NOTINITED:
                    System.out.println("SecSigner nicht initialisiert");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID:
                    System.out.println("Signaturprüfung erfolgreich, Signatur gültig");

                    // signierte Daten, ggf. entschluesselt
                    byte plainData[] = signatureRecord.getDocument();
                    System.out.println("Signierte Daten = " + new String(plainData, "UTF-8"));
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID_RESIGNED:
                    System.out.println("Signaturprüfung erfolgreich, Signatur gültig, Der Anwender hat die Daten nach der Prüfung auch signiert.");
                    // return signatureRecord.getSignature(); // Dies ist die neue Signatur
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID_CERTINVALID:
                    // only possible with a test certificate in a test mode SecSigner
                    System.out.println("Signaturprüfung erfolgreich, Signatur mathematisch gültig, Signaturzertifikat ungültig.");

                    // signierte Daten, ggf. entschluesselt
                    plainData = signatureRecord.getDocument();
                    System.out.println("Signierte Daten = " + new String(plainData, "UTF-8"));
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID_CERTREVOKED:
                    System.out.println("Signaturprüfung erfolgreich, Signatur mathematisch gültig, Signaturzertifikat gesperrt.");

                    // signierte Daten, ggf. entschluesselt
                    plainData = signatureRecord.getDocument();
                    System.out.println("Signierte Daten = " + new String(plainData, "UTF-8"));
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_ALGORITHM_EXPIRED:
                    System.out.println("Signaturprüfung erfolgreich, Signaturgültig ohne Beweisdokument (ERS) nicht feststellbar, da ein Algorithmus nicht mehr gültig ist.");

                    // signierte Daten, ggf. entschluesselt
                    plainData = signatureRecord.getDocument();
                    System.out.println("Signierte Daten = " + new String(plainData, "UTF-8"));
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_DECODED_UNSIGNED_DATA:
                    System.out.println("Unsignierte Daten wurden entschlüsselt.");
                    plainData = signatureRecord.getDocument();
                    System.out.println("Signierte Daten = " + new String(plainData, "UTF-8"));
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_CERTVALID:
                    System.out.println("Zertifikatprüfung erfolgreich, Zertifikat gültig");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_CERTINVALID:
                    System.out.println("Zertifikatprüfung erfolgreich, Zertifikat NICHT gültig");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREINVALID:
                    System.out.println("Signaturprüfung erfolgreich, Signatur nicht gültig");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_NO_DATA:
                    System.out.println("Fehler bei der Signaturprüfung: Es wurden keine Daten übergeben.");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_DATA_MISMATCH:
                    System.out.println("Fehler bei der Signaturprüfung: Die Signatur passt nicht zum Dokument.");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_DECODE_FAILED:
                    System.out.println("Fehler bei der Signaturprüfung: Die Signatur konnte nicht dekodiert werden.");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_ERROR:
                    System.out.println("Fehler bei der Signaturprüfung");
                    break;
                default:
                    // does not happen
                    System.out.println("Unbekannter Statuscode von der Signaturprüfung: " + ret);
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Verifies all signatures generated with 'testMultiSign'
     */
    private synchronized void testMultiVerify()
    {
        // optional: Das Zertifikat des Unterzeichners, falls es in der Signatur fehlt
        byte externalSignersCerts[][] = null;

        // Die Moeglichkeit zur Signatur der verifizierten Daten kann abgeschaltet werden (auch in der Propertiesdatei).
        boolean allowSig = true;

        // Wenn Signatur oder signierte Daten nicht uebergeben wurden, zeigt SecSigner einen Datei-Dialog an
        // oder gibt sofort einen Fehler zurueck.
        boolean offerFileOpenDlg = true;

        // Es muss eine OCSP-Anfrage gestellt werden
        boolean oscpMandatory = false;

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        try
        {
            // start verification
            switch (secSigner.verify(testSignatureRecordArray, externalSignersCerts, allowSig, offerFileOpenDlg, oscpMandatory, modal).getStatus())
            {
                case SecSignerConstants.SECSIGNER_VERIFY_NOTINITED:
                    System.out.println("SecSigner nicht initialisiert");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID:
                    System.out.println("Signaturprüfung erfolgreich, alle Signaturen gültig");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREVALID_RESIGNED:
                    System.out.println("Signaturprüfung erfolgreich, alle Signaturen gültig, Der Anwender hat die Daten nach der Prüfung auch signiert.");
                    //return testSignatureRecordArray[i].getSignature(); // Dies ist die neue Signatur
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_SIGNATUREINVALID:
                    System.out.println("Signaturprüfung erfolgreich, alle Signaturen nicht gültig");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_ERROR:
                    System.out.println("Fehler bei der Signaturprüfung");
                    break;
                default:
                    // does not happen
                    System.out.println("Unbekannter Statuscode von der Signaturprüfung");
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Verifies a certificate
     */
    private synchronized void testCertVerify()
    {
        // the signer's certificate
        String certFileName = "c:/cert.der";
        byte cert[] = null;
        try
        {
            FileInputStream certFis = new FileInputStream(certFileName);
            ByteArrayOutputStream certBaos = new ByteArrayOutputStream();
            moveStreamData(certFis, certBaos);
            cert = certBaos.toByteArray();
            certFis.close();
        }
        catch (Exception e)
        {
            System.err.println("Could not read certificate to be verified '" + certFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(4);
        }

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        try
        {
            // start verification
            switch (secSigner.verifyCertificate(cert, modal).getStatus())
            {
                case SecSignerConstants.SECSIGNER_VERIFY_NOTINITED:
                    System.out.println("SecSigner nicht initialisiert");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_CERTVALID:
                    System.out.println("Zertifikatsprüfung erfolgreich, Zertifikat gültig");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_CERTINVALID:
                    System.out.println("Zertifikatsprüfung erfolgreich, Zertifikat nicht gültig");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_ERROR:
                    System.out.println("Fehler bei der Zertifikatsprüfung");
                    break;
                default:
                    // does not happen
                    System.out.println("Unbekannter Statuscode von der Zertifikatsprüfung");
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Verifies a timestamp for an unsigned document. To verify a timestamp for signature see testVerify().
     */
    private synchronized void testTimestampVerify()
    {
        SignatureRecord signatureRecord = new SignatureRecord();

        // optional: Dateinamenserweiterung der unterschriebenen Daten, um den externen Viewer zu starten
        signatureRecord.setDocumentFileNameExtension(null);

        // optional: Dateiname des Dokumentes
        signatureRecord.setDocumentFileName(null);

        // optional: Angezeigter Dateiname, URL oder sonstiger Names des Dokumentes
        signatureRecord.setDocumentUrl("Testdokument");

        // read the timestamp from a file
        String timeStampFileName = "c:/temp/abc.txt.tsr";
        try
        {
            FileInputStream timeStampFis = new FileInputStream(timeStampFileName);
            ByteArrayOutputStream timeStampBaos = new ByteArrayOutputStream();
            moveStreamData(timeStampFis, timeStampBaos);
            signatureRecord.setTimestamp(timeStampBaos.toByteArray());
            timeStampFis.close();
        }
        catch (Exception e)
        {
            System.err.println("Could not read the timestamp '" + timeStampFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(3);
        }

        // read the document from a file
        String documentFileName = "c:/temp/abc.txt.pkcs7";
        try
        {
            FileInputStream documentFis = new FileInputStream(documentFileName);
            ByteArrayOutputStream documentBaos = new ByteArrayOutputStream();
            moveStreamData(documentFis, documentBaos);
            signatureRecord.setDocument(documentBaos.toByteArray());
            documentFis.close();
        }
        catch (Exception e)
        {
            System.err.println("Could not read the document '" + documentFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(3);
        }

        // Die Moeglichkeit zur Signatur der verifizierten Daten kann abgeschaltet werden (auch in der Propertiesdatei).
        boolean allowSig = false;

        // Wenn Signatur oder signierte Daten nicht uebergeben wurden, zeigt SecSigner einen Datei-Dialog an
        // oder gibt sofort einen Fehler zurueck.
        boolean offerFileOpenDlg = true;

        // Die OCSP-Pruefung kann ohne Eingriff des Benutzers direkt aufgerufen werden.
        boolean oscpMandatory = false;

        // not relevant here
        byte[][] externalSignersCerts = null;

        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        // Die Verifizierung kann mit mehreren Signaturen aufgerufen werden.
        SignatureRecord[] signatureRecordArray = new SignatureRecord[1];
        signatureRecordArray[0] = signatureRecord;

        try
        {
            // start verification
            int ret = secSigner.verify(signatureRecordArray, externalSignersCerts, allowSig, offerFileOpenDlg, oscpMandatory, modal).getStatus();
            switch (ret)
            {
                case SecSignerConstants.SECSIGNER_VERIFY_NOTINITED:
                    System.out.println("SecSigner nicht initialisiert");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_ALGORITHM_EXPIRED:
                    System.out.println("Signaturprüfung erfolgreich, Signaturgültig ohne Beweisdokument (ERS) nicht feststellbar, da ein Algorithmus nicht mehr gültig ist.");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_TIMESTAMP_VALID:
                    System.out.println("Zeitstempelprüfung erfolgreich, Zeitstempel gültig");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_TIMESTAMP_INVALID:
                    System.out.println("Zeitstempelprüfung erfolgreich, Zeitstempel NICHT gültig");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_NO_DATA:
                    System.out.println("Fehler bei der Signaturprüfung: Es wurden keine Daten übergeben.");
                    break;

                case SecSignerConstants.SECSIGNER_VERIFY_ERROR:
                    System.out.println("Fehler bei der Signaturprüfung");
                    break;

                default:
                    // should not happen
                    System.out.println("Unbekannter Statuscode von der Signaturprüfung: " + ret);
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Changes a PIN.
     */
    private synchronized void testChangePin()
    {
        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        try
        {
            secSigner.changePin(modal);
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Encrypts data (without signing them).
     */
    private synchronized void testEncryptData()
    {
        // optional: Die Daten werden fuer diese Zertifikate verschluesselt. Sonst file-open-dialog.
        byte[][] encryptCerts = null;

        try
        {
            testEncryptedData = secSigner.encryptDataOnly(DOCUMENT, encryptCerts);
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Decrypts data.
     */
    private synchronized void testDecryptData()
    {
        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        SignatureRecord signatureRecord = new SignatureRecord();

        signatureRecord.setSignature(testEncryptedData);

        boolean offerFileOpenDlg = true;

        try
        {
            switch (secSigner.decrypt(signatureRecord, offerFileOpenDlg, modal).getStatus())
            {
                case SecSignerConstants.SECSIGNER_VERIFY_NOTINITED:
                    System.out.println("SecSigner nicht initialisiert");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_DECODED_UNSIGNED_DATA:
                    System.out.println("Daten entschluesselt");
                    System.out.println("Daten=" + new String(signatureRecord.getDocument(), "UTF-8"));
                    break;
                default:
                    System.out.println("Unbekannter Statuscode von der Entschluesselung");
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }

    }

    /**
     * Gets random numbers.
     */
    private synchronized void testPDFACompliance()
    {
        // Der SecSigner-Dialog kann modal sein.
        boolean modal = true;

        SignatureRecord[] docs = new SignatureRecord[1];

        // PDF-A compliant document
        SignatureRecord signatureRecord = new SignatureRecord();

        //signatureRecord.setDocumentType(VerifySignatureProcessMgr.getDocumentType(pdfToProof, null));
        //signatureRecord.setDocumentUrl(files[i].getAbsolutePath());
        signatureRecord.setDocumentFileName("pdfA.pdf");
        docs[0] = signatureRecord;


        String pdfFileName = "c:/users/root/Desktop/pdfA.pdf";
        try
        {
            FileInputStream pdfFis = new FileInputStream(pdfFileName);
            ByteArrayOutputStream pdfBaos = new ByteArrayOutputStream();
            moveStreamData(pdfFis, pdfBaos);
            signatureRecord.setDocument(pdfBaos.toByteArray());
            pdfFis.close();
        }
        catch (Exception e)
        {
            System.err.println("Could not read PDF document '" + pdfFileName + "': " + e.getMessage());
            e.printStackTrace();
            System.exit(3);
        }

        try
        {
            switch (secSigner.verifyPDFACompliance(docs, modal).getStatus())
            {
                case SecSignerConstants.SECSIGNER_VERIFY_PDFA_COMPLIANT:
                    System.out.println("Das Dokument entspricht PDF/A");
                    break;
                case SecSignerConstants.SECSIGNER_VERIFY_PDFA_NOT_COMPLIANT:
                    System.out.println("Das Dokument entspricht nicht PDF/");
                    break;
                default:
                    System.out.println("Unbekannter Statuscode von der PDF/A-Pruefung");
            }
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }

    /**
     * Closes the SecSigner instance and releases the card reader.
     */
    private void testClose()
    {
        secSigner.close();
    }

    /**
     * Moves stream data from input stream to output stream. Will copy the complete stream.
     * @param is input stream
     * @param os output stream
     * @exception IOException Fehler beim Lesen oder Schreiben
     */
    public static void moveStreamData(InputStream is, OutputStream os) throws IOException
    {
        int copyBufferLen = 2000;
        byte[] copyBuffer = new byte[copyBufferLen];

        while (true)
        {
            int bytesRead = is.read(copyBuffer);
            if (0 > bytesRead)
            {
                break;
            }
            os.write(copyBuffer, 0, bytesRead);
        }
    }
}
