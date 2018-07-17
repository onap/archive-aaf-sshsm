#!/bin/sh
set -e
cd /createca
/createca/create_ca.sh
cd /volume
DLIST=`ls -d host_*`
for DIR in $DLIST; do
  echo $DIR
  cp /createca/ca/ca.cert /volume/$DIR
  cd /volume/$DIR
  /dup/bin/ossl_tpm_duplicate -pemfile /createca/ca/privkey.pem  -parentPub /dup/database/$DIR/out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey
done
