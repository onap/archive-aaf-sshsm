#!/bin/sh

echo "../import/ossl_tpm_import -H 0x81000011 -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey -pub outPub -priv outPriv"
rm -f outPub outPriv
../import/ossl_tpm_import -H 0x81000011 -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey -pub outPub -priv outPriv

