#!/bin/sh

# minimum memory
MINMEM=64m
# maximum memory
MAXMEM=256m
# set classpath
CLASSPATH=$CLASSPATH:SecSigner.jar:SecSignerExt.jar:.
MAINCLASS="seccommerce.secsigner.ext.SecSignerMain"


java -version

echo "current directory: ${PWD}\n" #${PWD##*/}

java -classpath $CLASSPATH -ms$MINMEM -mx$MAXMEM $MAINCLASS $1

