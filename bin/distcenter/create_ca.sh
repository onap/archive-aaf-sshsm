#!/bin/bash
CA_DIR=$PWD/ca
mkdir $CA_DIR
mkdir -p $PWD/certs
cd $CA_DIR
echo "000a" > serial
touch certindex
openssl req -x509 -newkey rsa:2048 -days 3650 -nodes -out ca.cert -subj '/C=US/ST=CA/L=local/O=onap/CN=test.onap.ca'
