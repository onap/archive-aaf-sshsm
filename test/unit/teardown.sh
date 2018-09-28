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

CASERVICE_DATA="$(pwd)/data/caservice"
DISTCENTER_DATA="$(pwd)/data/distcenter"
ABRMD_DATA="$(pwd)/data/abrmd"
rm -rf $CASERVICE_DATA/host_testnode
rm -f $DISTCENTER_DATA/privkey.pem.gpg
rm -f $DISTCENTER_DATA/ca.cert
rm -f $ABRMD_DATA/host_testnode/tpm_status.yaml

docker stop tpm-init abrmd distcenter testca aafca
docker rm tpm-init abrmd distcenter testca aafca
rm -f setup.log

