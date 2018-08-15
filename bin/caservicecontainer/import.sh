#!/bin/bash

# NOTE - This scripts expects the Init and the Duplicate tools to be already
# run and the output files(listedb in README) to be present at the
# shared volume (input for Import tool)

set -e

#Placeholder of Input files to the Import tool which is the output of duplicate tool
export sharedvolume="/tmp/files"
#Pluginlibrary
export pluginlibrary="/usr/local/lib/libtpm2-plugin.so"
#key_id is the parameter expected by SoftHSM
export key_id="8738"
#TPM handle
export tpm_handle="0x81000011"
#Key_label is the  parameter expected by SoftHSM
export key_label="ABC"
#UserPin for the SoftHSM operations
export upin="123456789"
#SoPin for the SoftHSM operations
export sopin="123456789"
#Slot number for the SoftHSM operations. Initially it should be started with 0
export slot_no="0"
#Name for creating the slot used by SoftHSM
export token_no="Token1"
#Location for the application to fecth SoftHSM library
export applicationlibrary="/usr/local/lib/softhsm/libsofthsm2.so"
#cert_id is the input for the application which is hexadecimal equivalent of key_id
export cert_id=$(printf '%x' ${key_id})

# 1.Initialize the token/
    softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" --pin ${upin} --so-pin ${sopin}
    softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
    export SoftHSMv2SlotID="$(cat slotinfo.txt)"
    echo "${SoftHSMv2SlotID}"

# 2.Plugin directory for the SoftHSM to load plugin and for further operations
if [ ${SSHSM_HW_PLUGINS_PARENT_DIR} -a -f ${sharedvolume}/out_parent_public ]; then

    # 2.a Plugin Implementation
    echo "Plugin directory available. Hence creating the required directory"
    export SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}
    echo "The newly assigned plugin directory is ${SSHSM_HW_PLUGINS_PARENT_DIR}"
    chmod 755 softhsmconfig.sh
    ./softhsmconfig.sh
    
    mkdir -p /var/run/dbus

    # 2.b Copy the required input files for the Import tool
    cp ${sharedvolume}/* /sshsm/tpm-util/initandverify

    # 2.c Run the Import Utility
    cd /sshsm/tpm-util/initandverify
    ./ImportTpmKey.sh

    # 2.d Copy the output of the Import utility to the directory that SoftHSMv2 expects
    cp /sshsm/tpm-util/initandverify/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv
    cp /sshsm/tpm-util/initandverify/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub

else
    # 3 SoftHSM mode implementation

    echo "There is no Plugin directory assigned. Hence Falling
          back to SoftHSM implementation"

    cd ${sharedvolume}

    # 3.a Extract the Private key using passphrase
    passphrase="$(cat passphrase)"
    echo "${passphrase}"
    echo "${passphrase}" | gpg --batch --yes --passphrase-fd 0 privkey.pem.gpg

    # 3.b Convert the Private key pem into der format
    openssl rsa -in ./privkey.pem -outform DER -out privatekey.der

    # 3.c Load the Private key into SoftHSM
    pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin}
--write-object ./privatekey.der --type privkey --id ${cert_id} --label ${key_label}

fi

# 4. Application operation
cd ${sharedvolume}

# 3.a Convert the crt to der format
openssl x509 -in ca.cert -outform der -out ca.der

# 3.b Add the ca certificate
pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} --write-object ./ca.der --type cert --id ${cert_id}

# 4. Calling the functionalities of the sample application
cd /
chmod 755 application.sh
./application.sh
