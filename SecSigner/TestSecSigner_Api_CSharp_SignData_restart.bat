@echo off
REM $Id: TestSecSigner_Api_CSharp_SignData_restart.bat,v 1.5 2015/05/22 15:59:23 tk Exp $
REM $Source: /encfs/checkout/antonio/cvs/SecCommerceDev/seccommerce/secsignersigg/dll/TestCallSecSignerDLLCSharp/developer/TestSecSigner_Api_CSharp_SignData_restart.bat,v $
REM
REM This batch invokes the signature of some test files
REM located at D:\SecSigner\test
REM 
REM Params:
REM 1: Testmode 
REM         2: test signature of given documents
REM         3: test verification of given documents)
REM         4: test signature of given documents and restart SecSigner
REM 2: Name of configuration file, usually "secsigner.properties"
REM 3: Name of the installation path, usually "D:\SecSigner\"
REM 4: Mamimum memory in MB for the Java VM
REM 5: Name of the directory where some test documents and signatures are taken from
REM
echo Info: Oracle Java Runtime version 7 is required, currently used:
java -fullversion
TestCallSecSignerDLLCSharp.exe 4 secsigner.properties D:\SecSigner\ 1024 D:\SecSigner\test\
pause
