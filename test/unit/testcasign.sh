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

echo -n "testca container key import & sign: "
docker run --workdir /testca/bin -e SECRETS_FOLDER=/caservice/data \
     -e DATA_FOLDER=/caservice/data \
     -v $CASERVICE_DATA/host_testnode:/caservice/data \
     --entrypoint /testca/bin/import.sh \
     --name testca nexus3.onap.org:10001/onap/aaf/testcaservice \
     >&  runtestca.log 
grep -q "test.cert: OK" runtestca.log
if [ "$?" -eq 0 ]; then
    echo   Success
    rm -f runtestca.log
else
    echo Failed
    exit 1
fi
