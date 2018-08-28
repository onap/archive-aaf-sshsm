#!/bin/sh

# 1. Create the directory with the host's name to store the output of Init tool
# This is demarcate the generated files which are specific to this host's TPM
mkdir -p ${ABRMD_DATA}/host_${TPM_NODE_NAME}
WORKDIR=${ABRMD_DATA}/host_${TPM_NODE_NAME}
cd ${WORKDIR}
# /abrmd/cred will contain srk_handle and tpm owner hierarchy password
# provided by the admin of the TPM node
SRKHANDLE="$(cat /abrmd/cred/srk_handle | base64 -d)"
TPMPASSWD="$(cat /abrmd/cred/tpm_pass | base64 -d)"

# 2. Create initial Flag values which are reset upon failure
error="NULL"
flag="1"

# 3. TPM initialize
echo "tpm2_startup --clear -T tabrmd --verbose"
tpm2_startup --clear -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "TPM Startup failed"); flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""

# 4. Create Primary Key in RH_OWNER hierarchy if one doesn't exist already
#    This is a step that is executed once and will not be run after the TPM
#    primary key has been created.
rm -f out_primary_public
echo "tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T tabrmd --verbose"
tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${YELLOW} Primary Key does not exist, creating...";

rm -f PrimaryKeyBlob
echo "tpm2_createprimary -P ${TPMPASSWD} -a o --halg 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob"
tpm2_createprimary -P ${TPMPASSWD} -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "Error: TPM create Primary key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""


#Store Primary Key in TPMs NV RAM
echo "tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${SRKHANDLE} -T tabrmd -V -P ${TPMPASSWD}"
tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${SRKHANDLE} -T tabrmd -V -P ${TPMPASSWD}
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "Error: Inserting Primary Key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "errror:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""
rm -f PrimaryKeyBlob

#End of step 5
fi

# 5. To test, Read public portion of TPM primary key with stored handle
rm -f out_primary_public
echo "tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T tabrmd -V"
tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo" Error:Reading Public part of Primary Key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""

# 6. Update the tpm_status.yaml to report the status of this Script
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml
