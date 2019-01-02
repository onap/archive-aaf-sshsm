#!/bin/sh
set -e
cd /distcenter
/distcenter/bin/create_ca.sh

cd /distcenter/data
DCOUNT=$(ls -d host_*  2> /dev/null | wc -l)
if [ "$DCOUNT" != "0" ]
then
  DLIST=`ls -d host_*`
  #Iterate over all hosts (host in k8s corresponds node)
  for DIR in $DLIST; do
    echo $DIR
    cp /distcenter/ca/ca.cert /distcenter/data/$DIR
    cd /distcenter/data/$DIR
    /dup/bin/ossl_tpm_duplicate -pemfile /distcenter/ca/privkey.pem  -parentPub \
    /distcenter/data/$DIR/out_parent_public -dupPub dupPub -dupPriv dupPriv -dupSymSeed \
    dupSymseed -dupEncKey dupEncKey
  done
else
  cd /distcenter/ca
  cat /distcenter/data/passphrase | gpg --no-tty --symmetric -z 9 --require-secmem \
    --cipher-algo AES256 --s2k-cipher-algo AES256 --s2k-digest-algo SHA512 \
    --s2k-mode 3 --s2k-count 65000000 --compress-algo BZIP2 \
    --passphrase-fd 0 privkey.pem
  cp /distcenter/ca/privkey.pem.gpg /distcenter/data
  cp /distcenter/ca/ca.cert /distcenter/data
fi
