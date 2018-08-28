#!/bin/bash

set -e

echo "Shared Volume location is $ABRMD_DATA"
echo "Node name is $TPM_NODE_NAME"

/abrmd/bin/run_abrmd.sh &
sleep 2m
/abrmd/bin/initialize_tpm.sh
status=$?
if [ $status -eq "0" ]; then
    echo "TPM Initialization successful $status"
fi
exit $status