#!/bin/sh
set -e
cd /createca
/createca/create_ca.sh
cd /createca/ca
cat /volume/passphrase | gpg --no-tty --symmetric -z 9 --require-secmem --cipher-algo AES256 --s2k-cipher-algo AES256 --s2k-digest-algo SHA512 --s2k-mode 3 --s2k-count 65000000 --compress-algo BZIP2 --passphrase-fd 0 privkey.pem
cp /createca/ca/privkey.pem.gpg /volume

cd /volume
DLIST=`ls -d host_*`
for DIR in $DLIST; do
  echo $DIR
  cp /createca/ca/ca.cert /volume/$DIR
  cd /volume/$DIR
  /dup/bin/ossl_tpm_duplicate -pemfile /createca/ca/privkey.pem  -parentPub /volume/$DIR/out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed dupSymseed -dupEncKey dupEncKey
done
