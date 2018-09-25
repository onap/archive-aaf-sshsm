#!/bin/bash

# Script for multiprocessing test
# Replace the varabiles as required
# "java CaSign <upin> <certid>"
cd ../applicationfiles
javac CaSign.java
for ((i=1; i<14; i++)); do
java CaSign 123456789 0x2222&
done
