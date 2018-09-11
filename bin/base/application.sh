#!/bin/bash


# This script receives 2 arguments
applicationlibrary=$1
SoftHSMv2SlotID=$2

# Setting up the pkcs11 config file
echo "library = ${applicationlibrary}" >> /opt/app/osaaf/local/org.osaaf.aaf.cm.pkcs11
echo "slot = ${SoftHSMv2SlotID}" >> /opt/app/osaaf/local/org.osaaf.aaf.cm.pkcs11
