// $Id: secerror.h,v 1.10 2015/05/15 12:45:32 titus Exp $
// $Source: /encfs/checkout/antonio/cvs/SecCommerceDev/seccommerce/c/common/secerror.h,v $

/**
 * Error codes returned SecCommerce DLLs.
 *
 * @version $Id: secerror.h,v 1.10 2015/05/15 12:45:32 titus Exp $
 * @author SecCommerce Informationssysteme GmbH, Hamburg
 */

#ifdef OK
#undef OK
#endif
 
// returned status codes, in case of an error call getErrorMessage() for an additional text message
#define OK			  (0)  // success
#define STARTED			  1  // request started, but not finished yet. Only possible if modal=false
#define SIGNATURE_INVALID	 -1  // one or more signatures are invalid
#define BUFFER_TOO_SHORT	 -2  // supplied buffer is too short
#define METHOD_FAILED		 -3  // JVM could not execute a Java method
#define METHOD_NOT_FOUND	 -4  // JVM could not find a Java method
#define NO_MEMORY		 -5  // not enough memory for a temporary buffer
#define NOT_INITED		 -6  // init() was not called yet
#define JVM_NOT_AVAILABLE	 -7  // JVM is not available
#define MISSING_PARAMETER	 -8  // a parameter is NULL, but it must have a value
#define CLASS_NOT_FOUND		 -9  // JVM can not find a Java class
#define VERSION_MISMATCH	-10  // version of a struct does not match
#define DOC_MISMATCH            -11  // stopped by user because of a discrepancy between the scanned image and the original document
#define CANCELED                -12  // user canceled the process
#define THREAD_ERROR            -13  // a thread could not be started or its exit code could not be retrieved
#define SIG_HASH_COLLISION      -14  // The signature hash cannot be recorded, because it already exists in the hash archive.
#define SIG_OVER_HASH_FAILED    -15  // The parent level overhash could not be built and therefore the signature hash could not be stored.
#define COMMUNICATION_ERROR     -16  // The request could not be transfered to SecPKI.
#define DB_INCONSISTENT         -17  // The data base scheme does not match SecPKI's version or a data base operation failed.
#define MISSINGREQOBJECT        -18  // A parameter for the request is missing.
#define ORG_NOT_FOUND           -19  // The specified organisation was not found in SecPKI's data base.
#define ORGID_MISSING           -20  // An organisation name was not specified.
#define CERT_NOT_PARSABLE       -21  // The certificate could not be read. The format may be invalid.
#define CERT_NOT_VALID          -22  // The certificate is not valid. No trusted issuer certificate is installed in
                                     // SecPKI or the certificate was revoked or not actived yet.
#define ACCOUNT_NOTFOUND        -23  // No account in SecPKI's data base is connected to the supplied certificate.
#define ACCOUNT_LOCKED          -24  // The account of the certificate's owner in SecPKI is locked.
#define NO_SIGN_PERMISSION      -25  // The certificate's owner has no permission to sign in SecPKI.
#define SIGNEDDATA_UNREADABLE   -26  // The signature (SignedData) is not readable. The format may be invalid.
#define SMARTCARD_REMOVED       -27  // The smart card was removed. Login has to be called again before the next signature.
#define TOO_FEW_DATA_RETURNED   -28  // A Java method did not return enough values.
#define DOC_HAS_NO_SIGNATURE    -29  // For instance if a pdf document does not contain a signature
