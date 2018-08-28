#!/bin/bash

set -e

echo "Shared Volume location is $ABRMD_DATA"
echo "Node name is $TPM_NODE_NAME"

# Start DBUS
mkdir -p /var/run/dbus
stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &
# Time for Daemon to start before executin next step
sleep 1m
# Start Resource Manager
if [ -z $TPM_SIMULATOR ]; then
    echo "Using TPM Hardware for the operations"
    tpm2-abrmd &
    # Time for abrmd process to start
    sleep 1m
    state=$( ps aux | grep tpm2-abrmd | grep -v grep )
    echo "Staus of abrmd Process is $state"
else
    echo "Using TPM Simulator for the opeations";
    hostip=$(ip route show | awk '/default/ {print $3}');
    echo "Connecting to $hostip\n";
    tpm2-abrmd -a $hostip -t socket&
fi

/abrmd/bin/initialize_tpm.sh
status=$?
if [ $status -eq "0" ]; then
    echo "TPM Initialization successful $status"
fi
exit status