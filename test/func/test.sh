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

#setup for testing
./setup.sh $1 $2 >& setup.log

#Run tests
./testaafimport.sh
if [ "$?" -ne 0 ]; then
   exit 1
fi
./testcasign.sh
if [ "$?" -ne 0 ]; then
   exit 1
fi

#cleanup
./teardown.sh >& /dev/null
