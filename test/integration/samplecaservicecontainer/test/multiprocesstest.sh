#!/bin/bash

# Script for multiprocessing test
# Replace the varabiles as required
# "java CaSign <upin> <certid>"
count=15;
cd ../applicationfiles
javac CaSign.java
for ((i=1; i<$count; i++)); do
java CaSign 123456789 0x2222&
done
