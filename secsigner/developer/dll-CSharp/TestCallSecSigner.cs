using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

/**
 * Example for calling SecSigner from a C# application.
 * 
 * The mode (signature or verification) is selected at the command line.
 * The file names of test data are also specified at the command line.
 * 
 * Command line parameters:
 * 1: Testmode 
 *          2: test signature of given documents
 *          3: test verification of given documents)
 *          4: test signature of given documents and restart SecSigner
 * 2: Name of configuration file, usually "secsigner.properties"
 * 3: Name of the installation path, usually "D:\SecSigner\"
 * 4: Maximum memory in MB for the Java VM
 * 5: Name of the directory where some test documents and signatures are taken from
 *
 * Command line example for signature: 
 * TestCallSecSignerDLLCSharp.exe 2 d:\SecSigner\secsigner.properties d:\SecSigner 256 d:\SecSigner\test\
 *
 * Command line example for verification:
 * TestCallSecSignerDLLCSharp.exe 3 d:\SecSigner\secsigner.properties d:\SecSigner 256 d:\SecSigner\test\
 *
 * @author SecCommerce Informationssysteme GmbH, Hamburg
 */
namespace TestCallSecSignerDLLCSharp
{
    // Document to be signed and its signature. 
    // If a value is missing the buffer shall be NULL and the length 0.
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct Document
    {
        public int version;                // version = 13
        public string documentFileName;    // 0x00 terminated, will be mapped to dataURL
        public IntPtr dataToBeSigned;      // data to be signed 
        public int dataToBeSignedLen;      // sig-mode: IN: length of the supplied data to be signed
                                           // verify-mode: IN: length of the supplied data which were signed
                                           // verify-mode: OUT: length of extracted data which were signed
        public int dataToBeSignedBufLen;   // verify-mode: IN: length of the buffer for the data which were signed
                                           //                  and are extracted from the signature by SecSigner
        public int documentType;           // type of data to be signed (SIGNDATATYPE_PLAINTEXT, ...)
        public int signatureFormatType;    // desired signature type (SIGNATUREFORMATTYPE_PKCS7, SIGNATUREFORMATTYPE_PDF, SIGNATUREFORMATTYPE_XMLDSIG)
        public string mimeType;            // mimetype of to be signed data (0x00 terminated), alternative to documentType
        public IntPtr oldSignature;        // optional: buffer for the old signature where the SignerInfo shall be added to
        public int oldSignatureLen;        // length of the buffer
        public IntPtr signature;           // sig-mode: buffer for the returned signature
                                           // verify-mode: signature to be verified
        public int signatureLen;           // sig-mode: IN: length of the buffer
                                           // sig-mode: OUT: length of the actual signature
                                           // verify-mode: IN: length of the signature
        public IntPtr encryptedSig;        // sig-mode: optional: buffer for the returned encrypted signature (if 
                                           //                     encryption certificates were supplied)
        public int encryptedSigLen;        // sig-mode: IN: length of the buffer
                                           // sig-mode: OUT: length of the actual encrypted signature
        public IntPtr ocspResponse;        // verify-mode: buffer for the returned OCSP response if the certificate is checked online
                                           //     or existing OCSP response for the given signature
        public int ocspResponseLen;        // verify-mode: IN: length of the supplied old OCSP response
                                           // verify-mode: OUT: length of the new OCSP response received by SecSigner
        public int ocspResponseBufLen;     // verify-mode: IN: length of the buffer for a new OCSP response received by SecSigner
        public IntPtr timeStamp;           // sig-mode: buffer for the returned time stamp if one is received
                                           // verify-mode: time stamp to be verified for the given signature
        public int timeStampLen;           // sig-mode: IN: length of the buffer
                                           // sig-mode: OUT: length of the actual time stamp
                                           // verify-mode: IN: length of the time stamp to be verified
        public IntPtr encryptedDoc;        // encrypt-mode: buffer for the returned encrypted document
        public int encryptedDocLen;        // encrypt-mode: IN: length of the buffer
                                           // encrypt-mode: OUT: length of the actual encrypted document

        public string signatureID;         // id which is inserted as id-attribute of the xmldsig-signature-node
        public string xmlDSigNodePath;     // xpath within a document to specify where to insert the xmldsig-signature
        public IntPtr xmlDSigFilterPaths;   // (an array of)xpath transform filter which defines the parts of the xml document which is signed.
                                            // xpath-transform filter could be an intersect, subtract and union filter
        public int numberOfXmlDSigFilterPaths; // the number of tranform filters
 
        public string xmlDSigNameSpaceName; // sig-mode: IN: the namespace name of the XML-DSig signature node. The URI is http://www.w3.org/2000/09/xmldsig#.
        public IntPtr evidenceRecordArray;  // BYTEARRAY*    // verify-mode: IN: evidence records for signatures using expired algorithms
        public int numberOfEvidenceRecords; // verify-mode: IN: number of supplied evidence records
        public string hashAlgorithm;        // sig-mode: IN: desired hash algorithm. If null then the default of the smart card will be used.
        public IntPtr pdfAnnotation;        // PDFANNOTATION*  // sig-mode: IN: optional visual representation of the signature in the PDF document
        public IntPtr verificationReport;   // verify-mode: the verification report
        public int verificationReportLen;   // verify-mode: the verification report length /  the length of suplied buffer for the verification report
        public bool setUseLegacyBmuXmlSigFormat; // Determines whether redundant signer information as separate XML nodes for waste signatures
											//     (BMU ZKS) shall be added to the signature and whether the signature shall conform
                                     		//     to an old XAdES version where SigningCertificate and SignerRole still have version 1.
    };

    // A buffer and its length
    struct ByteArray
    {
        public IntPtr data;                // data
        public int dataLen;                // data length
    };

    // xpath transform filter for xmldsig signatures
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct XPathTransformFilter
    {
        public int transformMethod;         // the transform method of this filter
                                            // 0 = intersect
                                            // 1 = subtract
                                            // 2 = union
        public string xpathExpr;            // the xpath-expression which selects xml-nodes. the xml-document is intersect/union/subtract with these nodes.

        public IntPtr xmlDSigNameSpaceMappings; // an array of namespace mappings which are used in transform filters to validate the filters xpath expression

        public int numberOfXmlDSigNamespaces; // number of mappings
    };


    // contains a mapping between namespace prefix and its URI, only used for xml dsig signatures
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct NameSpaceMapping
    {
        public string namespacePrefix;      // namespace prefix 
        public string namespaceURI;         // namespace uri
    };

    // Optional visual representation of a signature included into PDF documents
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct PdfAnnotation
    {
        public bool pdfDisplayAnnotation;          	// determine whether a verification item (JPG image) should be added to the PDF document.
        public IntPtr pdfSignatureImage;       // deprecated, not used any longer.
        public bool pdfSignatureImageText;         	// deprecated, not used any longer.
        public IntPtr pdfLinkImage;            // deprecated, not used any longer.
        public string pdfLinkUrl;                   // deprecated, not used any longer.
        public string pdfSignatureReason;           // signature reason text to be added onto the verification item image.
	    public string pdfSignatureTextAnnot;        // deprecated, not used any longer.
        public string pdfSignatureLocation;         // signature location text to be added onto the verification item image.
        
        public int pdfSignaturePosition;           	// position for a virtual box containing verification item image and link item image.
        public string pdfSigTextSizeAndPosition;    // size and position for text (signer, time, etc.) in signature annotation.
        public bool pdfScaleSigImage;              // deprecated, not used any longer.
        public bool pdfSigAnnotLabels;             	// determines whether signature annotation data should be labeled ('am', 'in', 'Ort' ...).
        public bool pdfSigShowDate;                	// display date and time in signature annotation
		public bool pdfSigAnnotTransparentBg;      	// display a transparent background?
		public int pdfSignatureWidth;              	// width for pdf signature annotation
		public int pdfSignatureHeight;             	// height for pdf signature annotation
		public IntPtr pdfSignatureSignedImage; 		// The default signature annotation has a "signed" image in the left bottom corner and an icon in the right bottom corner
		public IntPtr pdfSignatureSignerIcon;  		// The default signature annotation has a signers icon in the right bottom corner
		public IntPtr pdfSigBackgroundImage;   		// Display reader operated verification item as jpg (max. 220x70 pixel)
		public string pdfFormFieldName;             // The position is given by a form field. The rect entry of form fields dictionary will be used.
		public string pdfOutlineName;               // The position is given by an outline. This is not really PDF compliant but a customer's wish.
    };

    class SecSignerCaller
    {
        // success
        private const int OK = 0;  

        // sign data type: plain text
        private const int SIGNDATATYPE_PLAINTEXT = 0;
    
        // sign data type: HTML
        private const int SIGNDATATYPE_HTML = 1;

        // sign data type binary
        private const int SIGNDATATYPE_BINARY = 2;

        // sign data type: image
        private const int SIGNDATATYPE_IMAGE = 3;

        // sign data type: PDF
        private const int SIGNDATATYPE_PDF = 4;

        // sign data type: XML
        private const int SIGNDATATYPE_XML = 12;
    
        // sign data type: Rich text format 
        private const int SIGNDATATYPE_RTF = 13;

        // sign data type: ZIP file
        private const int SIGNDATATYPE_ZIP = 14;

        // sign data type: BMU ZKS document
        private const int SIGNDATATYPE_ZKS = 15;
        
        // signature format type: PKCS#7 CMS
        private const int SIGNATUREFORMATTYPE_PKCS7 = 0;
    
        // signature format type: PDF
        private const int SIGNATUREFORMATTYPE_PDF = 3;

        // signature format type XML-DSig
        private const int SIGNATUREFORMATTYPE_XMLDSIG = 4;

        // signature format type PKCS#7 SignedData with embedded document
        private const int SIGNATUREFORMATTYPE_PKCS7_EMBEDDED = 5;

        // PDF annotation position
        private const int PDF_ANNOTATION_NOT_SPECIFIED = 0;
        private const int PDF_ANNOTATION_LEFT_TOP_FIRSTPAGE = 1;
        private const int PDF_ANNOTATION_CENTER_TOP_FIRSTPAGE = 2;
        private const int PDF_ANNOTATION_RIGHT_TOP_FIRSTPAGE = 3;
        private const int PDF_ANNOTATION_LEFT_BOTTOM_FIRSTPAGE = 4;
        private const int PDF_ANNOTATION_CENTER_BOTTOM_FIRSTPAGE = 5;
        private const int PDF_ANNOTATION_RIGHT_BOTTOM_FIRSTPAGE = 6;
        private const int PDF_ANNOTATION_LEFT_TOP_LASTPAGE = 7;
        private const int PDF_ANNOTATION_CENTER_TOP_LASTPAGE = 8;
        private const int PDF_ANNOTATION_RIGHT_TOP_LASTPAGE = 9;
        private const int PDF_ANNOTATION_LEFT_BOTTOM_LASTPAGE = 10;
        private const int PDF_ANNOTATION_CENTER_BOTTOM_LASTPAGE = 11;
        private const int PDF_ANNOTATION_RIGHT_BOTTOM_LASTPAGE = 12;

        // buffer length for returned signature
        private const int SIG_PKCS7_BUF_LEN = 8000; // enough for detached CMS (PKCS#7) signatures
        private const int CERT_BUF_LEN = 5000; // should always be enough
        private const int CONTENT_BUF_LEN = 1500000;
        private const int VERSION_BUF_LEN = 100;

        // path to the SecSigner 32 bit DLL
        private const string SecSigner32DLLPath = "CallSecSignerDLL.dll";

        // path to the SecSigner 64 bit DLL
        private const string SecSigner64DLLPath = "CallSecSignerDLL64.dll";


        /**
         * Loads the JavaVM, checks whether all SecSigner JARs are found in the class path
         * and loads some SecSigner classes.
         *
         * @param secSignerInstallPath installation path of SecSigner
         * @param maximum size, in mega bytes, of the memory allocation pool
         * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND or METHOD_FAILED
         */
        public static int loadJavaVirtualMachine(string secSignerInstallPath, int maxMem)
        {
            return Environment.Is64BitProcess ? loadJavaVirtualMachine64(secSignerInstallPath, maxMem) :
                                                loadJavaVirtualMachine32(secSignerInstallPath, maxMem);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_LoadJavaVM", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int loadJavaVirtualMachine32(string secSignerInstallPath, int maxMem);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_LoadJavaVM", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int loadJavaVirtualMachine64(string secSignerInstallPath, int maxMem);

        /**
         * Initializes SecSigner.
         *
         * This method can be called again after SecSigner_Close().
         *
         * @param secSignerPropFileName property file name of SecSigner
         * @param secSignerInstallPath installation path of SecSigner
         * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND, METHOD_FAILED, DOC_MISMATCH or CANCELED
         */
        public static int initSecSigner(string secSignerPropFileName, string secSignerInstallPath)
        {
            return Environment.Is64BitProcess ? initSecSigner64(secSignerPropFileName, secSignerInstallPath) :
                                                initSecSigner32(secSignerPropFileName, secSignerInstallPath);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_Init", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSecSigner32(string secSignerPropFileName, string secSignerInstallPath);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_Init", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSecSigner64(string secSignerPropFileName, string secSignerInstallPath);

        /**
         * Opens the SecSigner dialog for initializing smart card reader and smart card.
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
         */
        public static int initSmartCard()
        {
            return Environment.Is64BitProcess ? initSmartCard64() : initSmartCard32();
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_InitSmartCard", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSmartCard32();

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_InitSmartCard", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSmartCard64();


        /**
         * Opens the SecSigner dialog for initializing smart card reader and smart card.
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         * @param sigCert buffer for the signature certificate of the inserted smart card
         * @param authCert buffer for the authentication certificate of the inserted smart card
         * @param encryptCert buffer for the encryption certificate of the inserted smart card
         *
         * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
         */
        public static int initSmartCard([In, Out] ref ByteArray sigCert, [In, Out] ref ByteArray authCert, [In, Out] ref ByteArray encryptCert)
        {
            return Environment.Is64BitProcess ? initSmartCard64(ref sigCert, ref authCert, ref encryptCert) :
                                                initSmartCard32(ref sigCert, ref authCert, ref encryptCert);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_InitSmartCardRetCerts", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSmartCard32([In, Out] ref ByteArray sigCert, [In, Out] ref ByteArray authCert, [In, Out] ref ByteArray encryptCert);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_InitSmartCardRetCerts", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int initSmartCard64([In, Out] ref ByteArray sigCert, [In, Out] ref ByteArray authCert, [In, Out] ref ByteArray encryptCert);


        /**
         * Opens the SecSigner dialog for checking and signing documents.
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * The signatures are returned in the document structs. The caller has to supply
         * buffers for that.
         *
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * @param document documents to be signed
         * @param document length of the document array
         * @param cipherCerts encrypt the signed PKCS#7 object with these certificates 
         * @param cipherCertCount number of encryption certificates or just offer encryption dialog if cipherCertCount = 0
         * @param signingKeyAndOrCertData a software key (PKCS#8 or PKCS#12) and/or corresponding or desired certificate
         * @param signingKeyAndOrCertDataCount must be 1 (key) or 2 (key and/or certificate)
         * @return OK or BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED or CANCELED
         */
        public static int signDocs([In, Out] Document[] documents, int documentCount,
                                          ByteArray[] cipherCerts, int cipherCertCount,
                                          ByteArray[] signingKeyAndOrCertData, int signingKeyAndOrCertDataCount)
        {
            return Environment.Is64BitProcess ? signDocs64(documents, documentCount, cipherCerts, cipherCertCount, signingKeyAndOrCertData, signingKeyAndOrCertDataCount) :
                                                signDocs32(documents, documentCount, cipherCerts, cipherCertCount, signingKeyAndOrCertData, signingKeyAndOrCertDataCount);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_Sign", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int signDocs32([In, Out] Document[] documents, int documentCount,
                                          ByteArray[] cipherCerts, int cipherCertCount,
                                          ByteArray[] signingKeyAndOrCertData, int signingKeyAndOrCertDataCount);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_Sign", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int signDocs64([In, Out] Document[] documents, int documentCount,
                                          ByteArray[] cipherCerts, int cipherCertCount,
                                          ByteArray[] signingKeyAndOrCertData, int signingKeyAndOrCertDataCount);

        /**
         * Opens the SecSigner dialog which verifies the given signatures.
         *
         * Works only for documentCount = 1 for the time being.
         *
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * The OCSP responses are returned in the document structs if the user starts an
         * OCSP request. The caller has to supply a buffer for that or set the buffer to NULL
         * if the OCSP response shall not be returned.
         *
         * @param document documents and signatures to be verified
         * @param document length of the document array
         * @return OK or SIGNATURE_INVALID, BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED, DOC_MISMATCH or CANCELED
         */
        public static int verifyDocs([In, Out] Document[] documents, int documentCount)
        {
            return Environment.Is64BitProcess ? verifyDocs64(documents, documentCount) :
                                                verifyDocs32(documents, documentCount);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_Verify", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int verifyDocs32([In, Out] Document[] documents, int documentCount);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_Verify", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int verifyDocs64([In, Out] Document[] documents, int documentCount);


        /**
         * Opens the SecSigner dialog which verifies the given signatures.
         *
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * The OCSP responses are returned in the document structs if the user starts an
         * OCSP request. The caller has to supply a buffer for that or set the buffer to NULL
         * if the OCSP response shall not be returned.
         *
         * @param document documents and signatures to be verified
         * @param document length of the document array
         * @param offerFileOpenDlg if true offer a file-open-dialog if data are missing, else return an error then
         * @param oscpMandatory automatically send oscp request if true
         * @param allowSig if false then the user cannot sign the verified document 
         * @param modal if true, dialog blocks input to other app windows when shown
         * @return OK or SIGNATURE_INVALID, DOC_HAS_NO_SIGNATURE, BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED, DOC_MISMATCH or CANCELED
         */
        public static int verifyDocs([In, Out] Document[] documents, int documentCount, bool offerFileOpenDlg, bool oscpMandatory,
                                                bool allowSig, bool modal)
        {
            return Environment.Is64BitProcess ? verifyDocs64(documents, documentCount, offerFileOpenDlg, oscpMandatory, allowSig, modal) :
                                                verifyDocs32(documents, documentCount, offerFileOpenDlg, oscpMandatory, allowSig, modal);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_VerifyExt", CharSet = CharSet.Ansi,
                 SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int verifyDocs32([In, Out] Document[] documents, int documentCount, bool offerFileOpenDlg, bool oscpMandatory,
                                                bool allowSig, bool modal);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_VerifyExt", CharSet = CharSet.Ansi,
                 SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int verifyDocs64([In, Out] Document[] documents, int documentCount, bool offerFileOpenDlg, bool oscpMandatory,
                                                bool allowSig, bool modal);

        /**
         * Encrypts data.
         *
         * The encrypted documents are returned in the document structs. The caller has to 
         * supply buffers for that.
         *
         * If not OK is returned then getErrorMessage() maybe called to get the error message.
         *
         * @param document documents to be signed
         * @param document length of the document array
         * @param cipherCert certificates for which the documents will be encrypted
         * @param cipherCertCount number of certificates
         * @return OK or BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED or CANCELED
         */
        public static int encryptOnly([In, Out] Document[] documents, int documentCount, [Out] ByteArray[] cipherCert, int cipherCertCount)
        {
            return Environment.Is64BitProcess ? encryptOnly64(documents, documentCount, cipherCert, cipherCertCount) :
                                                encryptOnly32(documents, documentCount, cipherCert, cipherCertCount);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_EncryptOnly", CharSet = CharSet.Ansi,
                 SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int encryptOnly32([In, Out] Document[] documents, int documentCount, [Out] ByteArray[] cipherCert, int cipherCertCount);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_EncryptOnly", CharSet = CharSet.Ansi,
                 SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int encryptOnly64([In, Out] Document[] documents, int documentCount, [Out] ByteArray[] cipherCert, int cipherCertCount);


        /**
         * Gets the version of SecSigner.
         *
         * @param buffer buffer for the 0x00 terminated version string
         * @param bufferLen length of the buffer
         * @return OK or BUFFER_TOO_SHORT
         */
        public static int getVersion([MarshalAs(UnmanagedType.LPArray, SizeConst = 100)] byte[] buffer, int bufferLen)
        {
            return Environment.Is64BitProcess ? getVersion64(buffer, bufferLen) :
                                                getVersion32(buffer, bufferLen);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_GetVersion", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getVersion32([MarshalAs(UnmanagedType.LPArray, SizeConst = 100)] byte[] buffer, int bufferLen);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_GetVersion", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getVersion64([MarshalAs(UnmanagedType.LPArray, SizeConst = 100)] byte[] buffer, int bufferLen);


        /**
         * Gets the maximal number of documents to be signed at once.
         *
         * @return maximal number of documents to be signed at once
         */
        public static int getSignatureLimit()
        {
            return Environment.Is64BitProcess ? getSignatureLimit64() : getSignatureLimit32();
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_GetSignatureLimit", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getSignatureLimit32();

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_GetSignatureLimit", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getSignatureLimit64();


        /**
         * Sets a licence for SecSigner.
         *
         * @param licence buffer for licence data
         * @param licenceLen length of the buffer
         * @return OK or METHOD_FAILED
         */
        public static int setLicence([MarshalAs(UnmanagedType.LPArray)] byte[] licence, int licenceLen)
        {
            return Environment.Is64BitProcess ? setLicence64(licence, licenceLen) :
                                                setLicence32(licence, licenceLen);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_SetLicence", CharSet = CharSet.Ansi,
                      SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int setLicence32([MarshalAs(UnmanagedType.LPArray)] byte[] licence, int licenceLen);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_SetLicence", CharSet = CharSet.Ansi,
                      SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int setLicence64([MarshalAs(UnmanagedType.LPArray)] byte[] licence, int licenceLen);

        /**
         * Gets the error message of the last failed operation. This message which is returned by 
         * this function is updated whenever a function (like SecSigner_Sign()) returns an error status.
         *
         * @param buffer buffer for the 0x00 terminated error messag
         * @param bufferLen length of the buffer
         * @return OK or BUFFER_TOO_SHORT
         */
        public static int getErrorMessage([MarshalAs(UnmanagedType.LPArray, SizeConst = 5000)] byte[] buffer, int bufferLen)
        {
            return Environment.Is64BitProcess ? getErrorMessage64(buffer, bufferLen) :
                                                getErrorMessage32(buffer, bufferLen);
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_GetErrorMessage", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getErrorMessage32([MarshalAs(UnmanagedType.LPArray, SizeConst = 5000)] byte[] buffer, int bufferLen);

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_GetErrorMessage", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int getErrorMessage64([MarshalAs(UnmanagedType.LPArray, SizeConst = 5000)] byte[] buffer, int bufferLen);


        /**
         * Closes SecSigner.
         *
         * A new instance of SecSigner can be created with SecSigner_Init() as long
         * as SecSigner_Unload_JavaVM() has not been called.
         *
         * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
         */
        public static int closeSecSigner()
        {
            return Environment.Is64BitProcess ? closeSecSigner64() : closeSecSigner32();
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_Close", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int closeSecSigner32();

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_Close", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int closeSecSigner64();

        
        /**
         * Unloads the JavaVM. Another call to SecSigner_LoadJavaVM() in the same
         * application will fail.
         *
         * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
         */
        public static int unloadJavaVirtualMachine()
        {
            return Environment.Is64BitProcess ? unloadJavaVirtualMachine64() : unloadJavaVirtualMachine32();
        }

        [DllImport(SecSigner32DLLPath, EntryPoint = "SecSigner_UnloadJavaVM", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int unloadJavaVirtualMachine32();

        [DllImport(SecSigner64DLLPath, EntryPoint = "SecSigner_UnloadJavaVM", CharSet = CharSet.Ansi,
             SetLastError = true, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl)]
        public static extern int unloadJavaVirtualMachine64();

        
        /**
         * Prints SecSigner's version on the console.
         */
        private static void showSecSignerVersion()
        {
            // The version is given as a 0x00 terminated UTF-8 string by SecSigner.
            byte[] versionByteBuffer = new byte[100];
            versionByteBuffer[0] = 0;
            string version = "Cannot get the version from SecSigner.";
            if (0 == (getVersion(versionByteBuffer, 100)))
            {
                int endIndex = 0;
                while ((versionByteBuffer[endIndex] != 0) && (endIndex < 5000))
                {
                    endIndex++;
                }
                Encoding encoding = Encoding.UTF8;
                version = encoding.GetString(versionByteBuffer, 0, endIndex);
            }
            Console.WriteLine(version);
        }
        
        /**
         * Prints the error message of the last failed SecSigner operation on the console.
         */
        private static void showLastErrorMessage()
        {
            // The error message is given as a 0x00 terminated UTF-8 string by SecSigner.
            byte[] errorMsgByteBuffer = new byte[5000];
            errorMsgByteBuffer[0] = 0;
            string errorMsg = "Cannot get the error message from SecSigner.";
            if (0 == (getErrorMessage(errorMsgByteBuffer, 5000))) 
            {
                int endIndex=0;
                while ((errorMsgByteBuffer[endIndex] !=0) && (endIndex < 5000))
                {
                    endIndex++;
                }
                Encoding encoding = Encoding.UTF8;
                errorMsg = encoding.GetString(errorMsgByteBuffer, 0, endIndex);
            }
            Console.WriteLine(errorMsg);
        }

        /**
         * Test function start point.
         */
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length < 4)
            {
                Console.WriteLine ("usage TestCallSecSignerDLL <test mode> <SecSignerPropertyName> <SecSignerInstallPath> <maxMemMB> ...");
                return;
            }

            // read command line parameters
            string option = args[0];
            string secSignerPropertyName = args[1];
            string secSignerInstallPath = args[2];
            string maxMemStr = args[3];

            // path of documents to be signed
            string documentsPath = null;

            bool signGivenDocs = false;
            bool verifyGivenDocs = false;
            bool restart = false;
            bool setSecSignerLicence = false;

            if ((option[0] == '2') || (option[0] == '4'))
            {
                // read command line parameter
                if (args.Length < 5)
                {
                    Console.WriteLine ("parameter <documentsPath> missing");
                    return;
                }

                documentsPath = args[4];
                Console.WriteLine ("taking test documents from " + documentsPath);

                    signGivenDocs=true;
        
                if (option[0] == '4')
                {
                    restart=true;
                }
            }
            else if (option[0] == '3')
            {
                // read command line parameter
                if (args.Length < 5)
                {
                    Console.WriteLine ("parameter <documentsPath> missing");
                    return;
                }

                documentsPath = args[4];
                Console.WriteLine ("taking test documents from " + documentsPath);

                verifyGivenDocs=true;
            }
            else if (option[0] == '6')
            {
                documentsPath = args[4];
                Console.WriteLine("taking test documents from " + documentsPath);
                setSecSignerLicence = true;
            }
            else
            {
                Console.WriteLine ("The first parameter has to be");
                Console.WriteLine ("  '2' = test signature of given documents");
                Console.WriteLine ("  '3' = test verification of given documents");
                Console.WriteLine ("  '4' = test signature of given documents and restart");
                Console.WriteLine ("  '6' = test set licence");
                return;
            }

            int maxMem = 0;
            try
            {
                maxMem = Convert.ToInt32(maxMemStr);
            }
            catch (Exception e)
            {
                Console.WriteLine(maxMemStr + " is no integer. MaxMem defaults to 64 MB. " + e.ToString());
                maxMem= 64;
            }

            // Load Java virtual machine
            Console.WriteLine("Loading JavaVM");
            int ret = loadJavaVirtualMachine(secSignerInstallPath, maxMem);
            if (ret <0)
            {
                showLastErrorMessage();
                return;
            }

            // init SecSecSigner
            Console.WriteLine("Initializing SecSigner");
            ret = initSecSigner(secSignerPropertyName, secSignerInstallPath);
            if (ret <0)
            {
                showLastErrorMessage();
                return;
            }

            showSecSignerVersion();
            
            // find smart card
            if (!verifyGivenDocs && !setSecSignerLicence)
            {
                Console.WriteLine("Looking for smart card.");
                
                // This is optional. If the signature function is called without initSmartCard
                // before, then it will start with the same smart-card-init-dialog.

                // buffer for returned signature certificate
                ByteArray sigCert = new ByteArray();
                sigCert.data = Marshal.AllocHGlobal(CERT_BUF_LEN);
                sigCert.dataLen = CERT_BUF_LEN;

                // buffer for returned authentication certificate
                ByteArray authCert = new ByteArray();
                authCert.data = Marshal.AllocHGlobal(CERT_BUF_LEN);
                authCert.dataLen = CERT_BUF_LEN;

                // buffer for returned encryption certificate
                ByteArray encryptCert = new ByteArray();
                encryptCert.data = Marshal.AllocHGlobal(CERT_BUF_LEN);
                encryptCert.dataLen = CERT_BUF_LEN;

                try
                {
                    ret = initSmartCard(ref sigCert, ref authCert, ref encryptCert);
                
                    if (ret <0)
                    {
                        showLastErrorMessage();
                        return;
                    }

                    // write the returned signature certificate to a file
                    if (sigCert.dataLen > 0)
                    {
                        String sigCertFileName = "sigCert.der";
                        Console.WriteLine("Writing signature certificate to " + sigCertFileName);

                        byte[] certBuffer = new byte[sigCert.dataLen];
                        Marshal.Copy(sigCert.data, certBuffer, 0, sigCert.dataLen);

                        FileStream certFileStream = new FileStream(sigCertFileName, FileMode.Create, FileAccess.Write);
                        BinaryWriter certFileWriter = new BinaryWriter(certFileStream);
                        certFileWriter.Write(certBuffer);
                        certFileWriter.Close();
                    }

                    // write the returned authentication certificate to a file
                    if (authCert.dataLen > 0)
                    {
                        String authCertFileName = "authCert.der";
                        Console.WriteLine("Writing authentication certificate to " + authCertFileName);

                        byte[] certBuffer = new byte[authCert.dataLen];
                        Marshal.Copy(authCert.data, certBuffer, 0, authCert.dataLen);

                        FileStream certFileStream = new FileStream(authCertFileName, FileMode.Create, FileAccess.Write);
                        BinaryWriter certFileWriter = new BinaryWriter(certFileStream);
                        certFileWriter.Write(certBuffer);
                        certFileWriter.Close();
                    }

                    // write the returned encryption certificate to a file
                    if (encryptCert.dataLen > 0)
                    {
                        String encryptCertFileName = "encryptCert.der";
                        Console.WriteLine("Writing encryption certificate to " + encryptCertFileName);

                        byte[] certBuffer = new byte[encryptCert.dataLen];
                        Marshal.Copy(encryptCert.data, certBuffer, 0, encryptCert.dataLen);

                        FileStream certFileStream = new FileStream(encryptCertFileName, FileMode.Create, FileAccess.Write);
                        BinaryWriter certFileWriter = new BinaryWriter(certFileStream);
                        certFileWriter.Write(certBuffer);
                        certFileWriter.Close();
                    }
                }
                finally
                {
                    // free the buffers
                    Marshal.FreeHGlobal(sigCert.data);
                    Marshal.FreeHGlobal(authCert.data);
                    Marshal.FreeHGlobal(encryptCert.data);
                }
            }

            if (setSecSignerLicence)
            {
                // read test licence file
                string fileNameWithPath = documentsPath + "\\test-licence.pkcs7";
                Console.WriteLine("Reading " + fileNameWithPath);

                // read test file
                FileStream fileStream = new FileStream(fileNameWithPath, FileMode.Open, FileAccess.Read);
                BinaryReader fileReader = new BinaryReader(fileStream);
                // Read the licence file. Must not be longer than CONTENT_BUF_LEN in this example!
                byte[] licenceBuffer = fileReader.ReadBytes(CONTENT_BUF_LEN);
                fileReader.Close();

                IntPtr contentCBuffer = Marshal.AllocHGlobal(licenceBuffer.Length);
                Marshal.Copy(licenceBuffer, 0, contentCBuffer, licenceBuffer.Length);

                // call Signer.setLicence
                Console.WriteLine("Calling SecSigner.setLicence().");

                ret = setLicence(licenceBuffer, licenceBuffer.Length);
                Console.WriteLine("SecSigner.setLicence() returned " + ret);
            }

            if (signGivenDocs)
            {
                // read test documents and pass them to SecSigner
                const int DOCCOUNT = 1; // standard SecSigner
                // const int DOCCOUNT = 16; // SecSigner with mass-signature option
                string[] fileNames = new string[16];
                Document[] documents = new Document[DOCCOUNT];

                fileNames[ 0] = "doc1000.txt";
                fileNames[ 1] = "doc1001.txt";
                fileNames[ 2] = "doc1002.txt";
                fileNames[ 3] = "doc1003.txt";
                fileNames[ 4] = "doc1004.txt";
                fileNames[ 5] = "doc1005.txt";
                fileNames[ 6] = "doc1006.txt";
                fileNames[ 7] = "doc1007.txt";
                fileNames[ 8] = "doc1008.txt";
                fileNames[ 9] = "doc1009.txt";
                fileNames[10] = "doc1010.txt";
                fileNames[11] = "doc1011.txt";
                fileNames[12] = "doc1012.txt";
                fileNames[13] = "doc1013.txt";
                fileNames[14] = "doc1014.txt";
                fileNames[15] = "doc1015.txt";

                // sign twice, just to test the signature call, of course once is enough in production
                for (int s=0; s<2; s++)
                {
                    for (int i=0; i<DOCCOUNT; i++)
                    {
                        // set document file name
                        string fileNameWithPath = documentsPath + "\\" + fileNames[i];
                        Console.WriteLine ("Reading " + fileNameWithPath);

                        // buffer for returned signature
                        int signatureLen = SIG_PKCS7_BUF_LEN;
                        IntPtr signatureCBuffer = Marshal.AllocHGlobal(SIG_PKCS7_BUF_LEN);

                        // buffer for returned encrypted signature (if encryption certificates are supplied)
                        IntPtr encrSigCBuffer = Marshal.AllocHGlobal(SIG_PKCS7_BUF_LEN);
    
                        // Recent SecSigner versions do not return a time stamp as a separate file but insert into the created signature. 
                        IntPtr timestampCBuffer = IntPtr.Zero;
    
                        // read test file
                        FileStream fileStream = new FileStream(fileNameWithPath, FileMode.Open, FileAccess.Read);
                        BinaryReader fileReader = new BinaryReader(fileStream);
                        // Read data to be signed. Must not be longer than CONTENT_BUF_LEN in this example!
                        byte[] contentBuffer = fileReader.ReadBytes(CONTENT_BUF_LEN);
                        fileReader.Close();

                        IntPtr contentCBuffer = Marshal.AllocHGlobal(contentBuffer.Length);
                        Marshal.Copy(contentBuffer, 0, contentCBuffer, contentBuffer.Length);

                        // fill document struct
                        documents[i].version = 13;
                        documents[i].documentFileName = fileNames[i];
                        documents[i].dataToBeSigned = contentCBuffer;
                        documents[i].dataToBeSignedLen = contentBuffer.Length;
                        documents[i].dataToBeSignedBufLen = 0; // not used in signature mode
                        documents[i].documentType = SIGNDATATYPE_PLAINTEXT;
                        documents[i].signatureFormatType = SIGNATUREFORMATTYPE_PKCS7;
                        if(documents[i].signatureFormatType == SIGNATUREFORMATTYPE_XMLDSIG)
                        {
                            // ensure signature buffer is enough to hold the whole xml/pdf document
                            signatureLen = contentBuffer.Length + 2 * SIG_PKCS7_BUF_LEN;
                            signatureCBuffer = Marshal.AllocHGlobal(signatureLen);
                        }
                        documents[i].mimeType = null;
                        documents[i].oldSignature = IntPtr.Zero; // a previous signature for the same document could be set here
                        documents[i].oldSignatureLen = 0;
                        documents[i].signature = signatureCBuffer;
                        documents[i].signatureLen = signatureLen;
                        documents[i].encryptedSig = encrSigCBuffer;
                        documents[i].encryptedSigLen = signatureLen;
                        documents[i].ocspResponse = IntPtr.Zero;
                        documents[i].ocspResponseLen = 0;
                        documents[i].ocspResponseBufLen = 0;
                        documents[i].timeStamp = timestampCBuffer;
                        documents[i].timeStampLen = SIG_PKCS7_BUF_LEN;
                        documents[i].encryptedDoc = IntPtr.Zero;
                        documents[i].encryptedDocLen = 0;
                        documents[i].hashAlgorithm = null; // null = "autoselect", "SHA1", "SHA256", "SHA512", ...
                        documents[i].verificationReport = IntPtr.Zero; // not used in signature mode
                        documents[i].verificationReportLen = 0;

                        documents[i].signatureID = null;
                        documents[i].xmlDSigNodePath = null;
                        documents[i].setUseLegacyBmuXmlSigFormat = false;
                        documents[i].numberOfXmlDSigFilterPaths = 0;
                        documents[i].xmlDSigFilterPaths = IntPtr.Zero;

                        // example for xml dsig parameters
                        bool createXmlDSig = false;
                        if (createXmlDSig)
                        {
                            documents[i].signatureID = "Sig-1234567"; // the id is inserted as value of id-attribute of the signature node
                            documents[i].xmlDSigNodePath = "//*[@lib:LayerID='ERZ-01-b37d1bb7-42e9-4e6c-89b9-e824343566b8']"; // put the signature under the node with the spcific layer id

                            XPathTransformFilter[] transformFilter = new XPathTransformFilter[2];
                            transformFilter[0].transformMethod = 1; // subtract
                            transformFilter[0].xpathExpr = "here()/ancestor-or-self::*/dsig:Signature"; // which nodes have to be subtracted
                            transformFilter[0].numberOfXmlDSigNamespaces = 0;
                            transformFilter[0].xmlDSigNameSpaceMappings = IntPtr.Zero;

                            NameSpaceMapping[] namespaceMappings = new NameSpaceMapping[1];
                            namespaceMappings[0].namespacePrefix = "dsig";
                            namespaceMappings[0].namespaceURI = "http://www.w3.org/2000/09/xmldsig#";

                            // transform namespace mappings array into a single IntPtr
                            IntPtr namespaceMappingsPtr = IntPtr.Zero;
                            if (namespaceMappings != null && namespaceMappings.Length > 0)
                            {
                                namespaceMappingsPtr = Marshal.AllocHGlobal(namespaceMappings.Length * Marshal.SizeOf(typeof(NameSpaceMapping)));
                                IntPtr currentNameSpace = new IntPtr(namespaceMappingsPtr.ToInt32());
                                for (int j = 0; j < namespaceMappings.Length; j++)
                                {
                                    Marshal.StructureToPtr(namespaceMappings[j], currentNameSpace, false);
                                    currentNameSpace = new IntPtr(currentNameSpace.ToInt32() + Marshal.SizeOf(typeof(NameSpaceMapping)));
                                }
                            }

                            // 0=intersect, 1=subtract, 2=union
                            transformFilter[1].transformMethod = 0; // intersect
                            transformFilter[1].xpathExpr = "/descendant::*[local-name()='BGSERZLayer']"; // which nodes have to be intersect with the document
                            transformFilter[1].numberOfXmlDSigNamespaces = 1;
                            transformFilter[1].xmlDSigNameSpaceMappings = namespaceMappingsPtr;

                            // transform filter array into a single IntPtr
                            IntPtr transformFilterPtr = IntPtr.Zero;
                            if (transformFilter != null && transformFilter.Length > 0)
                            {
                                transformFilterPtr = Marshal.AllocHGlobal(transformFilter.Length * Marshal.SizeOf(typeof(XPathTransformFilter)));
                                IntPtr currentTransform = new IntPtr(transformFilterPtr.ToInt32());
                                for (int j = 0; j < transformFilter.Length; j++)
                                {
                                    Marshal.StructureToPtr(transformFilter[j], currentTransform, false);
                                    currentTransform = new IntPtr(currentTransform.ToInt32() + Marshal.SizeOf(typeof(XPathTransformFilter)));
                                }
                            }

                            documents[i].numberOfXmlDSigFilterPaths = 2; // number of transform filter in the array. this is used to de-reference arrays values
                            documents[i].xmlDSigFilterPaths = transformFilterPtr;
                        }
                        
                        documents[i].evidenceRecordArray = IntPtr.Zero; // not used in signature mode
                        documents[i].numberOfEvidenceRecords = 0; // not used in signature mode
                        
                        bool createPdfEmbeddedSig = false;
                        if(createPdfEmbeddedSig)
                        {
                        	// set signature type to PDF
                        	documents[i].signatureFormatType = SIGNATUREFORMATTYPE_PDF;
                        	documents[i].documentType = SIGNDATATYPE_PDF;
                        	
							PdfAnnotation pdfAnnotation = new PdfAnnotation(); // optional visual representation of the signature in the PDF document
							pdfAnnotation.pdfDisplayAnnotation = true; // determine whether a verification item (JPG image) should be added to the PDF document.
							pdfAnnotation.pdfSignatureImageText = true; // determines whether a text should be added onto the verification item image data.
							pdfAnnotation.pdfSignatureReason = null; // signature reason text to be added onto the verification item image.
							pdfAnnotation.pdfSignatureLocation = null; // signature location text to be added onto the verification item image.
							pdfAnnotation.pdfSignaturePosition = PDF_ANNOTATION_RIGHT_BOTTOM_LASTPAGE; // position for a virtual box containing verification item image and link item image.
							pdfAnnotation.pdfSigTextSizeAndPosition = null; // size and position for text (signer, time, etc.) in signature annotation.
							pdfAnnotation.pdfSigAnnotLabels = true; // determines whether signature annotation data should be labeled ('am', 'in', 'Ort' ...).
							pdfAnnotation.pdfSigShowDate = true; // display date and time in signature annotation
							pdfAnnotation.pdfSigAnnotTransparentBg = false; // display a transparent background?
							pdfAnnotation.pdfSignatureWidth = 220; // width for pdf signature annotation
							pdfAnnotation.pdfSignatureHeight = 70; // height for pdf signature annotation
							pdfAnnotation.pdfSignatureSignedImage = IntPtr.Zero; // The default signature annotation has a "signed" image in the left bottom corner and an icon in the right bottom corner
							pdfAnnotation.pdfSignatureSignerIcon = IntPtr.Zero; // The default signature annotation has a signers icon in the right bottom corner
							pdfAnnotation.pdfSigBackgroundImage = IntPtr.Zero;  // Display reader operated verification item as jpg (max. 220x70 pixel)
							pdfAnnotation.pdfFormFieldName = null; // The position is given by a form field. The rect entry of form fields dictionary will be used.
							pdfAnnotation.pdfOutlineName = null; // The position is given by an outline. This is not really PDF compliant but a customer's wish.
							
							// deprecated fields
							pdfAnnotation.pdfSignatureImage = IntPtr.Zero; // 
							pdfAnnotation.pdfSignatureImageText = true; // 
							pdfAnnotation.pdfLinkImage = IntPtr.Zero; // 
							pdfAnnotation.pdfLinkUrl = null; // 
                            pdfAnnotation.pdfSignatureTextAnnot = null; // 
                            pdfAnnotation.pdfScaleSigImage = true; // 

							// transform the PDF-Annotation-Data into an IntPtr
							IntPtr pdfAnnotationPtr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(PdfAnnotation)));
							IntPtr cur = new IntPtr(pdfAnnotationPtr.ToInt32());
							Marshal.StructureToPtr(pdfAnnotation, cur, false);
						
							documents[i].pdfAnnotation = pdfAnnotationPtr;
                        } else {
	                        documents[i].pdfAnnotation = IntPtr.Zero; // pdfAnnotationPtr; // pdfAnnotation;
	                    }
                    }

                    ByteArray[] cipherCerts = null; // no encryption certificates in this example
                    int cipherCertCount = -1;       // do not show encryption certificate dialog
                    ByteArray[] signingKeyAndOrCertData = null; // no software key in this example
                    int signingKeyAndOrCertDataCount = 0;
                    
                    // call Signer signature process
                    Console.WriteLine ("Calling SecSigner signature");
                    ret = signDocs(documents, DOCCOUNT, cipherCerts, cipherCertCount, signingKeyAndOrCertData, signingKeyAndOrCertDataCount);
                    Console.WriteLine("signature return value " + ret);

                    if (ret == OK)
                    {
                        // write the returned signatures to files
                        for (int j=0; j<DOCCOUNT; j++)
                        {
                            byte[] signatureBuffer = new byte[documents[j].signatureLen];
                            Marshal.Copy(documents[j].signature, signatureBuffer, 0, documents[j].signatureLen);
                            
                            FileStream sigFileStream = new FileStream(documentsPath + "\\" + fileNames[j] + ".pkcs7", FileMode.Create, FileAccess.Write);
                            BinaryWriter sigFileWriter = new BinaryWriter(sigFileStream);
                            sigFileWriter.Write(signatureBuffer);
                            sigFileWriter.Close();

                            // write the returned encrypted signature (if encryption certificates were supplied,
                            // which is not possible with the DLL currently)
                            if (documents[j].encryptedSigLen > 0)
                            {
                                byte[] encrSigBuffer = new byte[documents[j].encryptedSigLen];
                                Marshal.Copy(documents[j].encryptedSig, encrSigBuffer, 0, documents[j].encryptedSigLen);
                            
                                FileStream encrSigFileStream = new FileStream(documentsPath + "\\" + fileNames[j] + "_encrypted.pkcs7", FileMode.Create, FileAccess.Write);
                                BinaryWriter encrSigFileWriter = new BinaryWriter(encrSigFileStream);
                                encrSigFileWriter.Write(encrSigBuffer);
                                encrSigFileWriter.Close();
                            }
                        }
                    }
                    else
                    {
                        showLastErrorMessage();
                    }

                    for (int d=0; d<DOCCOUNT; d++)
                    {
                        // release the buffers
                        if (documents[d].dataToBeSigned != IntPtr.Zero)
                        {
                            // free the buffer with the document's content
                            Marshal.FreeHGlobal(documents[d].dataToBeSigned);
                            documents[d].dataToBeSigned = IntPtr.Zero;
                        }

                        if (documents[d].signature != IntPtr.Zero)
                        {
                            // free the buffer with the document's signature
                            Marshal.FreeHGlobal(documents[d].signature);
                            documents[d].signature = IntPtr.Zero;
                        }

                        if (documents[d].oldSignature != IntPtr.Zero)
                        {
                            // free the buffer with the document's previous signature
                            Marshal.FreeHGlobal(documents[d].oldSignature);
                            documents[d].oldSignature = IntPtr.Zero;
                        }

                        if (documents[d].encryptedSig != IntPtr.Zero)
                        {
                            // free the buffer with the encrypted document's signature
                            Marshal.FreeHGlobal(documents[d].encryptedSig);
                            documents[d].encryptedSig = IntPtr.Zero;
                        }

                        if (documents[d].timeStamp != IntPtr.Zero)
                        {
                            // free the buffer with the time stamp
                            Marshal.FreeHGlobal(documents[d].timeStamp);
                            documents[d].timeStamp = IntPtr.Zero;
                        }
                    }
                }
            }
            else if (verifyGivenDocs)
            {
                // read test documents and verify them in SecSigner
                string[] fileNames = new string[14];
    
                const int DOC_COUNT = 1; // for the time being verification only works with a single document
                Document[] documents = new Document[DOC_COUNT];
    
                fileNames[0]  = "doc1000.txt";
                fileNames[1]  = "doc1001.txt";
                fileNames[2]  = "doc1002.txt";
                fileNames[3]  = "doc1003.txt";
                fileNames[4]  = "doc1004.txt";
                fileNames[5]  = "doc1005.txt";
                fileNames[6]  = "doc1006.txt";
                fileNames[7]  = "doc1007.txt";
                fileNames[8]  = "doc1008.txt";
                fileNames[9]  = "doc1009.txt";
                fileNames[10] = "doc1010.txt";
                fileNames[11] = "doc1011.txt";
                fileNames[12] = "doc1012.txt";
                fileNames[13] = "doc1013.txt";
        
                for (int vi=0; vi<DOC_COUNT; vi++)
                {
                    // set document file name
                    string docFileNameWithPath = documentsPath + fileNames[vi];
                    string sigFileNameWithPath = documentsPath + fileNames[vi] + ".pkcs7";
                    string ocspRespFileNameWithPath = documentsPath + fileNames[vi] + ".ors";

                    // read document
                    Console.WriteLine ("Reading " + docFileNameWithPath);
                    try
                    {
                        FileStream fileStream = new FileStream(docFileNameWithPath, FileMode.Open, FileAccess.Read);
                        BinaryReader fileReader = new BinaryReader(fileStream);
                        byte[] contentBuffer = fileReader.ReadBytes(CONTENT_BUF_LEN);
                        fileReader.Close();

                        // buffer for signed content
                        IntPtr contentCBuffer = Marshal.AllocHGlobal(contentBuffer.Length);
                        Marshal.Copy(contentBuffer, 0, contentCBuffer, contentBuffer.Length);

                        documents[vi].dataToBeSigned = contentCBuffer;
                        documents[vi].dataToBeSignedLen = contentBuffer.Length;
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Cannot read document file " + docFileNameWithPath + e.ToString());
                        return;
                    }
    

                    // read signature
                    Console.WriteLine ("Reading " + sigFileNameWithPath);
                    try
                    {
                        FileStream fileStream = new FileStream(sigFileNameWithPath, FileMode.Open, FileAccess.Read);
                        BinaryReader fileReader = new BinaryReader(fileStream);
                        byte[] signatureBuffer = fileReader.ReadBytes(CONTENT_BUF_LEN);
                        fileReader.Close();

                        // buffer for signature
                        IntPtr signatureCBuffer = Marshal.AllocHGlobal(signatureBuffer.Length);
                        Marshal.Copy(signatureBuffer, 0, signatureCBuffer, signatureBuffer.Length);

                        documents[vi].signature = signatureCBuffer;
                        documents[vi].signatureLen = signatureBuffer.Length;
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Cannot read signature file " + sigFileNameWithPath + e.ToString());
                        return;
                    }


                    // read supplied OCSP response
                    Console.WriteLine ("Reading " + ocspRespFileNameWithPath);
                    int ocspResponseBufLen = SIG_PKCS7_BUF_LEN;
                    byte[] ocspResponseBuffer = null;
                    try
                    {
                        FileStream fileStream = new FileStream(ocspRespFileNameWithPath, FileMode.Open, FileAccess.Read);
                        BinaryReader fileReader = new BinaryReader(fileStream);
                        ocspResponseBuffer = fileReader.ReadBytes(CONTENT_BUF_LEN);
                        fileReader.Close();

                        // The buffer shall be big enough for the file just read and
                        // it least SIG_BUF_LEN bytes big.
                        if (ocspResponseBuffer.Length > ocspResponseBufLen)
                        {
                            ocspResponseBufLen = ocspResponseBuffer.Length;
                        }

                        // buffer is used for the supplied OCSP response.
                        // SecSigner may overwrite the buffer's content with a new OCSP response.
                        documents[vi].ocspResponseLen = ocspResponseBuffer.Length; 
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Cannot open optional OCSP response " + ocspRespFileNameWithPath + 
                                          ". Calling SecSigner without it: " + e);
                        documents[vi].ocspResponseLen = 0;
                    }

                    // buffer for supplied or returned OCSP response
                    IntPtr ocspResponseCBuffer = Marshal.AllocHGlobal(ocspResponseBufLen);
                    if (null != ocspResponseBuffer)
                    {
                        // copy the OCSP response which was read from the file
                        Marshal.Copy(ocspResponseBuffer, 0, ocspResponseCBuffer, ocspResponseBuffer.Length);
                    }    
                    documents[vi].ocspResponse = ocspResponseCBuffer;
                    documents[vi].ocspResponseBufLen = ocspResponseBufLen; 

                    documents[vi].timeStamp = IntPtr.Zero; // usually a timestamp is included in the signature file and not given here as a separate file
                    documents[vi].timeStampLen = 0;

                    // fill document struct
                    documents[vi].version = 13;
                    documents[vi].documentType = SIGNDATATYPE_PLAINTEXT;
                    documents[vi].signatureFormatType = SIGNATUREFORMATTYPE_PKCS7;
                    documents[vi].mimeType = null;
                    documents[vi].dataToBeSignedBufLen = 0;    // signed content shall not be returned

                    documents[vi].oldSignature = IntPtr.Zero;  // not used in verify
                    documents[vi].oldSignatureLen = 0;         // not used in verify

                    documents[vi].encryptedSig = IntPtr.Zero;  // not used in verify, supply signature in the 
                                                               // signature-field, whether it's encrypted or not
                    documents[vi].encryptedSigLen = 0;         // not used in verify

                    documents[vi].documentFileName = fileNames[vi];
                    
                    documents[vi].encryptedDoc = IntPtr.Zero;
                    documents[vi].encryptedDocLen = 0;

                    documents[vi].signatureID = null;           // not used in verify
                    documents[vi].xmlDSigNodePath = null;
                    documents[vi].setUseLegacyBmuXmlSigFormat = false;
                    documents[vi].xmlDSigFilterPaths = IntPtr.Zero;    // not used in verify
                }

                // call SecSigner verification process
                Console.WriteLine ("Calling SecSigner verification\n");
                ret = verifyDocs(documents, DOC_COUNT);
                Console.WriteLine("verification return value =" + ret);

                if (ret == OK)
                {
                    // write the returned OCSP responses to files
                    for (int j=0; j<DOC_COUNT; j++)
                    {
                        if (0 == documents[j].ocspResponseLen)
                        {
                            Console.WriteLine("Received no OCSP response for this document.");
                        }
                        else
                        {
                            string fileNameWithPath = documentsPath + "\\" + fileNames[j] + ".ors";
                            Console.WriteLine ("Writing OCSP response " + fileNameWithPath);
                            try
                            {
                                byte[] ocspResponseBuffer = new byte[documents[j].ocspResponseLen];
                                Marshal.Copy(documents[j].ocspResponse, ocspResponseBuffer, 0, documents[j].ocspResponseLen);
                                
                                FileStream ocspRespFileStream = new FileStream(fileNameWithPath, FileMode.Create, FileAccess.Write);
                                BinaryWriter ocspRespFileWriter = new BinaryWriter(ocspRespFileStream);
                                ocspRespFileWriter.Write(ocspResponseBuffer);
                                ocspRespFileWriter.Close();
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("Cannot open OCSP response output file " + fileNameWithPath +
                                                  " for writing. " + e.ToString());
                            }
                        }
                    }
                }
                else
                {
                    showLastErrorMessage();
                }

                for (int d=0; d<DOC_COUNT; d++)
                {
                    // release the buffers
                    if (documents[d].dataToBeSigned != IntPtr.Zero)
                    {
                        // free the buffer with the document's content
                        Marshal.FreeHGlobal(documents[d].dataToBeSigned);
                        documents[d].dataToBeSigned = IntPtr.Zero;
                    }

                    if (documents[d].signature != IntPtr.Zero)
                    {
                        // free the buffer with the document's signature
                        Marshal.FreeHGlobal(documents[d].signature);
                        documents[d].signature = IntPtr.Zero;
                    }

                    if (documents[d].oldSignature != IntPtr.Zero)
                    {
                        // free the buffer with the document's previous signature
                        Marshal.FreeHGlobal(documents[d].oldSignature);
                        documents[d].oldSignature = IntPtr.Zero;
                    }

                    if (documents[d].ocspResponse != IntPtr.Zero)
                    {
                        // free the buffer with the OCSP response
                        Marshal.FreeHGlobal(documents[d].ocspResponse);
                        documents[d].ocspResponse = IntPtr.Zero;
                    }

                    if (documents[d].encryptedSig != IntPtr.Zero)
                    {
                        // free the buffer with the encrypted document's signature
                        Marshal.FreeHGlobal(documents[d].encryptedSig);
                        documents[d].encryptedSig = IntPtr.Zero;
                    }

                    if (documents[d].timeStamp != IntPtr.Zero)
                    {
                        // free the buffer with the time stamp
                        Marshal.FreeHGlobal(documents[d].timeStamp);
                        documents[d].timeStamp = IntPtr.Zero;
                    }
                }
            }

            // close SecSecSigner
            Console.WriteLine ("Closing SecSigner");
            closeSecSigner();

            if (restart)
            {
                // init SecSigner again, maybe with different properties
                ret = initSecSigner(secSignerPropertyName, secSignerInstallPath);
                if (ret <0)
                {
                    showLastErrorMessage();
                    return;
                }

                // find smart card again
                ByteArray sigCert = new ByteArray();
                sigCert.data = IntPtr.Zero;
                sigCert.dataLen = 0;

                ByteArray authCert = new ByteArray();
                authCert.data = IntPtr.Zero;
                authCert.dataLen = 0;

                ByteArray encryptCert = new ByteArray();
                encryptCert.data = IntPtr.Zero;
                encryptCert.dataLen = 0;

                ret = initSmartCard(ref sigCert, ref authCert, ref encryptCert);
                if (ret < 0)
                {
                    showLastErrorMessage();
                    return;
                }

                // close SecSigner again
                Console.WriteLine ("Closing SecSigner");
                closeSecSigner();
            }

            // Unload Java virtual machine -> This is the end, another load JVM will fail.
            Console.WriteLine("Unloading JavaVM");
            ret = unloadJavaVirtualMachine();
            if (ret <0)
            {
                showLastErrorMessage();
                return;
            }

            Console.WriteLine ("SecCommerce SecSigner test program ends.");
            return;
        }
    }
}
