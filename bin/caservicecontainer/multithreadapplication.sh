#!/bin/bash

# This script takes 4 arguments
key_label=$1
SoftHSMv2SlotID=$2
upin=$3
cert_id=$4

# Location to fecth SoftHSM library required by application
applicationlibrary="/usr/local/lib/softhsm/libsofthsm2.so"

# Setting up the java application and running the application
# 1. Create the configuration pkcs11.cfg for the application
# Remove any existing cfg file first from the CWD
rm -f pkcs11.cfg
touch pkcs11.cfg
chmod 755 pkcs11.cfg
echo "name = ${key_label}" >> pkcs11.cfg
echo "The location of applicationms library is ${applicationlibrary}"
echo "library = ${applicationlibrary}" >> pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> pkcs11.cfg

# 2. Compile the Application
# CaSign requires test.csr to be available in CWD
javac CaSignMultiThread.java

# 3. Run the Application
java CaSignMultiThread ${upin} 0x${cert_id}
