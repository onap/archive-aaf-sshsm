#!/bin/bash
set -e

# Start DBUS
mkdir -p /var/run/dbus
stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# Start Resource Manager
if [ -z $TPM_SIMULATOR ]; then
  echo "Using TPM Hardware for the operations";
  tpm2-abrmd
else
  echo "Using TPM Simulator for the opeations";
  hostip=$(ip route show | awk '/default/ {print $3}');
  echo "Connecting to $hostip\n";
  tpm2-abrmd -a $hostip -t socket
fi
