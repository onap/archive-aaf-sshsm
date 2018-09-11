#!/bin/bash

# CA key import script for both tpm and softhsm case
# required inputs are passed in through mount volume by oom

set -e

#if pkcs11 is not configured exit gracefully
str=$(awk '/org.osaaf.aaf.cm.pkcs11/{print $0}' /opt/app/osaaf/local/org.osaaf.cm.ca.props)
if [ ! -z $str ];then
   echo "Using pkcs11 configuration"
else
   echo "Not using pkcs11 configuration"
   exit 0
fi


#mount path for input files
mountvolume="/tmp/files"
#key_id in decimal
key_id="8738"
#Key_label used for key import
key_label="localca"
#Initial slot number
slot_no="0"
#Token name
token_name="CAToken"
#cert_id is the input for the application which is hexadecimal equivalent of key_id
cert_id=$(printf '%x' ${key_id})
#SoftHSM2 lib location
applicationlibrary="/usr/local/lib/softhsm/libsofthsm2.so"
#User pin for the SoftHSM operations
cd /
phrase="$(cat ${mountvolume}/passphrase-pin)"
cp ${mountvolume}/upin.txt.gpg .
echo "${phrase}" | gpg --batch --yes --passphrase-fd 0 upin.txt.gpg
upin="$(cat upin.txt)"
rm -f upin.txt
rm -f upin.txt.gpg
#SoPin for the SoftHSM operations
cp ${mountvolume}/sopin.txt.gpg .
echo "${phrase}" | gpg --batch --yes --passphrase-fd 0 sopin.txt.gpg
sopin="$(cat sopin.txt)"
rm -f sopin.txt
rm -f sopin.txt.gpg

# Initialize the token and set the perms on the .sh files
softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" \
    --pin ${upin} --so-pin ${sopin}
softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
SoftHSMv2SlotID="$(cat slotinfo.txt)"
rm -rf slotinfo.txt

chmod 755 softhsmconfig.sh
chmod 755 application.sh

# import the key either to tpm or softhsm
if [ -f ${mountvolume}/out_parent_public ]; then

    #tpm import password
    phrase="$(cat ${mountvolume}/passphrase-ipass)"
    cp ${mountvolume}/ipass.txt.gpg .
    echo "${phrase}" | gpg --batch --yes --passphrase-fd 0 ipass.txt.gpg
    ipass="$(cat ipass.txt)"
    rm -f ipass.txt
    #TPM handle
    tpm_handle="$(cat ${mountvolume}/tpm-handle.txt)"
    # Copy the required input files for the Import tool
    cp ${mountvolume}/dup* /tpm-util/bin/

    # Run the Import Utility
    cd /tpm-util/bin
    ./ossl_tpm_import -H $tpm_handle -dupPub dupPub -dupPriv dupPriv \
    -dupSymSeed dupSymseed -dupEncKey dupEncKey -pub outPub -priv outPriv \
    -password $ipass

    # setup tpm-softhsm specific config
    cd /
    ./softhsmconfig.sh $tpm_handle $key_id $key_label $upin $sopin $SoftHSMv2SlotID \
    "/tpm-util/bin/outPriv" "/tpm-util/bin/outPub"
else

# SoftHSM mode implementation

    echo "TPM hardware unavailable, using SoftHSM implementation"

    # Extract the Private key using passphrase
    passphrase="$(cat ${mountvolume}/passphrase)"
    cp ${mountvolume}/privkey.pem.gpg .
    echo "${passphrase}" | gpg --batch --yes --passphrase-fd 0 privkey.pem.gpg

    # Convert the Private key pem into der format
    openssl rsa -in ./privkey.pem -outform DER -out privatekey.der

    # Load the Private key into SoftHSM
    pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
    --write-object ./privatekey.der --type privkey --id ${cert_id} --label ${key_label}

    # Clenup the files
    rm -f privkey.pem
    rm -f privatekey.der
    rm -f privkey.pem.gpg

fi


# Convert the crt to der format
openssl x509 -in ${mountvolume}/ca.cert -outform der -out ca.der

# Add the ca certificate
pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} \
--write-object ./ca.der --type cert --id ${cert_id}

rm -f ca.der

# Call app specific script
./application.sh $applicationlibrary $SoftHSMv2SlotID
