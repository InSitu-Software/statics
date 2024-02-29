/**
 * This test programme shows how to call SecSigner via DLL.
 *
 * @author SecCommerce Informationssysteme GmbH, Hamburg
 */

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <share.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "..\CallSecSignerDLL.h"
#include <secerror.h> // in SecCommerceDev/seccommerce/c/common

// getErrorMessage parameters
typedef int (*GET_ERRORMESSAGE_TYPE)
(
	char * buffer,
	int bufferLen
);

// loadJavaVM parameters
typedef int (*LOAD_JAVAVM_TYPE)
(
    char * secSignerInstallPath,
	int maxMem
);

// init parameters
typedef int (*INIT_TYPE)
(
    char * secSignerPropertyName,
    char * secSignerInstallPath
);

// getCardNumber parameters
typedef int (*GET_CARD_NUMBER_TYPE)
(
	char * buffer,
	int bufferLen
);

// getCardName parameters
typedef int (*GET_CARD_NAME_TYPE)
(
	char * buffer,
	int bufferLen
);

// getCardReaderName parameters
typedef int (*GET_CARD_READER_NAME_TYPE)
(
	char * buffer,
	int bufferLen
);

// getCardReaderFirmwareVersion parameters
typedef int (*GET_CARD_READER_FIRMWARE_VERSION_TYPE)
(
	char * buffer,
	int bufferLen
);

// getVersion parameters
typedef int (*GET_VERSION_TYPE)
(
	char * buffer,
	int bufferLen
);

// getSignatrureLimit parameters
typedef int (*GET_SIGNATURE_LIMIT_TYPE)
(
);

// setLicence parameters
typedef int (*SET_LICENCE_TYPE)
(
	unsigned char * licence,
	int licenceLen
);

// close parameters
typedef int (*CLOSE_TYPE)
(
);

// unloadJavaVM parameters
typedef int (*UNLOAD_JAVAVM_TYPE)
(
);

// old initSmartCardMgr parameters
typedef int (*INIT_SMARTCARD_TYPE)
(
);

// new initSmartCardMgr parameters with returned certificates
typedef int (*INIT_SMARTCARD_RET_CERTS_TYPE)
(
	BYTEARRAY *sigCert,    // buffer for the signature certificate of the inserted smart card
	BYTEARRAY *authCert,   // buffer for the authentication certificate of the inserted smart card
	BYTEARRAY *cipherCert  // buffer for the encryption certificate of the inserted smart card
);

// sign parameters
typedef int (*SIGN_TYPE)
(
	DOCUMENT document[],    // documents to be signed
	int documentCount,		// length of the document array
	BYTEARRAY cipherCerts[],// encrypt the signed PKCS#7 object with these certificates 
	int cipherCertCount,    // number of encryption certificates or just offer encryption dialog if cipherCertCount = 0
	BYTEARRAY signingKeyAndOrCertData[],// a signing key (PKCS#8 or PKCS#12) and/or corresponding or desired certificate
	int signingKeyAndOrCertDataCount    // must be 1 (key) or 2 (key and certificate)
);

// verify parameters
typedef int (*VERIFY_TYPE)
(
	DOCUMENT document[],	// documents and signatures to be verified
	int documentCount		// length of the document array
);


// encryption parameters
typedef int (*ENCRYPT_TYPE)
(
	DOCUMENT document[],    // documents to be signed
	int documentCount,		// length of the document array
	BYTEARRAY cipherCert[], // certificates for which the documents will be encrypted
	int cipherCertCount     // number of certificates
);

// function pointer into the loaded library: SecSigner_GetErrorMessage()
GET_ERRORMESSAGE_TYPE GET_ERRORMESSAGE;

// function pointer into the loaded library: SecSigner_LoadJavaVM()
LOAD_JAVAVM_TYPE LOAD_JAVAVM;

// function pointer into the loaded library: SecSigner_Init()
INIT_TYPE INIT;

// function pointer into the loaded library: SecSigner_UnloadJavaVM()
UNLOAD_JAVAVM_TYPE UNLOAD_JAVAVM;

// function pointer into the loaded library: SecSigner_Close()
CLOSE_TYPE CLOSE;

// function pointer into the loaded library: SecSigner_InitSmartCard()
INIT_SMARTCARD_TYPE INIT_SMARTCARD;

// function pointer into the loaded library: SecSigner_InitSmartCardRetCerts()
INIT_SMARTCARD_RET_CERTS_TYPE INIT_SMARTCARD_RET_CERTS;

// function pointer into the loaded library: SecSigner_Sign()
SIGN_TYPE SIGN;

// function pointer into the loaded library: SecSigner_Verify()
VERIFY_TYPE VERIFY;

// function pointer into the loaded library: SecSigner_EncryptOnly()
ENCRYPT_TYPE ENCRYPT;

// function pointer into the loaded library: SecSigner_SetLicence()
SET_LICENCE_TYPE SET_LICENCE;

// function pointer into the loaded library: SecSigner_GetSignatureLimit()
GET_SIGNATURE_LIMIT_TYPE GET_SIGNATURE_LIMIT;

// function pointer into the loaded library: SecSigner_GetVersion()
GET_VERSION_TYPE GET_VERSION;

// function pointer into the loaded library: SecSigner_GetCardNumber()
GET_CARD_NUMBER_TYPE GET_CARD_NUMBER;

// function pointer into the loaded library: SecSigner_GetCardName()
GET_CARD_NAME_TYPE GET_CARD_NAME;

// function pointer into the loaded library: SecSigner_GetCardReaderName()
GET_CARD_READER_NAME_TYPE GET_CARD_READER_NAME;

// function pointer into the loaded library: SecSigner_GetCardReaderFirmwareVersion()
GET_CARD_READER_FIRMWARE_VERSION_TYPE GET_CARD_READER_FIRMWARE_VERSION;

// buffer length for returned signature
const int SIG_BUF_LEN = 8000; // enough for detached CMS (PKCS#7) signatures
const int CERT_BUF_LEN = 5000; // should always be enough
const int CONTENT_BUF_LEN = 1500000;
const int VERSION_BUF_LEN = 1000;
const int ERS_BUF_LEN = 1000000;

// handle to SecSigner DLL
HINSTANCE hSecSignerDLL;


/**
 * Loads the DLL which calls SecSigner. Function pointers are set here.
 * @param secSignerDllName name and path of the SecSigner DLL
 */
int loadSecSignerDLL(char * secSignerDllName)
{
    HMODULE hMod;

	// Load DLL
    hSecSignerDLL = LoadLibrary(secSignerDllName);
    if (NULL == hSecSignerDLL)
    {
		fprintf(stderr, "Error: SecSigner start DLL (%s) not loaded\n", secSignerDllName);
        return -1;
    }    
    
    hMod = GetModuleHandle(secSignerDllName);
    
	// pointer to loadJavaVM()
	LOAD_JAVAVM = (LOAD_JAVAVM_TYPE) GetProcAddress(hMod, "SecSigner_LoadJavaVM");
    if (NULL == LOAD_JAVAVM)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_LoadJavaVM\".\n");
        return -1;
    }

	// pointer to init()
	INIT = (INIT_TYPE) GetProcAddress(hMod, "SecSigner_Init");
    if (NULL == INIT)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_Init\".\n");
        return -1;
    }

	// pointer to unloadJavaVM()
	UNLOAD_JAVAVM = (UNLOAD_JAVAVM_TYPE) GetProcAddress(hMod, "SecSigner_UnloadJavaVM");
    if (NULL == UNLOAD_JAVAVM)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_UnloadJavaVM\".\n");
        return -1;
    }

	// pointer to close()
	CLOSE = (CLOSE_TYPE) GetProcAddress(hMod, "SecSigner_Close");
    if (NULL == CLOSE)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_Close\".\n");
        return -1;
    }

	// pointer to old initSmartCard()
    INIT_SMARTCARD = (INIT_SMARTCARD_TYPE) GetProcAddress(hMod, "SecSigner_InitSmartCard");
    if (NULL == INIT_SMARTCARD)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_InitSmartCard\".\n");
        return -1;
    }

	// pointer to initSmartCardRetCerts()
    INIT_SMARTCARD_RET_CERTS = (INIT_SMARTCARD_RET_CERTS_TYPE) GetProcAddress(hMod, "SecSigner_InitSmartCardRetCerts");
    if (NULL == INIT_SMARTCARD_RET_CERTS)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_InitSmartCardRetCerts\".\n");
        return -1;
    }

	// pointer to sign()
    SIGN = (SIGN_TYPE) GetProcAddress(hMod, "SecSigner_Sign");
    if (NULL == SIGN)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_Sign\".\n");
        return -1;
    }

	// pointer to verify()
    VERIFY = (VERIFY_TYPE) GetProcAddress(hMod, "SecSigner_Verify");
    if (NULL == VERIFY)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_Verify\".\n");
        return -1;
    }

	// pointer to encryptOnly()
    ENCRYPT = (ENCRYPT_TYPE) GetProcAddress(hMod, "SecSigner_EncryptOnly");
    if (NULL == ENCRYPT)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_EncryptOnly\".\n");
        return -1;
    }

	// pointer to getErrorMessage()
    GET_ERRORMESSAGE = (GET_ERRORMESSAGE_TYPE) GetProcAddress(hMod, "SecSigner_GetErrorMessage");
    if (NULL == GET_ERRORMESSAGE)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetErrorMessage\".\n");
        return -1;
    }

	// pointer to SecSigner_GetCardNumber()
    GET_CARD_NUMBER = (GET_CARD_NUMBER_TYPE) GetProcAddress(hMod, "SecSigner_GetCardNumber");
    if (NULL == GET_CARD_NUMBER)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetCardNumber\".\n");
        return -1;
    }

	// pointer to SecSigner_GetCardName()
    GET_CARD_NAME = (GET_CARD_NAME_TYPE) GetProcAddress(hMod, "SecSigner_GetCardName");
    if (NULL == GET_CARD_NAME)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetCardName\".\n");
        return -1;
    }

	// pointer to SecSigner_GetCardReaderName()
    GET_CARD_READER_NAME = (GET_CARD_READER_NAME_TYPE) GetProcAddress(hMod, "SecSigner_GetCardReaderName");
    if (NULL == GET_CARD_READER_NAME)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetCardReaderName\".\n");
        return -1;
    }

	// pointer to SecSigner_GetCardReaderFirmwareVersion()
    GET_CARD_READER_FIRMWARE_VERSION = (GET_CARD_READER_FIRMWARE_VERSION_TYPE) GetProcAddress(hMod, "SecSigner_GetCardReaderFirmwareVersion");
    if (NULL == GET_CARD_READER_FIRMWARE_VERSION)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetCardReaderFirmwareVersion\".\n");
        return -1;
    }

	// pointer to SecSigner_GetVersion()
    GET_VERSION = (GET_VERSION_TYPE) GetProcAddress(hMod, "SecSigner_GetVersion");
    if (NULL == GET_VERSION)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetVersion\".\n");
        return -1;
    }

	// pointer to SecSigner_GetSignatureLimit()
    GET_SIGNATURE_LIMIT = (GET_SIGNATURE_LIMIT_TYPE) GetProcAddress(hMod, "SecSigner_GetSignatureLimit");
    if (NULL == GET_SIGNATURE_LIMIT)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_GetSignatureLimit\".\n");
        return -1;
    }

	// pointer to SecSigner_GetVersion()
    SET_LICENCE = (SET_LICENCE_TYPE) GetProcAddress(hMod, "SecSigner_SetLicence");
    if (NULL == SET_LICENCE)
    {
        fprintf(stderr, "Error: SecSigner DLL does not contain function \"SecSigner_SetLicence\".\n");
        return -1;
    }

	return 0;
}

/**
 * Loads the JavaVM, checks whether all SecSigner JARs are found in the class path
 * and loads some SecSigner classes.
 *
 * @param secSignerInstallPath installation path of SecSigner
 * @param maximum size, in mega bytes, of the memory allocation pool
 * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND or METHOD_FAILED
 */
int loadJavaVirtualMachine(char * secSignerInstallPath, int maxMem)
{
	printf ("Loading JavaVM with %d MB maximum memory\n", maxMem);
	int ret = (*LOAD_JAVAVM)(secSignerInstallPath, maxMem);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner loadJavaVM failed.\n");
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s", errorMsg);
		}

		return -1;
	}

	return 0;
}    

/**
 * Initializes SecSigner.
 *
 * This method can be called again after SecSigner_Close().
 *
 * @param secSignerPropFileName property file name of SecSigner
 * @param secSignerInstallPath installation path of SecSigner
 * @return OK or MISSING_PARAMETER, JVM_NOT_AVAILABLE, NO_MEMORY, METHOD_NOT_FOUND, CLASS_NOT_FOUND, METHOD_FAILED, DOC_MISMATCH or CANCELED
 */
int initSecSigner(char * secSignerPropertyName, char * secSignerInstallPath)
{
	int ret = (*INIT)(secSignerPropertyName, secSignerInstallPath);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner init failed.\n");
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s", errorMsg);
		}

		return -1;
	}

	return 0;
}    


/**
 * Closes SecSigner.
 *
 * A new instance of SecSigner can be created with SecSigner_Init() as long
 * as SecSigner_Unload_JavaVM() has not been called.
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
 */
int closeSecSigner()
{
	int ret = (*CLOSE)();
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner close failed.\n");
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s", errorMsg);
		}

		return -1;
	}

	return 0;
}    

/**
 * Unloads the JavaVM. Another call to SecSigner_LoadJavaVM() in the same
 * application will fail.
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED
 */
int unloadJavaVirtualMachine()
{
	int ret = (*UNLOAD_JAVAVM)();
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner unloadJavaVM failed.\n");
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s", errorMsg);
		}

		return -1;
	}

	return 0;
}    


/**
 * Opens the SecSigner dialog for initializing smart card reader and smart card.
 * If not OK is returned then getErrorMessage() maybe called to get the error message.
 * @param sigCert buffer for the signature certificate of the inserted smart card
 * @param authCert buffer for the authentication certificate of the inserted smart card
 * @param cipherCert buffer for the encryption certificate of the inserted smart card
 *
 * @return OK or NOT_INITED, JVM_NOT_AVAILABLE, METHOD_NOT_FOUND, METHOD_FAILED or CANCELED
 */
int initSmartCard(BYTEARRAY *sigCert, BYTEARRAY *authCert, BYTEARRAY *cipherCert)
{
	int ret = (*INIT_SMARTCARD_RET_CERTS)(sigCert, authCert, cipherCert);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.initSmartCard with certificate return failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}


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
int signDocs(DOCUMENT documents[], int documentCount, 
			 BYTEARRAY cipherCerts[], int cipherCertCount,
			 BYTEARRAY signingKeyAndOrCertData[], int signingKeyAndOrCertDataCount)
{
	int ret = (*SIGN)(documents, documentCount, cipherCerts, cipherCertCount, signingKeyAndOrCertData, signingKeyAndOrCertDataCount);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.signDocuments failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}


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
int verifyDocs(DOCUMENT documents[], int documentCount)
{
	int ret = (*VERIFY)(documents, documentCount);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner verifyDocuments failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}

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
int encryptDocs(DOCUMENT documents[], int documentCount, BYTEARRAY cipherCert[], int cipherCertCount)
{
	int ret = (*ENCRYPT)(documents, documentCount, cipherCert, cipherCertCount);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.encryptDataOnly failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}

/**
 * Gets the number printed on the signature card in the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated card number of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
int getCardNumber(char * buffer, int bufferLen)
{
	int ret = (*GET_CARD_NUMBER)(buffer, bufferLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.getCardNumber() failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}

/**
 * Sets a licence for SecSigner..
 * OK will be returned if the licence has been accepted.
 *
 * @param licence licence data
 * @param licenceLen length of licence data
 * @return OK, METHOD_FAILED
 */
int setLicence(unsigned char * licence, int licenceLen)
{
	int ret = (*SET_LICENCE)(licence, licenceLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.setLicence() failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	int limit = (*GET_SIGNATURE_LIMIT)();
	if (limit < 0)
	{
        fprintf(stdout, "Error: SecSigner.getSignatureLimit() failed. ret=%d\n", limit);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}
	else
	{
		printf ("Signature limit = %d\n", limit);
	}

	return ret;
}

/**
 * Gets the name of the signature card in the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated name of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
int getCardName(char * buffer, int bufferLen)
{
	int ret = (*GET_CARD_NAME)(buffer, bufferLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.getCardName() failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}

/**
 * Gets the name of the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized.
 *
 * @param buffer buffer for the 0x00 terminated first card reader's name
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
int getCardReaderName(char * buffer, int bufferLen)
{
	int ret = (*GET_CARD_READER_NAME)(buffer, bufferLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.getCardReaderName() failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}

/**
 * Gets the firmware version information available from the first card reader.
 * SMARTCARD_REMOVED will be returned if that card has not yet been initialized
 * or no information is available.
 *
 * @param buffer buffer for the 0x00 terminated name of the signature card in first card reader
 * @param bufferLen length of the buffer
 * @return OK, BUFFER_TOO_SHORT or SMARTCARD_REMOVED
 */
int getCardReaderFirmwareVersion(char * buffer, int bufferLen)
{
	int ret = (*GET_CARD_READER_FIRMWARE_VERSION)(buffer, bufferLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.getCardReaderFirmwareVersion() failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}


/**
 * Gets the version of SecSigner.
 *
 * @param buffer buffer for the 0x00 terminated version string
 * @param bufferLen length of the buffer
 * @return OK or BUFFER_TOO_SHORT
 */
int getVersion(char * buffer, int bufferLen)
{
	int ret = (*GET_VERSION)(buffer, bufferLen);
	if (ret < 0)
	{
        fprintf(stderr, "Error: SecSigner.getVersion failed. ret=%d\n", ret);
		char errorMsg[5000];
		errorMsg[0] = 0; // empty string
		if (0 == (*GET_ERRORMESSAGE)(errorMsg, 5000))
		{
			fprintf(stderr, "Message: %s\n", errorMsg);
		}
	}

	return ret;
}


/**
 * Test main. SecArchiveClient is started.
 */
int main(int argc, char* argv[])
{
	if (argc < 6)
	{
		printf ("usage TestCallSecSignerDLL <test mode> <CallSecSignerDLL> <SecSignerPropertyName> <SecSignerInstallPath> <maxMemMB> ... \n");
		return 1;
	}

	// read command line parameters
	char * option = argv[1];
	char * secSignerDllName = argv[2];
	char * secSignerPropertyName = argv[3];
	char * secSignerInstallPath = argv[4];
	char * maxMemStr = argv[5];

	// path of documents to be signed
	char * documentsPath = NULL;

	bool signGivenDocs = false;
	bool verifyGivenDocs = false;
	bool encryptGivenDocs = false;
	bool restart = false;
	bool setSecSignerLicence = false;

	if ((option[0] == '2') || (option[0] == '4') || (option[0] == '5'))
	{
		// read command line parameter
		if (argc < 7)
		{
			printf ("parameter <documentsPath> missing\n");
			return 6;
		}

		documentsPath = argv[6];
		printf ("taking test documents from = %s\n", documentsPath);

		if (option[0] == '5')
		{
			encryptGivenDocs = true;
		}
		else
		{
			signGivenDocs=true;
		}
		
		if (option[0] == '4')
		{
			restart=true;
		}
	}
	else if (option[0] == '3')
	{
		// read command line parameter
		if (argc < 7)
		{
			printf ("parameter <documentsPath> missing\n");
			return 6;
		}

		documentsPath = argv[6];
		printf ("taking test documents from = %s\n", documentsPath);

		verifyGivenDocs=true;
	}
	else if (option[0] == '6')
	{
		documentsPath = argv[6];
		printf ("taking test documents from = %s\n", documentsPath);
		setSecSignerLicence = true;
	}
	else
	{
		printf ("The first parameter has to be\n");
		printf ("  '2' = test signature of given documents\n");
		printf ("  '3' = test verification of given documents\n");
		printf ("  '4' = test signature of given documents and restart\n");
		printf ("  '5' = test encryption only of given documents\n");
		printf ("  '6' = test set licence\n");
		return 2;
	}

	// Load DLL
	printf("Loading SecSigner DLL %s\n", secSignerDllName);
	int ret = loadSecSignerDLL(secSignerDllName);
	if (ret <0)
	{
		return ret;
	}

	int maxMem = 0;
	int maxMemConvertCount = sscanf_s(maxMemStr, "%d", &maxMem);
	if (1 > maxMemConvertCount)
	{
		printf("%s is no integer. MaxMem defaults to 64 MB\n", maxMemStr);
		maxMem= 64;
	}

	// Load Java virtual machine
	printf("Loading JavaVM\n");
	ret = loadJavaVirtualMachine(secSignerInstallPath, maxMem);
	if (ret <0)
	{
		return ret;
	}

	// init SecSecSigner
	ret = initSecSigner(secSignerPropertyName, secSignerInstallPath);
	if (ret <0)
	{
		return ret;
	}

	char * versionBuf = (char*)malloc(VERSION_BUF_LEN);
	ret = getVersion(versionBuf, VERSION_BUF_LEN);
	if (ret < 0)
	{
		return ret;
	}
	printf ("SecSigner version = %s\n", versionBuf);
	free (versionBuf);

	// find smart card
	if (!verifyGivenDocs && !encryptGivenDocs && !setSecSignerLicence)
	{
		// buffer for returned signature certificate
		BYTEARRAY *sigCert = (BYTEARRAY*) malloc(sizeof BYTEARRAY);
		if (NULL == sigCert)
		{
			printf("No memory for signature certificate buf\n");
			return -1;
		}
		sigCert->data = (unsigned char*)malloc(CERT_BUF_LEN);
		sigCert->dataLen = CERT_BUF_LEN;
		if (NULL == sigCert->data)
		{
			printf("No memory for signature certificate buffer\n");
			return -1;
		}

		// buffer for returned authentication certificate
		BYTEARRAY *authCert =(BYTEARRAY*) malloc(sizeof BYTEARRAY);
		if (NULL == authCert)
		{
			printf("No memory for authentication certificate buf\n");
			return -1;
		}
		authCert->data = (unsigned char*)malloc(CERT_BUF_LEN);
		authCert->dataLen = CERT_BUF_LEN;
		if (NULL == authCert->data)
		{
			printf("No memory for authentication certificate buffer\n");
			return -1;
		}

		// buffer for returned encryption certificate
		BYTEARRAY *encryptCert =(BYTEARRAY*) malloc(sizeof BYTEARRAY);
		if (NULL == encryptCert)
		{
			printf("No memory for encryption certificate buf\n");
			return -1;
		}
		encryptCert->data = (unsigned char*)malloc(CERT_BUF_LEN);
		encryptCert->dataLen = CERT_BUF_LEN;
		if (NULL == encryptCert->data)
		{
			printf("No memory for encryption certificate buffer\n");
			return -1;
		}
		
		ret = initSmartCard(sigCert, authCert, encryptCert);
		if (ret <0)
		{
			return ret;
		}

		// write the returned signature certificate to a file
		if (sigCert->dataLen > 0)
		{
			char * sigCertFileName = "sigCert.der";
			printf ("Writing signature certificate to %s\n", sigCertFileName);
			int fd;
			int openRet = _sopen_s(&fd, sigCertFileName, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
			if (0 != openRet)
			{
				printf("Cannot open signature certificate output file %s for writing.\n", sigCertFileName);
				return -1;
			}

			_write(fd, sigCert->data, sigCert->dataLen);
			_close(fd);
		}

		// write the returned authentication certificate to a file
		if (authCert->dataLen > 0)
		{
			char * authCertFileName = "authCert.der";
			printf ("Writing authentication certificate to %s\n", authCertFileName);
			int fd;
			int openRet = _sopen_s(&fd, authCertFileName, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
			if (0 != openRet)
			{
				printf("Cannot open authentication certificate output file %s for writing.\n", authCertFileName);
				return -1;
			}

			_write(fd, authCert->data, authCert->dataLen);
			_close(fd);
		}

		// write the returned encryption certificate to a file
		if (encryptCert->dataLen > 0)
		{
			char * encryptCertFileName = "encryptCert.der";
			printf ("Writing encryption certificate to %s\n", encryptCertFileName);
			int fd;
			int openRet = _sopen_s(&fd, encryptCertFileName, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
			if (0 != openRet)
			{
				printf("Cannot open encryption certificate output file %s for writing.\n", encryptCertFileName);
				return -1;
			}

			_write(fd, encryptCert->data, encryptCert->dataLen);
			_close(fd);
		}

		// free the buffers
		free(sigCert->data);
		free(sigCert);

		free(authCert->data);
		free(authCert);

		free(encryptCert->data);
		free(encryptCert);

		// get the number printed on the signature card in the first card reader
	    char * cardNumberBuf = (char*)malloc(VERSION_BUF_LEN);
	    ret = getCardNumber(cardNumberBuf, VERSION_BUF_LEN);
		if (ret < 0)
		{
			return ret;
		}
		printf ("Number printed on the smart card in the first reader = %s\n", cardNumberBuf);
		free (cardNumberBuf);

		// get the name of the smart card in the first reader
	    char * cardNameBuf = (char*)malloc(VERSION_BUF_LEN);
	    ret = getCardName(cardNameBuf, VERSION_BUF_LEN);
		if (ret < 0)
		{
			return ret;
		}
		printf ("Name of the smart card in the first reader = %s\n", cardNameBuf);
		free (cardNameBuf);

		// get the name of the first card reader
	    char * cardReaderNameBuf = (char*)malloc(VERSION_BUF_LEN);
	    ret = getCardReaderName(cardReaderNameBuf, VERSION_BUF_LEN);
		if (ret < 0)
		{
			return ret;
		}
		printf ("Name of the first card reader = %s\n", cardReaderNameBuf);
		free (cardReaderNameBuf);

		// get the firmware version of the first card reader
	    char * cardReaderFirmwareVersionBuf = (char*)malloc(VERSION_BUF_LEN);
	    ret = getCardReaderFirmwareVersion(cardReaderFirmwareVersionBuf, VERSION_BUF_LEN);
		if (ret < 0)
		{
			return ret;
		}
		printf ("Firmware of the first card reader = %s\n", cardReaderFirmwareVersionBuf);
		free (cardReaderFirmwareVersionBuf);
	}

	if (setSecSignerLicence)
	{
		// read test licence file
		int fd;
		#define FILE_NAME_WITH_PATH_LEN 200
		char fileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s", documentsPath, "test-licence.pkcs7");
		printf ("Reading %s\n", fileNameWithPath);
		int openRet = _sopen_s(&fd, fileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
		if (0 != openRet)
		{
			printf("Cannot open test data %s\n", "test-licence.pkcs7");
			return -1;
		}
		
		unsigned char * licenceBuffer;
		licenceBuffer = (unsigned char*)malloc(CONTENT_BUF_LEN +1); // 1 byte more to notice if the file is too large
		size_t filePos = 0;
		while (true)
		{
			int bytesRead = _read(fd, &licenceBuffer[filePos], CONTENT_BUF_LEN +1 -filePos);
			
			if (bytesRead < 0)
			{
				printf("Cannot read test data %s\n", "test-licence.pkcs7");
				return -1;
			}

			if (bytesRead == 0)
			{
				break;
			}
	
			filePos+=bytesRead;
		}
			
		_close(fd);
		if (filePos > CONTENT_BUF_LEN)
		{
			printf("Test licence file %s is larger than the buffer of %d bytes.\n", "test-licence.pkcs7", CONTENT_BUF_LEN);
			return -1;
		}

		// call Signer.setLicence
		printf ("Calling SecSigner signature\n");
		ret = setLicence(licenceBuffer, filePos);
		printf("signature return value =%d\n", ret);
	}

	if (signGivenDocs)
	{
		// read test documents and pass them to SecArchive
		const int DOCCOUNT = 16;
		char * fileNames[DOCCOUNT];
#define FILE_NAME_WITH_PATH_LEN 200
		char fileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		unsigned char * contentBuffer;
		unsigned char * signatureBuffer;
		unsigned char * encrSigBuffer;
		unsigned char * timestampBuffer;
		size_t filePos;
		DOCUMENT documents[DOCCOUNT];

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
				sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s", documentsPath, fileNames[i]);
				printf ("Reading %s\n", fileNameWithPath);

				// buffer for to be signed content
				contentBuffer = (unsigned char*)malloc(CONTENT_BUF_LEN +1); // 1 byte more to notice if the file is too large
				if (NULL == contentBuffer)
				{
					printf("No memory for content buffer\n");
					return -1;
				}

				// buffer for returned signature
				signatureBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
				if (NULL == signatureBuffer)
				{
					printf("No memory for content buffer\n");
					return -1;
				}

				// buffer for returned encrypted signature (if encryption certificates are supplied)
				encrSigBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
				if (NULL == encrSigBuffer)
				{
					printf("No memory for encrypted signature buffer\n");
					return -1;
				}
	
				// buffer for returned time stamp
				timestampBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
				if (NULL == timestampBuffer)
				{
					printf("No memory for time stamp buffer\n");
					return -1;
				}
	
				// read test file
				int fd;
			    int openRet = _sopen_s(&fd, fileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
				if (0 != openRet)
				{
					printf("Cannot open test data %s\n", fileNameWithPath);
					return -1;
				}

				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &contentBuffer[filePos], CONTENT_BUF_LEN +1 -filePos);
			
					if (bytesRead < 0)
					{
						printf("Cannot read test data %s\n", fileNameWithPath);
						return -1;
					}

					if (bytesRead == 0)
					{
						break;
					}
	
					filePos+=bytesRead;
				}
			
				_close(fd);
				if (filePos > CONTENT_BUF_LEN)
				{
					printf("Test data file %s is larger than the buffer of %d bytes.\n", fileNameWithPath, CONTENT_BUF_LEN);
					return -1;
				}

				// fill document struct
				documents[i].version = 13;
				documents[i].documentFileName = fileNames[i];
				documents[i].dataToBeSigned = contentBuffer;
				documents[i].dataToBeSignedLen = filePos;
				documents[i].dataToBeSignedBufLen = 0; // not used in signature mode
				documents[i].documentType = SIGNDATATYPE_PLAINTEXT;
				documents[i].signatureFormatType = SIGNATUREFORMATTYPE_PKCS7;
				documents[i].mimeType = NULL;
				documents[i].oldSignature = NULL; // a previous signature for the same document could be set here
				documents[i].oldSignatureLen = 0;
				documents[i].signature = signatureBuffer;
				documents[i].signatureLen = SIG_BUF_LEN;
				documents[i].encryptedSig = encrSigBuffer;
				documents[i].encryptedSigLen = SIG_BUF_LEN;
				documents[i].ocspResponse = NULL;
				documents[i].ocspResponseLen = 0;
				documents[i].ocspResponseBufLen = 0;
				documents[i].timeStamp = timestampBuffer;
				documents[i].timeStampLen = SIG_BUF_LEN;
				documents[i].encryptedDoc = NULL;
				documents[i].encryptedDocLen = 0;
				documents[i].hashAlgorithm = NULL; // NULL = "autoselect", "SHA1", "SHA256", "SHA512", ...
				documents[i].verificationReport = NULL; // dont need the verification report
				documents[i].verificationReportLen = 0;

				// example values for xml dsig signature
				// to use xmldsig specific values, signatureFormatType has to be set SIGNATUREFORMATTYPE_XMLDSIG;
				documents[i].signatureID = NULL; // "Sig-1234567"; // the id is inserted as value of id-attribute of the signature node
				documents[i].xmlDSigNodePath = NULL; // "/"; // put the signature under the root element
                documents[i].setUseLegacyBmuXmlSigFormat = FALSE;
                
				XPATHTRANSFORMFILTER transformFilter[2];
				transformFilter[0].transformMethod = 1; // subtract
				transformFilter[0].xpathExpr = "here()/ancestor-or-self::*/dsig:Signature"; // which nodes have to be subtracted
				transformFilter[0].numberOfXmlDSigNamespaces = 0;
				transformFilter[0].xmlDSigNameSpaceMappings = NULL;

				NAMESPACEMAPPING namespaceMappings[1];
				namespaceMappings[0].namespacePrefix = "dsig";
				namespaceMappings[0].namespaceURI = "http://www.w3.org/TR/xmldsig-core/";

				transformFilter[1].transformMethod = 0; // intersect
				transformFilter[1].xpathExpr = "/descendant::*[local-name()='BGSERZLayer']"; // which nodes have to be subtracted
				transformFilter[1].numberOfXmlDSigNamespaces = 1;
				transformFilter[1].xmlDSigNameSpaceMappings = namespaceMappings;

				documents[i].numberOfXmlDSigFilterPaths = 0; //2; // number of transform filter in the array. this is used to de-reference arrays values
				documents[i].xmlDSigFilterPaths = NULL; // transformFilter; 
				documents[i].xmlDSigNameSpaceName = NULL;   // the namespace name of the XML-DSig signature node. The URI is http://www.w3.org/2000/09/xmldsig#.
				
				documents[i].evidenceRecordArray = NULL; // not used in signature mode
				documents[i].numberOfEvidenceRecords = 0; // not used in signature mode
				
				PDFANNOTATION pdfAnnotation[1]; // optional visual representation of the signature in the PDF document
				documents[i].pdfAnnotation = NULL; // pdfAnnotation;
				pdfAnnotation[0].pdfDisplayAnnotation = TRUE; // determine whether a verification item (JPG image) should be added to the PDF document.
				pdfAnnotation[0].pdfSignatureImageText = TRUE; // determines whether a text should be added onto the verification item image data.
				pdfAnnotation[0].pdfSignatureReason = NULL; // signature reason text to be added onto the verification item image.
				pdfAnnotation[0].pdfSignatureLocation = NULL; // signature location text to be added onto the verification item image.
				pdfAnnotation[0].pdfSignaturePosition = NULL; // position for a virtual box containing verification item image and link item image.
				pdfAnnotation[0].pdfSigTextSizeAndPosition = NULL; // size and position for text (signer, time, etc.) in signature annotation.
				pdfAnnotation[0].pdfSigAnnotLabels = TRUE; // determines whether signature annotation data should be labeled ('am', 'in', 'Ort' ...).
                pdfAnnotation[0].pdfSigShowDate = TRUE; // display date and time in signature annotation
                pdfAnnotation[0].pdfSigAnnotTransparentBg = FALSE;      // display a transparent background?
                pdfAnnotation[0].pdfSignatureWidth = 220; // width for pdf signature annotation
                pdfAnnotation[0].pdfSignatureHeight = 70; // height for pdf signature annotation
                pdfAnnotation[0].pdfSignatureSignedImage = NULL; // The default signature annotation has a "signed" image in the left bottom corner and an icon in the right bottom corner
                pdfAnnotation[0].pdfSignatureSignerIcon = NULL; // The default signature annotation has a signers icon in the right bottom corner
                pdfAnnotation[0].pdfSigBackgroundImage = NULL; // Display reader operated verification item as jpg (max. 220x70 pixel)
                pdfAnnotation[0].pdfFormFieldName = NULL; // Display reader operated verification item as jpg (max. 220x70 pixel)
                pdfAnnotation[0].pdfOutlineName = NULL; // The position is given by a form field. The rect entry of form fields dictionary will be used.
			}

			BYTEARRAY * cipherCerts = NULL; // no encryption certificates in this example
			int cipherCertCount = -1;       // do not show encryption certificate dialog
			BYTEARRAY * signingKeyAndOrCertData = NULL; // no software key in this example
			int signingKeyAndOrCertDataCount = 0;

			// call Signer signature process
			printf ("Calling SecSigner signature\n");
			ret = signDocs(documents, DOCCOUNT, cipherCerts, cipherCertCount, signingKeyAndOrCertData, signingKeyAndOrCertDataCount);
			printf("signature return value =%d\n", ret);

			if (ret == OK)
			{
				// write the returned signatures to files
				for (int j=0; j<DOCCOUNT; j++)
				{
					sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.pkcs7", documentsPath, fileNames[j]);
	
					printf ("Writing signature %s\n", fileNameWithPath);
					int fd;
					int openRet = _sopen_s(&fd, fileNameWithPath, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
					if (0 != openRet)
					{
						printf("Cannot open signature output file %s for writing.\n", fileNameWithPath);
						return -1;
					}

					_write(fd, documents[j].signature, documents[j].signatureLen);
					_close(fd);

					// write the returned encrypted signature (if encryption certificates were supplied,
					// which is not possible currently)
					if (documents[j].encryptedSigLen > 0)
					{
						sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s_encrypted.pkcs7", documentsPath, fileNames[j]);

						printf ("Writing encrypted signature %s\n", fileNameWithPath);
						int fd;
						int openRet = _sopen_s(&fd, fileNameWithPath, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
						if (0 != openRet)
						{
							printf("Cannot open encrypted signature output file %s for writing.\n", fileNameWithPath);
							return -1;
						}

						_write(fd, documents[j].encryptedSig, documents[j].encryptedSigLen);
					
						_close(fd);
					}
				}
			}

			for (int d=0; d<DOCCOUNT; d++)
			{
				// release the buffers
				if (documents[d].dataToBeSigned != NULL)
				{
					free (documents[d].dataToBeSigned);
					documents[d].dataToBeSigned = NULL;
				}

				if (documents[d].signature != NULL)
				{
					free (documents[d].signature);
					documents[d].signature = NULL;
				}

				if (documents[d].oldSignature != NULL)
				{
					free (documents[d].oldSignature);
					documents[d].oldSignature = NULL;
				}

				if (documents[d].encryptedSig != NULL)
				{
					free (documents[d].encryptedSig);
					documents[d].encryptedSig = NULL;
				}

				if (documents[d].timeStamp != NULL)
				{
					free (documents[d].timeStamp);
					documents[d].timeStamp = NULL;
				}
			}
		}
	}
	else if (verifyGivenDocs)
	{
		// read test documents and verify them in SecSigner
		char * fileNames[14];
		char fileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char docFileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char sigFileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char timestampFileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char ocspRespFileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char evidenceRec1FileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		char evidenceRec2FileNameWithPath[FILE_NAME_WITH_PATH_LEN];
	
		unsigned char * contentBuffer;
		unsigned char * signatureBuffer;
		unsigned char * sigAfterVerifyBuffer;
		unsigned char * timestampBuffer;
		unsigned char * ocspResponseBuffer;
		BYTEARRAY evidenceRecordArray[2];

		size_t filePos;
		const int DOC_COUNT = 10;
		DOCUMENT documents[DOC_COUNT];
		int dmsDocIDInt = 500;
	
		fileNames[ 0] = "document_01.pdf";
		fileNames[ 1] = "document_02.pdf";
		fileNames[ 2] = "document_03.pdf";
		fileNames[ 3] = "document_04.pdf";
		fileNames[ 4] = "document_05.pdf";
		fileNames[ 5] = "document_06.pdf";
		fileNames[ 6] = "document_07.pdf";
		fileNames[ 7] = "document_08.pdf";
		fileNames[ 8] = "document_09.pdf";
		fileNames[ 9] = "document_10.pdf";
		
		for (int vi=0; vi<DOC_COUNT; vi++)
		{
			// buffer for signed content
			contentBuffer = (unsigned char*)malloc(CONTENT_BUF_LEN);
			if (NULL == contentBuffer)
			{
				printf("No memory for content buffer\n");
				return -1;
			}

			// buffer for signature
			signatureBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
			if (NULL == signatureBuffer)
			{
				printf("No memory for signature buffer\n");
				return -1;
			}

			// buffer for new signature, done by the verifier (optional)
			sigAfterVerifyBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
			if (NULL == sigAfterVerifyBuffer)
			{
				printf("No memory for a buffer for the optional new signature of the verifier\n");
				return -1;
			}

			// buffer for supplied or returned OCSP response
			ocspResponseBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
			if (NULL == ocspResponseBuffer)
			{
				printf("No memory for OCSP response buffer\n");
				return -1;
			}
	
			// buffer for first supplied evidence record. This example uses 2 evidence records.
			evidenceRecordArray[0].data = (unsigned char*)malloc(ERS_BUF_LEN);
			if (NULL == evidenceRecordArray[0].data)
			{
				printf("No memory for first evidence record buffer\n");
				return -1;
			}
	
			// buffer for second supplied evidence record. This example uses 2 evidence records.
			evidenceRecordArray[1].data = (unsigned char*)malloc(ERS_BUF_LEN);
			if (NULL == evidenceRecordArray[1].data)
			{
				printf("No memory for second evidence record buffer\n");
				return -1;
			}
	
			// buffer for time stamp
			timestampBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
			if (NULL == timestampBuffer)
			{
				printf("No memory for timestamp response buffer\n");
				return -1;
			}
	
			// set document file name
			sprintf_s(docFileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s", documentsPath, fileNames[vi]);
			sprintf_s(sigFileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.pkcs7", documentsPath, fileNames[vi]);
			sprintf_s(timestampFileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.tsr", documentsPath, fileNames[vi]);
			sprintf_s(ocspRespFileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.ors", documentsPath, fileNames[vi]);
			sprintf_s(evidenceRec1FileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s-1.ers", documentsPath, fileNames[vi]);
			sprintf_s(evidenceRec2FileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s-2.ers", documentsPath, fileNames[vi]);

			// read document
			printf ("Reading %s\n", docFileNameWithPath);
			int fd;
			int openRet = _sopen_s(&fd, docFileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
			if (0 != openRet)
			{
				printf("Cannot open document %s\n", docFileNameWithPath);
				return -1;
			}

			filePos = 0;
			while (true)
			{
				int bytesRead = _read(fd, &contentBuffer[filePos], CONTENT_BUF_LEN-filePos);
			
				if (bytesRead < 0)
				{
					printf("Cannot read document %s\n", fileNameWithPath);
					return -1;
				}

				if (bytesRead == 0)
				{
					break;
				}

				filePos+=bytesRead;
			}
			
			_close(fd);

			documents[vi].dataToBeSigned = contentBuffer;
			documents[vi].dataToBeSignedLen = filePos;
			documents[vi].dataToBeSignedBufLen = 0; // signed content shall not be returned

			// read signature
			printf ("Reading %s\n", sigFileNameWithPath);
			openRet = _sopen_s(&fd, sigFileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
			if (0 != openRet)
			{
				printf("Cannot open signature %s\n", sigFileNameWithPath);
				return -1;
			}

			filePos = 0;
			while (true)
			{
				int bytesRead = _read(fd, &signatureBuffer[filePos], SIG_BUF_LEN-filePos);
			
				if (bytesRead < 0)
				{
					printf("Cannot read signature %s\n", sigFileNameWithPath);
					return -1;
				}

				if (bytesRead == 0)
				{
					break;
				}

				filePos+=bytesRead;
			}
			
			_close(fd);

			documents[vi].signature = signatureBuffer;
			documents[vi].signatureLen = filePos;


			// read supplied OCSP response
			printf ("Reading %s\n", ocspRespFileNameWithPath);
			openRet = _sopen_s(&fd, ocspRespFileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
			if (0 != openRet)
			{
				printf("Cannot open optional OCSP response %s. Calling SecSigner without it.\n", sigFileNameWithPath);

				// buffer is used for the OCSP response which SecSigner may return
				documents[vi].ocspResponseLen = 0;
			}
			else
			{
				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &ocspResponseBuffer[filePos], SIG_BUF_LEN-filePos);
			
					if (bytesRead < 0)
					{
						printf("Cannot read OCSP response %s\n", sigFileNameWithPath);
						return -1;
					}
	
					if (bytesRead == 0)
					{
						break;
					}

					filePos+=bytesRead;
				}
				
				_close(fd);

				documents[vi].ocspResponseLen = filePos; // buffer is used for the supplied OCSP response.
							// SecSigner may overwrite the buffer's content with a new OCSP response.
			}
			documents[vi].ocspResponse = ocspResponseBuffer;
			documents[vi].ocspResponseBufLen = SIG_BUF_LEN; 

			// evidence records for signatures using expired algorithms
			// read first evidence record
			int numberOfEvidenceRecords = 0;
			printf ("Reading %s\n", evidenceRec1FileNameWithPath);
			openRet = _sopen_s(&fd, evidenceRec1FileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
			if (0 != openRet)
			{
				printf("Cannot open optional first evidence record %s. Calling SecSigner without it.\n", evidenceRec1FileNameWithPath);
				documents[vi].evidenceRecordArray = NULL;
			}
			else
			{
				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &evidenceRecordArray[0].data[filePos], ERS_BUF_LEN-filePos);
				
					if (bytesRead < 0)
					{
						printf("Cannot read first evidence record %s\n", evidenceRec1FileNameWithPath);
						return -1;
					}
	
					if (bytesRead == 0)
					{
						break;
					}

					filePos+=bytesRead;
				}

				_close(fd);
				numberOfEvidenceRecords++;
				evidenceRecordArray[0].dataLen = filePos;

				// read second evidence record
				printf ("Reading %s\n", evidenceRec2FileNameWithPath);
				openRet = _sopen_s(&fd, evidenceRec2FileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
				if (0 != openRet)
				{
					printf("Cannot open optional second evidence record %s. Calling SecSigner without it.\n", evidenceRec2FileNameWithPath);
				}
				else
				{
					filePos = 0;
					while (true)
					{
						int bytesRead = _read(fd, &evidenceRecordArray[1].data[filePos], ERS_BUF_LEN-filePos);
				
						if (bytesRead < 0)
						{
							printf("Cannot read secons evidence record %s\n", evidenceRec2FileNameWithPath);
							return -1;
						}
	
						if (bytesRead == 0)
						{
							break;
						}

						filePos+=bytesRead;
					}

					_close(fd);
					numberOfEvidenceRecords++;
					evidenceRecordArray[1].dataLen = filePos;
				}

				documents[vi].evidenceRecordArray = evidenceRecordArray;
			}

			documents[vi].numberOfEvidenceRecords = numberOfEvidenceRecords; // number of supplied evidence records

			// read time stamp
			printf ("Reading %s\n", timestampFileNameWithPath);
			openRet = _sopen_s(&fd, timestampFileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
			if (0 != openRet)
			{
				printf("Cannot open optional time stamp %s. Calling SecSigner without it.\n", timestampFileNameWithPath);

				documents[vi].timeStamp = timestampBuffer;
				documents[vi].timeStampLen = 0;
			}
			else
			{
				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &timestampBuffer[filePos], SIG_BUF_LEN-filePos);
				
					if (bytesRead < 0)
					{
						printf("Cannot read timestamp %s\n", timestampFileNameWithPath);
						return -1;
					}
	
					if (bytesRead == 0)
					{
						break;
					}

					filePos+=bytesRead;
				}

				_close(fd);

				documents[vi].timeStampLen = filePos;
			}
			documents[vi].timeStamp = timestampBuffer;

			// fill document struct
			documents[vi].version = 13;
			documents[vi].documentType = SIGNDATATYPE_PLAINTEXT;
			documents[vi].signatureFormatType = SIGNATUREFORMATTYPE_PKCS7;
			documents[vi].mimeType = NULL;
			documents[vi].hashAlgorithm = NULL; // not used in verify mode

			documents[vi].oldSignature = sigAfterVerifyBuffer; // optional
			documents[vi].oldSignatureLen = SIG_BUF_LEN; // optional

			documents[vi].encryptedSig = NULL;  // not used in verify, supply signature in the 
												// signature-field, whether it's encrypted or not
			documents[vi].encryptedSigLen = 0;  // not used in verify mode

			documents[vi].setUseLegacyBmuXmlSigFormat = FALSE; // not used in verify mode
			documents[vi].xmlDSigNodePath = NULL; // not used in verify mode
			documents[vi].xmlDSigFilterPaths = NULL; // not used in verify mode
			documents[vi].numberOfXmlDSigFilterPaths = 0; // not used in verify mode
			documents[vi].xmlDSigNameSpaceName = NULL;   // not used in verify mode

			documents[vi].pdfAnnotation = NULL; // not used in verify mode

			documents[vi].documentFileName = fileNames[vi];

			documents[vi].encryptedDoc = NULL;
			documents[vi].encryptedDocLen = 0;
			documents[vi].verificationReport = NULL; // dont need the verification report
			documents[vi].verificationReportLen = 0;
		}

		// call SecSigner verification process
		printf ("Calling SecSigner verification\n");
		ret = verifyDocs(documents, DOC_COUNT);
		printf("verification return value =%d\n", ret);

		if (ret == OK)
		{
			for (int j=0; j<DOC_COUNT; j++)
			{
				// write the returned OCSP responses to files
				sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.ors", documentsPath, fileNames[j]);
	
				if (0 == documents[j].ocspResponseLen)
				{
					printf("Received no OCSP response for this document.\n");
				}
				else
				{
					printf ("Writing OCSP response %s\n", fileNameWithPath);
					int fd;
					int openRet = _sopen_s(&fd, fileNameWithPath, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
					if (0 != openRet)
					{
						printf("Cannot open OCSP response output file %s for writing.\n", fileNameWithPath);
						return -1;
					}
	
					_write(fd, documents[j].ocspResponse, documents[j].ocspResponseLen);
						
					_close(fd);
				}

				// write the returned optional new signature to files (if the verifier
				// has signed the document after verification)
				sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.new.pkcs7", documentsPath, fileNames[j]);
	
				if (0 == documents[j].oldSignatureLen)
				{
					// This is the normal case.
					printf("This document was not signed after verification.\n");
				}
				else
				{
					printf ("Writing signature of the verifier to %s\n", fileNameWithPath);
					int fd;
					int openRet = _sopen_s(&fd, fileNameWithPath, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
					if (0 != openRet)
					{
						printf("Cannot open new signature output file %s for writing.\n", fileNameWithPath);
						return -1;
					}
	
					_write(fd, documents[j].oldSignature, documents[j].oldSignatureLen);
						
					_close(fd);
				}

			}
		}

		for (int d=0; d<DOC_COUNT; d++)
		{
			// release the buffers
			if (documents[d].dataToBeSigned != NULL)
			{
				free (documents[d].dataToBeSigned);
				documents[d].dataToBeSigned = NULL;
			}

			if (documents[d].signature != NULL)
			{
				free (documents[d].signature);
				documents[d].signature = NULL;
			}

			if (documents[d].oldSignature != NULL)
			{
				free (documents[d].oldSignature);
				documents[d].oldSignature = NULL;
			}

			if (documents[d].ocspResponse != NULL)
			{
				free (documents[d].ocspResponse);
				documents[d].ocspResponse = NULL;
			}

			if (documents[d].encryptedSig != NULL)
			{
				free (documents[d].encryptedSig);
				documents[d].encryptedSig = NULL;
			}

			if (documents[d].timeStamp != NULL)
			{
				free (documents[d].timeStamp);
				documents[d].timeStamp = NULL;
			}
		}
	}
	else if (encryptGivenDocs)
	{
		// read test documents and pass them to SecSigner
		const int DOCCOUNT = 16;
		const int CIPHERCERTCOUNT = 2;
		char * fileNames[DOCCOUNT];
		char * certFileNames[CIPHERCERTCOUNT];
		char fileNameWithPath[FILE_NAME_WITH_PATH_LEN];
		unsigned char * contentBuffer;
		unsigned char * certBuffer;
		unsigned char * encrDocBuffer;
		size_t filePos;
		DOCUMENT documents[DOCCOUNT];
		BYTEARRAY cipherCerts[CIPHERCERTCOUNT];

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

		certFileNames[0] = "encrCert1.der";
		certFileNames[1] = "encrCert2.der";

		// encrypt twice, just to test the encryption call, of course once is enough in production
		for (int s=0; s<2; s++)
		{
			for (int i=0; i<DOCCOUNT; i++)
			{
				// set document file name
				sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s", documentsPath, fileNames[i]);
				printf ("Reading %s\n", fileNameWithPath);

				// buffer for to be signed content
				contentBuffer = (unsigned char*)malloc(CONTENT_BUF_LEN);
				if (NULL == contentBuffer)
				{
					printf("No memory for content buffer\n");
					return -1;
				}

				// buffer for returned encrypted document
				encrDocBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
				if (NULL == encrDocBuffer)
				{
					printf("No memory for encrypted document buffer\n");
					return -1;
				}
	
				// read test file
				int fd;
				int openRet = _sopen_s(&fd, fileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
				if (0 != openRet)
				{
					printf("Cannot open test data %s\n", fileNameWithPath);
					return -1;
				}

				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &contentBuffer[filePos], CONTENT_BUF_LEN-filePos);
			
					if (bytesRead < 0)
					{
						printf("Cannot read test data %s\n", fileNameWithPath);
						return -1;
					}

					if (bytesRead == 0)
					{
						break;
					}
	
					filePos+=bytesRead;
				}
			
				_close(fd);

				// fill document struct
				documents[i].version = 13;
				documents[i].documentFileName = fileNames[i]; // not used in encryption mode
				documents[i].dataToBeSigned = contentBuffer;
				documents[i].dataToBeSignedLen = filePos;
				documents[i].dataToBeSignedBufLen = 0; // not used in encryption mode
				documents[i].documentType = SIGNDATATYPE_PLAINTEXT; // not used in encryption mode
  				documents[i].signatureFormatType = NULL; // not used in encryption mode
				documents[i].mimeType = NULL; // not used in encryption mode
				documents[i].oldSignature = NULL; // not used in encryption mode
				documents[i].oldSignatureLen = 0; // not used in encryption mode
				documents[i].signature = NULL; // not used in encryption mode
				documents[i].signatureLen = 0; // not used in encryption mode
				documents[i].encryptedSig = NULL; // not used in encryption mode
				documents[i].encryptedSigLen = 0; // not used in encryption mode
				documents[i].hashAlgorithm = NULL; // not used in encryption mode
				documents[i].ocspResponse = NULL; // not used in encryption mode
				documents[i].ocspResponseLen = 0; // not used in encryption mode
				documents[i].ocspResponseBufLen = 0; // not used in encryption mode
				documents[i].evidenceRecordArray = NULL; // not used in encryption mode
				documents[i].numberOfEvidenceRecords = 0; // not used in encryption mode
				documents[i].timeStamp = NULL; // not used in encryption mode
				documents[i].timeStampLen = 0; // not used in encryption mode
				documents[i].signatureID = NULL; // not used in encryption mode
                documents[i].setUseLegacyBmuXmlSigFormat = FALSE; // not used in encryption mode
				documents[i].xmlDSigNodePath = NULL; // not used in encryption mode
				documents[i].xmlDSigFilterPaths = NULL; // not used in encryption mode
				documents[i].numberOfXmlDSigFilterPaths = 0; // not used in encryption mode
				documents[i].xmlDSigNameSpaceName = NULL;   // not used in encryption mode
				documents[i].pdfAnnotation = NULL; // not used in encryption mode
				documents[i].encryptedDoc = encrDocBuffer;
				documents[i].encryptedDocLen = SIG_BUF_LEN;
				documents[i].verificationReport = NULL; // dont need the verification report
				documents[i].verificationReportLen = 0;
			}

			for (int i=0; i<CIPHERCERTCOUNT; i++)
			{
				// set document file name
				sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s", documentsPath, certFileNames[i]);
				printf ("Reading encryption cert %s\n", fileNameWithPath);

				// buffer for to be certificate
				certBuffer = (unsigned char*)malloc(SIG_BUF_LEN);
				if (NULL == certBuffer)
				{
					printf("No memory for encryption certificate buffer\n");
					return -1;
				}

				// read test file
				int fd;
				int openRet = _sopen_s(&fd, fileNameWithPath, O_RDONLY | O_BINARY, _SH_DENYWR, S_IREAD);
				if (0 != openRet)
				{
					printf("Cannot open certificate %s\n", fileNameWithPath);
					return -1;
				}

				filePos = 0;
				while (true)
				{
					int bytesRead = _read(fd, &certBuffer[filePos], SIG_BUF_LEN-filePos);
			
					if (bytesRead < 0)
					{
						printf("Cannot read certificate %s\n", fileNameWithPath);
						return -1;
					}

					if (bytesRead == 0)
					{
						break;
					}
	
					filePos+=bytesRead;
				}
			
				_close(fd);

				// fill bytearray struct
				cipherCerts[i].data = certBuffer;
				cipherCerts[i].dataLen = filePos;
			}

			// call Signer encryption process
			printf ("Calling SecSigner encryption\n");
			ret = encryptDocs(documents, DOCCOUNT, cipherCerts, CIPHERCERTCOUNT);
			printf("encryption return value =%d\n", ret);

			if (ret == OK)
			{
				// write the returned encrypted documents to files
				for (int j=0; j<DOCCOUNT; j++)
				{
					if (documents[j].encryptedDocLen > 0)
					{
						sprintf_s(fileNameWithPath, FILE_NAME_WITH_PATH_LEN, "%s\\%s.encrypted", documentsPath, fileNames[j]);
	
						printf ("Writing encrypted doc %s\n", fileNameWithPath);
						int fd;
						int openRet = _sopen_s(&fd, fileNameWithPath, O_CREAT | O_WRONLY | O_BINARY, _SH_DENYRW, S_IREAD | _S_IWRITE);
						if (0 != openRet)
						{
							printf("Cannot open encrypted document output file %s for writing.\n", fileNameWithPath);
							return -1;
						}

						_write(fd, documents[j].encryptedDoc, documents[j].encryptedDocLen);
						_close(fd);
					}
				}
			}

			for (int d=0; d<DOCCOUNT; d++)
			{
				// release the buffers
				if (documents[d].dataToBeSigned != NULL)
				{
					free (documents[d].dataToBeSigned);
					documents[d].dataToBeSigned = NULL;
				}

				if (documents[d].encryptedDoc != NULL)
				{
					free (documents[d].encryptedDoc);
					documents[d].encryptedDoc = NULL;
				}
			}

			for (int i=0; i<CIPHERCERTCOUNT; i++)
			{
				// release the buffers
				free (cipherCerts[i].data);
				cipherCerts[i].data = NULL;
			}
		}
	}


	// close SecSecSigner
	printf ("Closing SecSigner\n");
	closeSecSigner();

	if (restart)
	{
		// init SecSigner again, maybe with different properties
		ret = initSecSigner(secSignerPropertyName, secSignerInstallPath);
		if (ret <0)
		{
			return ret;
		}

		// find smart card again
		ret = initSmartCard((BYTEARRAY*) NULL, (BYTEARRAY*) NULL, (BYTEARRAY*) NULL);
		if (ret <0)
		{
			return ret;
		}

		// close SecSigner again
		printf ("Closing SecSigner\n");
		closeSecSigner();
	}

	// Unload Java virtual machine -> This is the end, another load JVM will fail.
	printf("Unloading JavaVM\n");
	ret = unloadJavaVirtualMachine();
	if (ret <0)
	{
		return ret;
	}

	// unload DLL
	printf ("Unloading DLL\n");
    FreeLibrary(hSecSignerDLL);

	printf ("SecCommerce SecSigner test program ends.\n");
	return ret;
}
