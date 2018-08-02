# Adding a Wrapper since this container must run
# tpm2-abrmd process and perform TPM initialization
# operation

#!/bin/bash

./entrypoint.sh -D
status=$?
if [ $status -ne 0 ]; then
 echo "Failed to start abrmd process $status"
 exit $status;
fi

./initialize_tpm.sh -D
status=$?
if [ $status -eq "0" ]; then
 echo "TPM is initialized successfully $status"
fi

while sleep 10; do
  abrmd_status=$( ps aux | grep tpm2-abrmd | grep -v grep )
  if [ -z "$abrmd_status" ] ; then
    echo "No abrmd process"
    exit 1
  else
    echo "The abrmd process is up"
  fi
done
