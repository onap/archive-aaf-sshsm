
#NOTE - This scripts expects the Init and the Duplicate tools to be already run and the output files to be present at the
# shared volume (input for Import tool)

#!/bin/bash
set -e

duplicatetooldir="/tmp/files/duplicatetool/"
cert_id="8738"
tpm_handle="0x81000011"
key_label="Key1"
upin="123456789"
sopin="123456789"
applicationlibrary="/usr/local/lib/libtpm2-plugin.so"
key_id=$(echo $(( 16#{cert_id} )))
slot_no="0"
token_no="Token1"
SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"

mkdir -p /var/run/dbus

stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# Initialize the token
softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" --pin ${upin} --so-pin ${sopin}
softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
SoftHSMv2SlotID="$(cat slotinfo.txt)"
echo "${SoftHSMv2SlotID}"

# 1. Create the directory as expected by the SoftHSM to read the files and
#    Copy the required files (keys and certificates) to be imported into TPM
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01

# 2. Copy the tpm Plugin's so file and the input for the Import tool
cp ${applicationlibrary} ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/plugin.so
touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
echo "$tpm_handle" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
# 3. Generate the pkcs11.cfg file required for the SoftHSM opeations

touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "${key_id}"
echo "key_id:${key_id}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "${key_label}"
echo "key_label:${key_label}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "upin:${upin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin:${sopin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin is ${sopin}"

# 4. Update the configuration files of SoftHSMv2 configuration files
echo "slot:${SoftHSMv2SlotID}" >>  ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> /tmp/pkcs11.cfg

# add the environment variable for the duplicate tool directory
cp ${duplicatetooldir}/ /sshsm/tpm-util/initandverify/
/sshsm/tpm-util/initandverify/ImportTpmKey.sh

# 5. Copy the output of the Import utility to the importtool directory (Shared volume) and the SoftHSMv2 files directory
cp /sshsm/tpm-util/initandverify/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv
cp /sshsm/tpm-util/initandverify/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub

# Setting up the java application and running the application
# Create the configuration pkcs11.cfg for the application
touch /tmp/pkcs11.cfg
chmod 755 /tmp/pkcs11.cfg
echo "name = ${key_label}" >> /tmp/pkcs11.cfg
echo "${applicationlibrary}"
echo "library = ${applicationlibrary}" >> /tmp/pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> /tmp/pkcs11.cfg

# 6. Compile the Application
cd /tmp
javac CaSign.java

# 7. Convert the crt to der format
openssl x509 -in ca.crt -outform der -out ca.der
# 8. Add the ca certificate
sudo pkcs11-tool --module /usr/local/lib/softhsm2.so -l --pin ${upin} --write-object ./ca.der --type cert --id ${certid}

# 9. Run the Application
sudo java CaSign ${upin} 0x${certid}
