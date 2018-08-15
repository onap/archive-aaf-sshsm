#NOTE - This scripts expects the Init and the Duplicate tools to be already run and the output files to be present at the
# shared volume (input for Import tool)

#!/bin/bash
set -e

#Placeholder of Input files to the Import tool which is the output of duplicate tool
export duplicatetooldir="/tmp/files"
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

# Plugin directory for the SoftHSM to load plugin and for further operations
if [ ${SSHSM_HW_PLUGINS_PARENT_DIR} -a -f ${duplicatetooldir}/out_parent_public ]; then

    # 1.a Plugin Implementation
    echo "Plugin directory available. Hence creating the required directory"
    export SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}
    echo "The newly assigned plugin directory is ${SSHSM_HW_PLUGINS_PARENT_DIR}"

    mkdir -p /var/run/dbus

    # 1.b  Initialize the token
    softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" --pin ${upin} --so-pin ${sopin}
    softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
    export SoftHSMv2SlotID="$(cat slotinfo.txt)"
    echo "${SoftHSMv2SlotID}"

    # 1.c Create the directory as expected by the SoftHSM to read the files
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate
    mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01

    cp ${pluginlibrary} ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/plugin.so
    touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
    chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
    echo "$tpm_handle" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1

    # 1.d Generate the pkcs11.cfg file required for the SoftHSM operations
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

    # 1.e Copy the required input files for the Import tool
    cp ${duplicatetooldir}/* /sshsm/tpm-util/initandverify

    # 1.f Run the Import Utility
    cd /sshsm/tpm-util/initandverify
    ./ImportTpmKey.sh

    # 1.g Copy the output of the Import utility to the directory that SoftHSMv2 expects
    cp /sshsm/tpm-util/initandverify/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv
    cp /sshsm/tpm-util/initandverify/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub

else

    # 2.a SoftHSM mode implementation

    echo "There is no Plugin directory assigned. Hence Falling
          back to SoftHSM implementation"

    cd /tmp/files

    # 2.b Convert the Private key pem into der format
    openssl rsa -in ./privkey.pem -outform DER -out privatekey.der

    softhsm2-util --init-token --slot ${slot_no} --label "${token_name}" --pin ${upin} --so-pin ${sopin}
    softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt
    export SoftHSMv2SlotID="$(cat slotinfo.txt)"
    echo "${SoftHSMv2SlotID}"

    # 2.c Load the Private key into SoftHSM
    pkcs11-tool --module /usr/local/lib/softhsm/libsofthsm2.so -l --pin ${upin}
--write-object ./privatekey.der --type privkey --id ${cert_id} --label ${key_label}

fi

# 3. Calling the functionalities of the sample application
cd /
chmod 755 application.sh
./application.sh
