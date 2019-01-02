#!/bin/bash

# NOTE - This scripts expects the Init and the Duplicate tools to be already
# run and the output files(listed in README) to be present at the
# shared volume (input for Import tool)
# It also requires the following ENVIRONMENT variables to be set
# SECRETS_FOLDER - containing the srk_handl and prk_passwd files in base64
# DATA_FOLDER - containing the files that are produced from the distcenter

set -e

#Primary Key Password used by TPM Plugin to load keys
export TPM_PRK_PASSWORD="$(cat ${SECRETS_FOLDER}/prk_passwd | base64 -d)"
#Handle to the aforementioned Primary Key
SRK_HANDLE="$(cat ${SECRETS_FOLDER}/srk_handle | base64 -d)"
#key_id is the parameter expected by SoftHSM
key_id="8738"
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
#Set working dir
WORKDIR=$PWD
#Process count for the Stress test
count="15"

# 1.Initialize the token/
    softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" \
    --pin ${upin} --so-pin ${sopin}
    softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
    SoftHSMv2SlotID="$(cat slotinfo.txt)"
    echo "The slot ID used is ${SoftHSMv2SlotID}"

# 2.Plugin directory for the SoftHSM to load plugin and for further operations
if [ -f ${DATA_FOLDER}/out_parent_public ]; then

    # 2.a Copy the required input files for the Import tool
    cp ${DATA_FOLDER}/dup* /tpm-util/bin/

    # 2.b Run the Import Utility
    cd /tpm-util/bin
    ./ossl_tpm_import -H $SRK_HANDLE -dupPub dupPub -dupPriv dupPriv \
    -dupSymSeed dupSymseed -dupEncKey dupEncKey -pub outPub -priv outPriv \
    -password $TPM_PRK_PASSWORD

    cd $WORKDIR
    ./softhsmconfig.sh $SRK_HANDLE $key_id $key_label $upin $sopin $SoftHSMv2SlotID
else

# 3 SoftHSM mode implementation

    echo "TPM hardware unavailable. Using SoftHSM implementation"

    cd ${DATA_FOLDER}

    # 3.a Extract the Private key using passphrase
    cat passphrase | gpg --batch --yes --passphrase-fd 0 privkey.pem.gpg

    # 3.b Convert the Private key pem into der format
    openssl rsa -in ./privkey.pem -outform DER -out privatekey.der

    # 3.c Load the Private key into SoftHSM
    pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
    --write-object ./privatekey.der --type privkey --id ${cert_id} --label ${key_label}

fi

# 3.a Application operation
cd ${DATA_FOLDER}

# 3.b Convert the crt to der format
openssl x509 -in ca.cert -outform der -out ca.der
cp ca.cert /testca/tests/multithread/

# 3.c Add the ca certificate
pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
--write-object ./ca.der --type cert --id ${cert_id}

# 4. Calling the functionalities of the sample application
cd $WORKDIR
./multithreadapplication.sh $key_label $SoftHSMv2SlotID $upin $cert_id

# 5. Cleanup
cd $WORKDIR
rm -rf slotinfo.txt
