#!/bin/sh

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

#echo "out-public file: $out_primary_public"

# TPM initialize
echo "tpm2_startup -clear -T tabrmd -V"
tpm2_startup -clear -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; exit 1; fi
echo ""

# Take ownership
echo "tpm2_takeownership -o new -e new -l new -T tabrmd -V"
tpm2_takeownership -o new -e new -l new -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; exit 1; fi
echo ""

# Create Primary Key in RH_OWNER hierarchy
rm -f PrimaryKeyBlob
echo "tpm2_createprimary -P new -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob"
tpm2_createprimary -P new -A o -g 0x000B -G 0x0001 -T tabrmd -V -C PrimaryKeyBlob
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; exit 1; fi
echo ""

# Store Primary Key in TPMs NV RAM
echo "tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S 0x81000011 -T tabrmd -V -P new"
tpm2_evictcontrol -A o -c ./PrimaryKeyBlob -S 0x81000011 -T tabrmd -V -P new
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; exit 1; fi
echo ""
rm -f PrimaryKeyBlob

# To test, Read public portion of TPM primary key with stored handle
rm -f $out_primary_public
echo "tpm2_readpublic -H 0x81000011 --opu $out_primary_public -T tabrmd -V"
tpm2_readpublic -H 0x81000011 --opu $out_primary_public -T tabrmd -V
if [ $? -ne 0 ]; then echo; echo -e "${RED}Error, Exit."; exit 1; fi
echo ""


# Some TPM commands to test
#tpm2_load -c PrimaryKeyBlob -u outPub -r outPriv -n ChildKeyName -C ContextChild

