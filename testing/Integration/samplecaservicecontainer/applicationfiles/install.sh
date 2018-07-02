#!/bin/bash
sudo apt-get update
sudo apt-get install -y softhsm2
sudo apt-get install -y opensc
#sudo apt-get install -y default-jdk
wget https://www.bouncycastle.org/download/bcprov-jdk15on-159.jar
wget https://www.bouncycastle.org/download/bcpkix-jdk15on-159.jar
wget https://www.bouncycastle.org/download/bcmail-jdk15on-159.jar
wget https://www.bouncycastle.org/download/bcpg-jdk15on-159.jar
wget https://www.bouncycastle.org/download/bctls-jdk15on-159.jar
wget https://www.bouncycastle.org/download/bctest-jdk15on-159.jar
sudo cp ./bcpkix-jdk15on-159.jar /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/ext/
sudo cp ./bcprov-jdk15on-159.jar /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/ext/
sudo cp ./bcmail-jdk15on-159.jar /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/ext/
sudo cp ./bcpg-jdk15on-159.jar /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/ext/
sudo cp ./bctls-jdk15on-159.jar /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/ext/
cp ./test.csr /tmp
#cp ./pkcs11.cfg /tmp
