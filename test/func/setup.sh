#!/bin/bash
#
# Copyright 2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Receives two arguments 1. srk_handle 2. password

docker login -u docker -p docker nexus3.onap.org:10001
docker pull nexus3.onap.org:10001/onap/aaf/distcenter
docker pull nexus3.onap.org:10001/onap/aaf/abrmd
docker pull nexus3.onap.org:10001/onap/aaf/testcaservice
docker pull nexus3.onap.org:10001/onap/aaf/aaf_cm

ABRMD_DATA="$(pwd)/data/abrmd"
SEC_FOLDER="$(pwd)/data/abrmd/cred"

DISTCENTER_DATA="$(pwd)/data/distcenter"
DISTCENTER_SEC="$(pwd)/data/distcenter/data"

CASERVICE_DATA="$(pwd)/data/caservice"
WORKDIR="$(pwd)/"

echo $1 > $ABRMD_DATA/cred/srk_handle.txt
echo $2 > $ABRMD_DATA/cred/opass.txt

base64 $ABRMD_DATA/cred/opass.txt > $ABRMD_DATA/cred/opass
base64 $ABRMD_DATA/cred/srk_handle.txt > $ABRMD_DATA/cred/srk_handle


if [ -e /dev/tpm0 ]; then

    echo "found tpm device"
    #
    # Running the abrmd container for tpm init
    #
    docker run --privileged -device=/dev/tpm0 -e ABRMD_DATA=/abrmd/data \
       -e TPM_NODE_NAME=testnode \
       -v $SEC_FOLDER:/abrmd/cred \
       -v $ABRMD_DATA:/abrmd/data \
       --entrypoint /abrmd/bin/initialize_tpm.sh \
       --name tpm-init nexus3.onap.org:10001/onap/aaf/abrmd

    sleep 1m
    mkdir -p $DISTCENTER_DATA/host_testnode
    ls  $ABRMD_DATA/host_testnode/
    cp  $ABRMD_DATA/host_testnode/out_parent_public \
          $DISTCENTER_DATA/host_testnode/

    docker run -d --privileged -device=/dev/tpm0 \
            -v /tmp/run/dbus:/var/run/dbus:rw \
       --entrypoint /abrmd/bin/run_abrmd.sh \
       --name abrmd nexus3.onap.org:10001/onap/aaf/abrmd

    sleep 1m
fi

#
# Running the distcenter container
#
docker run -v $DISTCENTER_DATA:/distcenter/data \
  --name distcenter nexus3.onap.org:10001/onap/aaf/distcenter


mkdir -p $CASERVICE_DATA/host_testnode
if [ -f $DISTCENTER_DATA/privkey.pem.gpg ]; then
    cp $DISTCENTER_DATA/privkey.pem.gpg $CASERVICE_DATA/host_testnode/
    cp $DISTCENTER_DATA/ca.cert $CASERVICE_DATA/host_testnode/
    cp $DISTCENTER_DATA/passphrase $CASERVICE_DATA/host_testnode/
else
    cp  $DISTCENTER_DATA/host_testnode/* \
          $CASERVICE_DATA/host_testnode
    cp $SEC_FOLDER/srk_handle $CASERVICE_DATA/host_testnode/
fi

cp $CASERVICE_DATA/passphrase-ipass $CASERVICE_DATA/host_testnode/
cp $CASERVICE_DATA/passphrase-pin $CASERVICE_DATA/host_testnode/
cd $CASERVICE_DATA/
cat passphrase-ipass | gpg --symmetric -z 9 --require-secmem \
       --cipher-algo AES256 --s2k-cipher-algo AES256 --s2k-digest-algo SHA512 \
       --s2k-mode 3 --s2k-count 65000000 --compress-algo BZIP2 \
        --passphrase-fd 0 ipass.txt
cat passphrase-pin | gpg --symmetric -z 9 --require-secmem \
       --cipher-algo AES256 --s2k-cipher-algo AES256 --s2k-digest-algo SHA512 \
       --s2k-mode 3 --s2k-count 65000000 --compress-algo BZIP2 \
        --passphrase-fd 0 upin.txt
cat passphrase-pin | gpg --symmetric -z 9 --require-secmem \
       --cipher-algo AES256 --s2k-cipher-algo AES256 --s2k-digest-algo SHA512 \
       --s2k-mode 3 --s2k-count 65000000 --compress-algo BZIP2 \
        --passphrase-fd 0 sopin.txt
cp $CASERVICE_DATA/*.gpg $CASERVICE_DATA/host_testnode/
cp $ABRMD_DATA/cred/srk_handle.txt $CASERVICE_DATA/host_testnode/tpm-handle.txt
cp $ABRMD_DATA/cred/opass $CASERVICE_DATA/host_testnode/prk_passwd
sleep 1m
