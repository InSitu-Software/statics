/**
 * This DLL makes SecSigner available to applications which rather access
 * a DLL than a Java application.
 *
 * Exported functions are
 * - SecSigner_LoadJavaVM
 * - SecSigner_Init
 * - SecSigner_InitSmartCard
 * - SecSigner_Sign
 * - SecSigner_Verify
 * - SecSigner_UnloadJavaVM
 * - SecSigner_Close
 * - SecSigner_GetVersion
 * - SecSigner_GetCardNumber
 * - SecSigner_GetCardName
 * - SecSigner_GetCardReaderName
 * - SecSigner_GetCardReaderFirmwareVersion
 * - SecSigner_GetErrorMessage
 *
 * @author SecCommerce Informationssysteme GmbH, Hamburg
 */

#include <windows.h>

// Der folgende ifdef-Block zeigt die Standardl￶sung zur Erstellung von Makros, die das Exportieren 
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL wurden mit dem in der Befehlszeile definierten
// Symbol CALLSECARCHIVEDLL_EXPORTS kompiliert. Dieses Symbol sollte f￼r kein Projekt definiert werden, das
// diese DLL verwendet. Auf diese Weise betrachtet jedes andere Projekt, dessen Quellcodedateien diese Datei 
// einbeziehen, CALLSECARCHIVEDLL_API-Funktionen als aus einer DLL importiert, w￤hrend diese DLL mit diesem 
// Makro definierte Symbole als exportiert betrachtet.
#ifdef CALLSECSIGNERDLL_EXPORTS
#define CALLSECSIGNERDLL_API extern "C" __declspec(dllexport)
#else
#define CALLSECSIGNERDLL_API __declspec(dllimport)
#endif

// A buffer and its length
typedef struct
{
	unsigned char* data;                // data
	int dataLen;                        // data length
} BYTEARRAY;

// Optional visual representation of a signature included into PDF documents
typedef struct
{
	BOOL pdfDisplayAnnotation;          // determine whether a verification item (JPG image) should be added to the PDF document.
	BYTEARRAY* pdfSignatureImage;       // deprecated, not used any longer.
	BOOL pdfSignatureImageText;         // This parameter is not used any more.
    BYTEARRAY* pdfLinkImage;            // deprecated, not used any longer.
    char* pdfLinkUrl;                   // deprecated, not used any longer.
	char* pdfSignatureReason;           // signature reason text to be added onto the verification item image.
	char* pdfSignatureTextAnnot;        // deprecated, not used any longer.
	char* pdfSignatureLocation;         // signature location text to be added onto the verification item image.
	int pdfSignaturePosition;           // position for a virtual box containing verification item image and link item image.
	char* pdfSigTextSizeAndPosition;    // size and position for text (signer, time, etc.) in signature annotation.
	BOOL pdfScaleSigImage;              // deprecated, not used any longer.
	BOOL pdfSigAnnotLabels;             // determines whether signature annotation data should be labeled ('am', 'in', 'Ort' ...).
	BOOL pdfSigShowDate;                // display date and time in signature annotation
	BOOL pdfSigAnnotTransparentBg;      // display a transparent background?
	int pdfSignatureWidth;              // width for pdf signature annotation
	int pdfSignatureHeight;             // height for pdf signature annotation
	BYTEARRAY* pdfSignatureSignedImage; // The default signature annotation has a "signed" image in the left bottom corner and an icon in the right bottom corner
	BYTEARRAY* pdfSignatureSignerIcon;  // The default signature annotation has a signers icon in the right bottom corner
	BYTEARRAY* pdfSigBackgroundImage;   // Display reader operated verification item as jpg (max. 220x70 pixel)
	char* pdfFormFieldName;             // The position is given by a form field. The rect entry of form fields dictionary will be used.
	char* pdfOutlineName;               // The position is given by an outline. This is not really PDF compliant but a customer's wish.
} PDFANNOTATION;



// only used for xmldsig signatures
// contains a mapping between namespace prefix and its URI
// ns:dsig=http://www.w3.org/TR/2002/REC-xmldsig-core-20020212/
typedef struct
{
    char* namespacePrefix;                  // namespace prefix 
    char* namespaceURI;                     // namespace uri
} NAMESPACEMAPPING;

// XPath transform filter for xmldsig signatures
typedef struct
{
    int transformMethod;                       // the transform method of this filter
                                               // 0 = intersect
                                               // 1 = subtract
                                               // 2 = union
    char* xpathExpr;                           // the xpath-expression which selects xml-nodes. the xml-document is intersect/union/subtract with these nodes.

    NAMESPACEMAPPING *xmlDSigNameSpaceMappings; // a list of namespace mappings which are used in transform filters to validate the filters xpath expression
    int numberOfXmlDSigNamespaces;              // the number of namespace mappings for the xmldsig transform filters
} XPATHTRANSFORMFILTER;


// Document to be signed and its signature. 
// If a value is missing the buffer will be NULL and the length 0.
typedef struct
{
    int version;                    // version = 13
    char* documentFileName;         // 0x00 terminated, will be mapped to dataURL
    unsigned char* dataToBeSigned;  // data to be signed
    int dataToBeSignedLen;          // sig-mode: IN: length of the supplied data to be signed
                                    // verify-mode: IN: length of the supplied data which were signed
                                    // verify-mode: OUT: length of extracted data which were signed
    int dataToBeSignedBufLen;       // verify-mode: IN: length of the buffer for the data which were signed
                                    //                  and are extracted from the signature by SecSigner
    int documentType;               // type of data to be signed (SIGNDATATYPE_PLAINTEXT, ...)
    int signatureFormatType;        // desired signature type (SIGNATUREFORMATTYPE_PKCS7, SIGNATUREFORMATTYPE_PDF, SIGNATUREFORMATTYPE_XMLDSIG)
    unsigned char* mimeType;        // mimetype of to be signed data (0x00 terminated), alternative to documentType
    unsigned char* oldSignature;    // optional: buffer for the old signature where the SignerInfo shall be added to
    int oldSignatureLen;            // length of the buffer
    unsigned char* signature;       // sig-mode: buffer for the returned signature
                                    // verify-mode: signature to be verified
    int signatureLen;               // sig-mode: IN: length of the buffer
                                    // sig-mode: OUT: length of the actual signature
                                    // verify-mode: IN: length of the signature
    unsigned char* encryptedSig;    // sig-mode: optional: buffer for the returned encrypted signature (if 
                                    //                     encryption certificates were supplied)
    int encryptedSigLen;            // sig-mode: IN: length of the buffer
                                    // sig-mode: OUT: length of the actual encrypted signature
    unsigned char* ocspResponse;    // verify-mode: buffer for the returned OCSP response if the certificate is checked online
                                    //     or existing OCSP response for the given signature
    int ocspResponseLen;            // verify-mode: IN: length of the supplied old OCSP response
                                    // verify-mode: OUT: length of the new OCSP response received by SecSigner
    int ocspResponseBufLen;         // verify-mode: IN: length of the buffer for a new OCSP response received by SecSigner
    unsigned char* timeStamp;       // sig-mode: buffer for the returned time stamp if one is received
                                    // verify-mode: time stamp to be verified for the given signature
    int timeStampLen;               // sig-mode: IN: length of the buffer
                                    // sig-mode: OUT: length of the actual time stamp
                                    // verify-mode: IN: length of the time stamp to be verified
    unsigned char* encryptedDoc;    // encrypt-mode: buffer for the returned encrypted document
    int encryptedDocLen;            // encrypt-mode: IN: length of the buffer
                                    // encrypt-mode: OUT: length of the actual encrypted document

    char* signatureID;              // sig-mode: IN: id which is inserted as id-attribute of the xmldsig-signature-node
    char* xmlDSigNodePath;          // sig-mode: IN: xpath within a document to specify where to insert the xmldsig-signature
    
    XPATHTRANSFORMFILTER *xmlDSigFilterPaths; // xpath transform filter which defines the parts of the xml document which is signed.
                                    // xpath-transform filter could be an intersect, subtract and union filter
    int numberOfXmlDSigFilterPaths; // sig-mode: IN: the number of tranform filters
    char* xmlDSigNameSpaceName;     // sig-mode: IN: the namespace name of the XML-DSig signature node. The URI is http://www.w3.org/2000/09/xmldsig#.
    BYTEARRAY* evidenceRecordArray; // verify-mode: IN: evidence records for signatures using expired algorithms
    int numberOfEvidenceRecords;    // verify-mode: IN: number of supplied evidence records
    char* hashAlgorithm;            // sig-mode: IN: desired hash algorithm. If null then the default of the smart card will be used.
    PDFANNOTATION* pdfAnnotation;   // sig-mode: IN: optional visual representation of the signature in the PDF document
    
    
    char* verificationReport;       // verify-mode: the verification report
    int verificationReportLen;      // verify-mode: the verification report length /  the length of suplied buffer for the verification report
    
    BOOL setUseLegacyBmuXmlSigFormat;// Determines whether redundant signer information as separate XML nodes for waste signatures
    //     (BMU ZKS) shall be added to the signature and whether the signature shall conform
    //     to an old XAdES version where SigningCertificate and SignerRole still have version 1.
    
} DOCUMENT;


// sign data type: plain text
#define SIGNDATATYPE_PLAINTEXT 0
    
// sign data type: HTML
#define SIGNDATATYPE_HTML 1

// sign data type binary
#define SIGNDATATYPE_BINARY 2

// sign data type: image
#define SIGNDATATYPE_IMAGE 3

// sign data type: PDF
#define SIGNDATATYPE_PDF 4

// sign data type: XML
#define SIGNDATATYPE_XML 12
    
// sign data type: Rich text format 
#define SIGNDATATYPE_RTF 13

// sign data type: ZIP file
#define SIGNDATATYPE_ZIP 14

// sign data type: BMU ZKS document
#define SIGNDATATYPE_ZKS 15

// signature format type: PKCS#7 CMS
#define SIGNATUREFORMATTYPE_PKCS7 0
    
// signature format type: PDF
#define SIGNATUREFORMATTYPE_PDF 3

// signature format type XML-DSig
#define SIGNATUREFORMATTYPE_XMLDSIG 4

// signature format type PKCS#7 SignedData with embedded document
#define SIGNATUREFORMATTYPE_PKCS7_EMBEDDED 5

// PDF annotation position
#define PDF_ANNOTATION_NOT_SPECIFIED 0
#define PDF_ANNOTATION_LEFT_TOP_FIRSTPAGE 1
#define PDF_ANNOTATION_CENTER_TOP_FIRSTPAGE 2
#define PDF_ANNOTATION_RIGHT_TOP_FIRSTPAGE 3
#define PDF_ANNOTATION_LEFT_BOTTOM_FIRSTPAGE 4
#define PDF_ANNOTATION_CENTER_BOTTOM_FIRSTPAGE 5
#define PDF_ANNOTATION_RIGHT_BOTTOM_FIRSTPAGE 6
#define PDF_ANNOTATION_LEFT_TOP_LASTPAGE 7
#define PDF_ANNOTATION_CENTER_TOP_LASTPAGE 8
#define PDF_ANNOTATION_RIGHT_TOP_LASTPAGE 9
#define PDF_ANNOTATION_LEFT_BOTTOM_LASTPAGE 10
#define PDF_ANNOTATION_CENTER_BOTTOM_LASTPAGE 11
#define PDF_ANNOTATION_RIGHT_BOTTOM_LASTPAGE 12


/**
 * Loads the JavaVM, checks whether all SecSigner JARs are found in the class path
 * and loads some SecSigner classes.
 *
 * @param secSignerInstallPath installation path of SecSigner
 * @param maximum size, in mega bytes, of the memory allocation pool
 * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND or METHOD_FAILED
 */
CALLSECSIGNERDLL_API int SecSigner_LoadJavaVM(char * secSignerInstallPath, int maxMem);

/**
 * Initializes SecSigner.
 *
 * This method can be called again after SecSigner_Close().
 *
 * @param secSignerPropFileName property file name of SecSigner
 * @param secSignerInstallPath installation path of SecSigner
 * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND, METHOD_FAILED, DOC_MISMATCH or CANCELED
 */
CALLSECSIGNERDLL_API int SecSigner_Init(char * secSignerPropFileName, char * secSignerInstallPath);

/**
 * Opens the SecSigner dialog for initializing smart card reader and smart card.
 * If not OK is returned then getErrorMessage() maybe called to get the error message.
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
 * @deprecated use initSmartCardRetCerts instead
 */
CALLSECSIGNERDLL_API int SecSigner_InitSmartCard();

/**
 * Opens the SecSigner dialog for initializing smart card reader and smart card.
 * If not OK is returned then getErrorMessage() maybe called to get the error message.
 *
 * @param sigCert buffer for the signature certificate of the inserted smart card
 * @param authCert buffer for the authentication certificate of the inserted smart card
 * @param encryptCert buffer for the encryption certificate of the inserted smart card
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
 */
CALLSECSIGNERDLL_API int SecSigner_InitSmartCardRetCerts(BYTEARRAY *sigCert, BYTEARRAY *authCert, BYTEARRAY *encryptCert);

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
 * @param signingKeyAndOrCertData a signing key (PKCS#8 or PKCS#12) and/or corresponding or desired certificate
 * @param signingKeyAndOrCertDataCount must be 1 (key) or 2 (key and certificate)
 * @return OK or BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED or CANCELED
 */
CALLSECSIGNERDLL_API int SecSigner_Sign(DOCUMENT documents[], int documentCount,
										BYTEARRAY cipherCerts[], int cipherCertCount,
										BYTEARRAY signingKeyAndOrCertData[], int signingKeyAndOrCertDataCount);

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
 * @return OK or SIGNATURE_INVALID, DOC_HAS_NO_SIGNATURE, BUFFER_TOO_SHORT, NOT_INITED, JVM_NOT_AVAILABLE, VERSION_MISMATCH, METHOD_NOT_FOUND, NO_MEMORY, METHOD_FAILED, DOC_MISMATCH or CANCELED
 */
CALLSECSIGNERDLL_API int SecSigner_Verify(DOCUMENT documents[], int documentCount);

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
CALLSECSIGNERDLL_API int SecSigner_VerifyExt(DOCUMENT documents[], int documentCount, BOOL offerFileOpenDlg, BOOL oscpMandatory,
	BOOL allowSig, BOOL modal);

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
CALLSECSIGNERDLL_API int SecSigner_EncryptOnly(DOCUMENT documents[], int documentCount, 
											   BYTEARRAY cipherCert[], int cipherCertCount);

/**
 * Gets the version of SecSigner.
 *
 * @param buffer buffer for the 0x00 terminated version string
 * @param bufferLen length of the buffer
 * @return OK or BUFFER_TOO_SHORT
 */
CALLSECSIGNERDLL_API int SecSigner_GetVersion(char * buffer, int bufferLen);


/**
 * Gets the maximal number of documents to be signed at once.
 *
 * @return maximal number of documents to be signed at once
 */
CALLSECSIGNERDLL_API int SecSigner_GetSignatureLimit();


/**
 * Sets a licence for SecSigner.
 *
 * @param licence buffer for licence data
 * @param licenceLen length of the buffer
 * @return OK or METHOD_FAILED
 */
CALLSECSIGNERDLL_API int SecSigner_SetLicence(unsigned char * licence, int licenceLen);


/**
 * Gets the number printed on the signature card in the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated card number of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
CALLSECSIGNERDLL_API int SecSigner_GetCardNumber(char * buffer, int bufferLen);


/**
 * Gets the name of the signature card in the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated name of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
CALLSECSIGNERDLL_API int SecSigner_GetCardName(char * buffer, int bufferLen);


/**
 * Gets the name of the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated first card reader's name
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
CALLSECSIGNERDLL_API int SecSigner_GetCardReaderName(char * buffer, int bufferLen);


/**
 * Gets the firmware version information available from the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized
 * or no information is available.
 *
 * @param buffer buffer for the 0x00 terminated name of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
CALLSECSIGNERDLL_API int SecSigner_GetCardReaderFirmwareVersion(char * buffer, int bufferLen);


/**
 * Gets the public exponent and the modulus of a certificate. The returned byte 
 * arrays are in little endian, because they are intended to be used by Microsoft.
 *
 * If not OK is returned then getErrorMessage() maybe called to get the error message.
 *
 * @param cert the DER encoded certificates
 * @param pubExp the output buffer for the public key
 * @param mod the output buffer for the modulus
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
 */
CALLSECSIGNERDLL_API int SecSigner_GetPubExpAndKeyFromCert(BYTEARRAY *cert, BYTEARRAY *pubExp, BYTEARRAY *mod);

/**
 * Gets the error message of the last failed operation. This message which is returned by 
 * this function is updated whenever a function (like SecSigner_Sign()) returns an error status.
 *
 * @param buffer buffer for the 0x00 terminated error message
 * @param bufferLen length of the buffer
 * @return OK or BUFFER_TOO_SHORT
 */
CALLSECSIGNERDLL_API int SecSigner_GetErrorMessage(char * buffer, int bufferLen);

/**
 * Closes SecSigner.
 *
 * A new instance of SecSigner can be created with SecSigner_Init() as long
 * as SecSigner_Unload_JavaVM() has not been called.
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
 */
CALLSECSIGNERDLL_API int SecSigner_Close();

/**
 * Unloads the JavaVM. Another call to SecSigner_LoadJavaVM() in the same
 * application will fail.
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
 */
CALLSECSIGNERDLL_API int SecSigner_UnloadJavaVM();
