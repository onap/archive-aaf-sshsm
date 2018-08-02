#!/bin/bash

set -e

# Start DBUS
mkdir -p /var/run/dbus
stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# Start Resource Manager
if [ -z $TPM_SIMULATOR ]; then
  tpm2-abrmd
else
  hostip=$(ip route show | awk '/default/ {print $3}')
  echo "Connecting to $hostip\n"
  tpm2-abrmd -a $hostip -t socket&

/abrmd/bin/initialize_tpm.sh
status=$?
if [ $status -eq "0" ]; then
 echo "TPM Initialization successful $status"
fi
exit $?;
