#!/bin/bash

#Location to fecth SoftHSM library required by application
export applicationlibrary="/usr/local/lib/softhsm/libsofthsm2.so"

# Setting up the java application and running the application
# 1. Create the configuration pkcs11.cfg for the application
touch /tmp/pkcs11.cfg
chmod 755 /tmp/pkcs11.cfg
echo "name = ${key_label}" >> /tmp/pkcs11.cfg
echo "${applicationlibrary}"
echo "library = ${applicationlibrary}" >> /tmp/pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> /tmp/pkcs11.cfg

# 2. Compile the Application
cd /tmp/files
cp test.csr /tmp/test.csr
javac CaSign.java

# 3. Run the Application
java CaSign ${upin} 0x${cert_id}

# 4. Verify the generated certificate
openssl verify -verbose -CAfile ca.cert /tmp/test.cert

# 5. Cleanup
cd /
rm -rf slotinfo.txt
