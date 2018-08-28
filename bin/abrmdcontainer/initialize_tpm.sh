#!/bin/sh

# 1. Create the directory with the host's name to store the output of Init tool
# This is demarcate the generated files which are specific to this host's TPM
mkdir -p ${ABRMD_DATA}/host_${TPM_NODE_NAME}
WORKDIR=${ABRMD_DATA}/host_${TPM_NODE_NAME}
cd ${WORKDIR}
# /abrmd/cred will contain srk_handle and tpm owner hierarchy password
# provided by the admin of the TPM node
SRKHANDLE="$(cat /abrmd/cred/srk_handle | base64 -d)"

# 2. Create initial Flag values which are reset upon failure
error="NULL"
flag="1"

# 3. TPM Startup
echo "tpm2_startup --clear -T device --verbose"
tpm2_startup --clear -T device -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "TPM Startup failed"); flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""

# 4. Read public portion of TPM primary key with stored handle
#    It is expected that the Admin would have created this already
#    using the create_primary.sh script
rm -f out_parent_public
echo "tpm2_readpublic -H ${SRKHANDLE} --opu out_parent_public -T device -V"
tpm2_readpublic -H ${SRKHANDLE} --opu out_parent_public -T device -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo" Error:Reading Public part of Primary Key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""

# 5. Update the tpm_status.yaml to report the status of this Script
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml
