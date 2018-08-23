#!/bin/sh

# 1.Environmental variables

# 1.a Location of Shared volume and Node's name
# These varaibles has to be made avaialble to this script

echo "Shared Volume location is $ABRMD_DATA"
echo "Node name is $TPM_NODE_NAME"

# 2. Create the directory with the host's name to store the output of Init tool
# This is demarcate the generated files which are specific to this host's TPM
srkhandle="$(cat ${ABRMD_DATA}/host_${TPM_NODE_NAME}/srkhandle)"
passphrase="$(cat ${ABRMD_DATA}/host_${TPM_NODE_NAME}/password-passphrase)"
echo "${passphrase}" | gpg --batch --yes --passphrase-fd 0 password.txt.gpg
password="$(cat ${ABRMD_DATA}/host_${TPM_NODE_NAME}/password.txt)"

# 3. Create initial Flag values which are reset upon failure
error="NULL"
flag="1"

# 4. TPM initialize
echo "tpm2_startup -clear -T tabrmd -V"
tpm2_startup -clear -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "TPM Startup failed"); flag="0";
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "error:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
exit 1;
fi
echo ""

# 5. Take ownership
echo "tpm2_takeownership -o new -e new -l new -T tabrmd -V"
tpm2_takeownership -o new -e new -l new -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "Error:TPM ownership acquire failed");flag="0";
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "error:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "$error"; exit 1;
fi
echo ""

# 6. Create Primary Key in RH_OWNER hierarchy
rm -f PrimaryKeyBlob
echo "tpm2_createprimary -P $password -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob"
tpm2_createprimary -P $password -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "Error: TPM create Primary key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "error:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
exit 1;
fi
echo ""

# 7. Store Primary Key in TPMs NV RAM
echo "tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${srkhandle} -T tabrmd -V -P $password"
tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${srkhandle} -T tabrmd -V -P $password
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "Error: Inserting Primary Key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "errror:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
exit 1;
fi
echo ""
rm -f PrimaryKeyBlob

# 8. To test, Read public portion of TPM primary key with stored handle
rm -f $out_primary_public
echo "tpm2_readpublic -H ${srkhandle} --opu out_primary_public -T tabrmd -V"
tpm2_readpublic -H ${srkhandle} --opu out_primary_public -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo" Error:Reading Public part of Primary Key failed");
echo "$error"; flag="0";
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
echo "error:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml;
exit 1;
fi
echo ""

# 9. Update the tpm_status.yaml to report the status of this Script
echo "flag:${flag}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml
echo "error:${error}" >> ${ABRMD_DATA}/host_${TPM_NODE_NAME}/tpm_status.yaml

# 10. Copy the public portion of the Primary key to the Shared volume
# Use environment variable TPM_NODE_NAME
cp out_parent_public ${ABRMD_DATA}/host_${TPM_NODE_NAME}
