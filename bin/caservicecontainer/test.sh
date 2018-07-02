
#NOTE - This scripts expects the Init and the Duplicate tools to be already run and the output files to be present at the
# shared volume (input for Import tool)

#!/bin/bash
set -e

#Placeholder of Input files to the Import tool which is the output of duplicate tool
duplicatetooldir="/tmp/files/duplicatetoolfiles"
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
#Location for the application to fecth SoftHSM library
applicationlibrary="/usr/local/lib/softhsm/libsofthsm2.so"
#cert_id is the input for the application which is hexadecimal equivalent of key_id
cert_id=$(printf '%x' ${key_id})

#Plugin directory for the SoftHSM to load plugin and for further operations
if [ ! "$SSHSM_HW_PLUGINS_PARENT_DIR" ]
then
    echo "There is no Plugin directory assigned. Hence creating the directory required by SoftHSM"
    SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"
fi
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}
echo "The newly assigned plugin directory is ${SSHSM_HW_PLUGINS_PARENT_DIR}"

mkdir -p /var/run/dbus

stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# Initialize the token
softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" --pin ${upin} --so-pin ${sopin}
softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
SoftHSMv2SlotID="$(cat slotinfo.txt)"
echo "${SoftHSMv2SlotID}"

# 1. Create the directory as expected by the SoftHSM to read the files
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01

cp ${applicationlibrary} ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/plugin.so
touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
echo "$tpm_handle" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1

# 2. Generate the pkcs11.cfg file required for the SoftHSM opeations
touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "${key_id}"
echo "key_id:${key_id}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "${key_label}"
echo "key_label:${key_label}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "upin:${upin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin:${sopin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin is ${sopin}"
echo "slot:${SoftHSMv2SlotID}" >>  ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg

# 3.a Copy the required input files for the Import tool
cp ${duplicatetooldir}/* /sshsm/tpm-util/initandverify

# 3.b Run the Import Utility
cd /sshsm/tpm-util/initandverify
./ImportTpmKey.sh

# 3.c Copy the output of the Import utility to the directory that SoftHSMv2 expects
cp /sshsm/tpm-util/initandverify/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv
cp /sshsm/tpm-util/initandverify/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub

# Setting up the java application and running the application
# 4. Create the configuration pkcs11.cfg for the application
touch /tmp/pkcs11.cfg
chmod 755 /tmp/pkcs11.cfg
echo "name = ${key_label}" >> /tmp/pkcs11.cfg
echo "${applicationlibrary}"
echo "library = ${applicationlibrary}" >> /tmp/pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> /tmp/pkcs11.cfg

# 5. Compile the Application
cd /tmp/files/applicationfiles
javac CaSign.java

# 6. Convert the crt to der format
openssl x509 -in ca.cert -outform der -out ca.der

# 7. Add the ca certificate
pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin} --write-object ./ca.der --type cert --id ${cert_id}

# 8. Run the Application
java CaSign ${upin} 0x${cert_id}

# 9. Verify the generated certificate
openssl verify -verbose -CAfile ca.cert /tmp/test.cert
