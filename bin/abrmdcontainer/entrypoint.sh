#!/bin/bash
set -e

# Location of Shared volume
export sharedvolume="/tmp/files/sampleabrmdcontainer/inittoolfiles"

# Start DBUS
mkdir -p /var/run/dbus
stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# Start Resource Manager
hostip=$(ip route show | awk '/default/ {print $3}')
echo "Connecting to $hostip\n"
tpm2-abrmd -a $hostip -t socket&

# For TPM Hardware presence
#tpm2-abrmd

# Create the directory with the host's name to store the output of Init tool
# Use the environemt variable "tpm_host_name" if available else use a sample host name(onap)
# This is demarcate the generated files which are specific to this host's TPM
if [ -z ${tpm_host_name} ]; then
  mkdir -p ${sharedvolume}/onap
else
  mkdir -p ${sharedvolume}/${tpm_host_name}
fi

# Run the Initialization tool
./initialize_tpm.sh
