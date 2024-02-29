SECSIGNER_DIR="`dirname "$0"`"
java -classpath "$SECSIGNER_DIR"/SecSigner.jar:"$SECSIGNER_DIR"/SecSignerExt.jar seccommerce.secsigner.ext.SecSignerMain $1

