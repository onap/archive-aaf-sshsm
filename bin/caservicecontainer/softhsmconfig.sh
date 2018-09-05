#!/bin/bash

# This script will take six parameters as input
tpm_handle=$1
key_id=$2
key_label=$3
upin=$4
sopin=$5
SoftHSMv2SlotID=$6

# export Pluginlibrary's location
pluginlibrary="/usr/local/lib/libtpm2-plugin.so"

SSHSM_HW_PLUGINS_PARENT_DIR="/tmp/hwparent"
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}
echo "The newly assigned plugin directory is ${SSHSM_HW_PLUGINS_PARENT_DIR}"

# Configuration generation for SoftHSM
# 1.a Create the directory as expected by the SoftHSM to read the files
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate
mkdir -p ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01

# 1.b Copy the Plugin library and create the required Configuration
cp ${pluginlibrary} ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/plugin.so
touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1
echo "$tpm_handle" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/activate/Afile1.id1

# 1.c Generate the pkcs11.cfg file required for the SoftHSM operations
touch ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
chmod 755 ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "key_id:${key_id}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "key_label:${key_label}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "upin:${upin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "sopin:${sopin}" >> ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg
echo "slot:${SoftHSMv2SlotID}" >>  ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/pkcs11.cfg

# 1.d Copy the output of Import utility into the directory where SoftHSMv2 expects
cp /tpm-util/bin/outPriv ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.priv
cp /tpm-util/bin/outPub ${SSHSM_HW_PLUGINS_PARENT_DIR}/S01tpm/key01/Kfile1.pub
