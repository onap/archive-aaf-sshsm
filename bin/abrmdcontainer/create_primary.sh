#!/bin/sh

# Utility Script to create a primary key
# Uses TCTI as device
# It takes three arguments, a STORAGE HANDLE, the RH_OWNER Password and the KEY PASSWORD
SRKHANDLE=$1
O_PASSWORD=$2
KEY_PASSWORD=$3

# TPM Startup
echo "tpm2_startup --clear -T device --verbose"
tpm2_startup --clear -T device -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
error=$(echo "TPM Startup failed"); flag="0";
echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
exit 1;
fi
echo ""

#Check if Primary Key already exists
echo "tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T device --verbose"
tpm2_readpublic -H ${SRKHANDLE} --opu out_primary_public -T device -V

if [ $? -ne 0 ]; then echo; echo -e "${YELLOW} Primary Key does not exist, creating...";
    rm -f PrimaryKeyBlob
    echo "tpm2_createprimary -P ${O_PASSWORD} -K ${KEY_PASSWORD} -A o -g 0x000B
        -G 0x0001 -T device -V -C PrimaryKeyBlob"
    
    tpm2_createprimary -P ${O_PASSWORD} -K ${KEY_PASSWORD} -A o -g 0x000B \
        -G 0x0001 -T device -V -C PrimaryKeyBlob
    
    if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
        error=$(echo "Error: TPM create Primary key failed");
        echo "$error"; flag="0";
        echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
        echo "error:${error}" >> ${WORKDIR}/tpm_status.yaml;
        exit 1;
    fi
    echo ""


    #Store Primary Key in TPMs NV RAM
    echo "tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${SRKHANDLE}
        -T device -V -P ${O_PASSWORD}"
    
    tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${SRKHANDLE} \
        -T device -V -P ${O_PASSWORD}
    
    if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";
    error=$(echo "Error: Inserting Primary Key failed");
    echo "$error"; flag="0";
    echo "flag:${flag}" >> ${WORKDIR}/tpm_status.yaml;
    echo "errror:${error}" >> ${WORKDIR}/tpm_status.yaml;
    exit 1;
    fi
    echo ""
    rm -f PrimaryKeyBlob
fi

#END