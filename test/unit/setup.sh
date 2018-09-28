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

if [ -f /dev/tpm0 ]; then

    SRK_HANDLE="$(cat ${SEC_FOLDER}/srk_handle | base64 -d)"
    K_PASS="$(cat ${SEC_FOLDER}/kpass | base64 -d)"
    O_PASS="$(cat ${SEC_FOLDER}/opass | base64 -d)"
    $(pwd)/bin/abrmdcontainer/create_primary.sh $SRK_HANDLE $O_PASS $K_PASS
    #
    # Running the abrmd container for tpm init 
    #
    docker run --privileged -device=/dev/tpm0 -e ABRMD_DATA=/abrmd/data \
       -e TPM_NODE_NAME=testnode \
       -v $SEC_FOLDER:/abrmd/cred \
       -v $ABRMD_DATA:/abrmd/data \
       --entrypoint /abrmd/bin/initialize_tpm.sh \
       --name tpm-init nexus3.onap.org:10001/onap/aaf/abrmd 
   
    cp  -r $(pwd)$ABRMD_DATA/host_testnode \
          $(pwd)$DISTCENTER_DATA

    docker run -d --privileged -device=/dev/tpm0 \
            -v /tmp/run/dbus:/var/run/dbus \
       --entrypoint /abrmd/bin/run_abrmd.sh \
       --name abrmd nexus3.onap.org:10001/onap/aaf/abrmd 
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
     cp $SEC_FOLDER/srk_handle $CASERVICE_DATA/host_testnode/
     cp $SEC_FOLDER/kpass $CASERVICE_DATA/host_testnode/prk_passwd

fi

cp $CASERVICE_DATA/passphrase-pin $CASERVICE_DATA/host_testnode/
cp $CASERVICE_DATA/upin.txt.gpg $CASERVICE_DATA/host_testnode/
cp $CASERVICE_DATA/sopin.txt.gpg $CASERVICE_DATA/host_testnode/
