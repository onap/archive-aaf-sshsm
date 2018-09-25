#!/bin/bash

# Script to check if abrmd is ready
# This will be called in initContainers in Kubernetes
# Takes a seconds parameter that will time out the script

timeout=$1
start=$SECONDS
tpm2_listpcrs -T tabrmd >/dev/null
ret=$?

while [ $ret -ne 0 ]
do
  if (($SECONDS-$start > $timeout))
  then
    echo "$0 timed out after $timeout seconds"
    break
  fi

  sleep 10
  tpm2_listpcrs -T tabrmd >/dev/null
  ret=$?
done
