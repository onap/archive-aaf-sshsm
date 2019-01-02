#!/bin/sh

echo "hello world.." > hello_tpm.txt

# Load the Child context in TPM
rm -f ChildKeyName ContextChild
tpm2_load -H 0x81000011 -u outPub -r outPriv -n ChildKeyName -C ContextChild

echo "tpm2_hash -H o -g 0x00B -I hello_tpm.txt -o hello_tpm_hash.bin -t outTicket"
rm -f hello_tpm_hash.bin outTicket
tpm2_hash -H o -g 0x00B -I hello_tpm.txt -o hello_tpm_hash.bin -t outTicket
echo ""

echo "tpm2_sign -c ContextChild -g 0x000B -m hello_tpm.txt -s hello_tpm.sig"
rm -f hello_tpm.sig
tpm2_sign -c ContextChild -g 0x000B -m hello_tpm.txt -s hello_tpm.sig
echo ""

echo "tpm2_verifysignature -c ContextChild -g 0x000b -m hello_tpm.txt -s hello_tpm.sig -t tk.sig"
rm -f tk.sig
tpm2_verifysignature -c ContextChild -g 0x000b -m hello_tpm.txt -s hello_tpm.sig -t tk.sig
echo ""

echo "Extracting signature from TPM format"
echo "dd if=hello_tpm.sig of=hello_tpm.sig.raw bs=1 skip=6 count=256"
rm -f hello_tpm.sig.raw
dd if=hello_tpm.sig of=hello_tpm.sig.raw bs=1 skip=6 count=256
echo ""

echo "openssl dgst -verify public.pem -keyform pem -sha256 -signature hello_tpm.sig.raw hello_tpm.txt"
openssl dgst -verify public.pem -keyform pem -sha256 -signature hello_tpm.sig.raw hello_tpm.txt
echo ""

rm -f hello_tpm_hash.bin outTicket tk.sig

