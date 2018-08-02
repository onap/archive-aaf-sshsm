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

# Create the directory with the name of the host this container is running
# This is demarcate the generated files which are specific to this host's TPM
mkdir -p ${sharedvolume}/onap

#This command will fetch the real host name. tpm_host_name is the environment variable which has the hostname in deployment
#mkdir -p ${sharedvolume}/${tpm_host_name}

# Run the Initialization tool
./initialize_tpm.sh

