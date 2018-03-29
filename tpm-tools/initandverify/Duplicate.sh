#!/bin/sh

echo "../duplicate/ossl_tpm_duplicate -pemfile ./private.pem  -parentPub out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey"
rm -f dupPub dupPriv dupSymseed dupEncKey
../duplicate/ossl_tpm_duplicate -pemfile ./private.pem  -parentPub out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey

