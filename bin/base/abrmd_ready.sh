#!/bin/bash

# Script to check if abrmd is ready
# This will be called in initContainers in Kubernetes

tpm2_listpcrs -T tabrmd >/dev/null
ret=$?

while [ $ret -ne 0 ]
do
  sleep 10
  tpm2_listpcrs -T tabrmd >/dev/null
  ret=$?
done
