#!/bin/bash

# NOTE - This scripts expects the Init and the Duplicate tools to be already
# run and the output files(listedb in README) to be present at the
# shared volume (input for Import tool)

set -e

#Placeholder of Input files to the Import tool which is the output of duplicate tool
sharedvolume="/tmp/files"
#key_id is the parameter expected by SoftHSM
key_id="8738"
#TPM handle
tpm_handle="0x81000011"
#Key_label is the  parameter expected by SoftHSM
key_label="ABC"
#UserPin for the SoftHSM operations
upin="123456789"
#SoPin for the SoftHSM operations
sopin="123456789"
#Slot number for the SoftHSM operations. Initially it should be started with 0
slot_no="0"
#Name for creating the slot used by SoftHSM
token_no="Token1"
#cert_id is the input for the application which is hexadecimal equivalent of key_id
cert_id=$(printf '%x' ${key_id})

# 1.Initialize the token/
    softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" \
    --pin ${upin} --so-pin ${sopin}
    softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
    SoftHSMv2SlotID="$(cat slotinfo.txt)"
    echo "${SoftHSMv2SlotID}"

# 2.Plugin directory for the SoftHSM to load plugin and for further operations
if [ -f ${sharedvolume}/out_parent_public ]; then

    mkdir -p /var/run/dbus
    # 2.a Copy the required input files for the Import tool
    cp ${sharedvolume}/dup* /sshsm/tpm-util/initandverify/

    # 2.b Run the Import Utility
    cd /sshsm/tpm-util/initandverify
    ../import/ossl_tpm_import -H $tpm_handle -dupPub dupPub -dupPriv dupPriv \
-dupSymSeed dupSymseed -dupEncKey dupEncKey -pub outPub -priv outPriv

    cd /
    chmod 755 softhsmconfig.sh
    (
    export tpm_handle key_id key_label upin sopin SoftHSMv2SlotID
    source ./softhsmconfig.sh
    )
else

# 3 SoftHSM mode implementation

    echo "There is no Plugin directory assigned.Hence SoftHSM implementation"

    cd ${sharedvolume}

    # 3.a Extract the Private key using passphrase
    passphrase="$(cat passphrase)"
    echo "${passphrase}"
    echo "${passphrase}" | gpg --batch --yes --passphrase-fd 0 privkey.pem.gpg

    # 3.b Convert the Private key pem into der format
    openssl rsa -in ./privkey.pem -outform DER -out privatekey.der

    # 3.c Load the Private key into SoftHSM
    pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
    --write-object ./privatekey.der --type privkey --id ${cert_id} --label ${key_label}

fi

# 3.a Application operation
cd ${sharedvolume}

# 3.b Convert the crt to der format
openssl x509 -in ca.cert -outform der -out ca.der

# 3.c Add the ca certificate
pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
--write-object ./ca.der --type cert --id ${cert_id}

# 4. Calling the functionalities of the sample application
cd /
chmod 755 application.sh
(
export key_label SoftHSMv2SlotID upin cert_id
source ./application.sh key_label SoftHSMv2SlotID upin cert_id
)
