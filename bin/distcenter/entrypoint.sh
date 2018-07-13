#!/bin/sh
set -e
cd /volume
DLIST=`ls -d host_*`
for DIR in $DLIST; do 
  echo $DIR
  mkdir -p /ca/$DIR
  cd /ca/$DIR
  /ca/create_ca.sh
  cp /ca/$DIR/ca/ca.cert /volume/$DIR
  cd /volume/$DIR
  /dup/bin/ossl_tpm_duplicate -pemfile /ca/$DIR/ca/privkey.pem  -parentPub /dup/database/$DIR/out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey
done
