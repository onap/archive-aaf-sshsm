#!/bin/sh

# 1. Environmental variable for SRK Handle file
srkhandle="0x81000011"

# 2. Create the flag.yaml file to report Success/Failure Flag along with error
touch flag.yaml
chmod 755 flag.yaml

# 3. Create initial Flag values which are reset upon failure
error="NULL"
flag="1"

case "$1" in

--out-public)
    out_primary_public=$2
    ;;

--help)
    echo "$0 [--out-public primaty_public_bin_file (optional)]"
    exit 0
    ;;

*)
    out_primary_public="out_parent_public"
    ;;

esac

# 4. TPM initialize
echo "tpm2_startup -clear -T tabrmd -V"
tpm2_startup -clear -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; error=$(echo "TPM Startup failed"); flag="0"; echo "flag:${flag}" >> flag.yaml; echo "error:${error}" >> flag.yaml; exit 1; fi
echo ""


# 5. Take ownership
echo "tpm2_takeownership -o new -e new -l new -T tabrmd -V"
tpm2_takeownership -o new -e new -l new -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";error=$(echo "Error:TPM ownership acquire failed");flag="0"; echo "flag:${flag}" >> flag.yaml; echo "error:${error}" >> flag.yaml; echo "$error"; exit 1; fi
echo ""

# 6. Create Primary Key in RH_OWNER hierarchy
rm -f PrimaryKeyBlob
echo "tpm2_createprimary -P new -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob"
tpm2_createprimary -P new -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";error=$(echo "Error: TPM create Primary key failed"); echo "$error"; flag="0";  echo "flag:${flag}" >> flag.yaml; echo "error:${error}" >> flag.yaml; exit 1; fi
echo ""

# 7. Store Primary Key in TPMs NV RAM
echo "tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${srkhandle} -T tabrmd -V -P new"
tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S ${srkhandle} -T tabrmd -V -P new
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit.";error=$(echo "Error: Inserting Primary Key failed"); echo "$error"; flag="0";  echo "flag:${flag}" >> flag.yaml; echo "errror:${error}" >> flag.yaml; exit 1;  fi
echo ""
rm -f PrimaryKeyBlob

# 8. To test, Read public portion of TPM primary key with stored handle
rm -f $out_primary_public
echo "tpm2_readpublic -H ${srkhandle} --opu $out_primary_public -T tabrmd -V"
tpm2_readpublic -H ${srkhandle} --opu $out_primary_public -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; error=$(echo" Error:Reading Public part of Primark Key failed"); echo "$error"; flag="0"; echo "flag:${flag}" >> flag.yaml; echo "error:${error}" >> flag.yaml; exit 1; fi
echo ""

# 9. Update the flag.yaml to report the status of this Script
echo "flag:${flag}" >> flag.yaml
echo "error:${error}" >> flag.yaml

# 10. Copy the public portion of the Primary key to the Shared volume, onap is the sample hostname use
# Use environment variable tpm_host_name is available else use onap as the sample host name
if [ -z ${tpm_host_name} ]; then
  cp out_parent_public ${sharedvolume}/onap/
  cp flag.yaml ${sharedvolume}/onap/
else
  cp out_parent_public ${sharedvolume}/${tpm_host_name}
  cp flag.yaml ${sharedvolume}/${tpm_host_name}
fi

