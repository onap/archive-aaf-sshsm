
#!/bin/bash
set -e

cert_id="8738"
tpm_handle="0x81000011"
key_label="Key1"
upin="123456789"
sopin="123456789"
applicationlibrary="/usr/local/lib/libtpm2-plugin.so"
key_id=$(echo $(( 16#{cert_id} )))

mkdir -p /var/run/dbus

stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &
SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"

# 1. Run Init_and_create_tpm_primary.sh script from /sshsm/tpm-util/initandverify directory
#    from Tabrmd (privileged) container

# 2. Create the directory as expected by the SoftHSM to read the files and
#    Copy the required files (keys and certificates) to be imported into TPM
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01

# 3. Copy the tpm Plugin's so file and the input for the Import tool

cp ${applicationlibrary} ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/plugin.so
cp /tmp/files/inittoolfiles/* /sshsm/tpm-util/initandverify/
#cp /tmp/files/duplicatetoolfiles/* /sshsm/tpm-util/initandverify/

# 4. Generate the TPM Primary key handle's file and give the required permission and update the file

touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1

echo "$tpm_handle" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1

# 5. Generate the pkcs11.cfg file required for the SoftHSM opeations

touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
# 5.a Create the config file for the application as well
touch /tmp/files/applicationfiles/cfg.pkcs11
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
chmod 755 /tmp/files/applicationfiles/cfg.pkcs11

echo "${key_id}"
echo "key_id:${key_id}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg

echo "${key_label}"
echo "key_label:${key_label}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "name = ${key_label}" >> /tmp/files/applicationfiles/cfg.pkcs11

echo "upin:${upin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin:${sopin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin is ${sopin}"

echo "${applicationlibrary}"
echo "library = ${applicationlibrary}" >> /tmp/files/applicationfiles/cfg.pkcs11

# 6. NOTE - Make sure all the files are present on the mounted folder

# 7. Initialize the token
softhsm2-util --init-token --slot 0 --label "Token1" --pin ${upin} --so-pin ${sopin}

softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt

SoftHSMv2SlotID="$(cat slotinfo.txt)"

echo "${SoftHSMv2SlotID}"

# 8. Update the configuration files of SoftHSMv2 and the Application

echo "slot:${SoftHSMv2SlotID}" >>  ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
#sed -i  '1i'slot:${SoftHSMv2SlotID}'' ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "slot = ${SoftHSMv2SlotID}" >> /tmp/cfg.pkcs11
#sed -i  '1i'slot = ${SoftHSMv2SlotID}'' /tmp/cfg.pkcs11


# 9. Run the Import utility (Assuming the Init Script and Duplicate is already run and the output is available in
#    shared volume)
/sshsm/tpm-util/initandverify/ImportTpmKey.sh

# 9.a. copy the output of the Import utility to the importtool directory (Shared volume) and the SoftHSMv2 files directory

cp /sshsm/tpm-util/initandverify/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv

cp /sshsm/tpm-util/initandverify/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub

# 10. Compile the Application
cd /tmp/files/applicationfiles
javac CaSign.java

# 11. Convert the crt to der format
openssl x509 -in ca.crt -outform der -out ca.der

# 12. Add the ca certificate
sudo pkcs11-tool --module /usr/local/lib/softhsm2.so -l --pin ${upin} --write-object ./ca.der --type cert --id ${certid}

# 13. Run the Application

sudo java CaSign ${upin} 0x${certid}

#NOTE - This scripts expects the Init and the Duplicate tools to be already run and the output files to be present at the
# shared volume (input for Import tool)
