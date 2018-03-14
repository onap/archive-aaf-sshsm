#!/etc/bash

#set -e
#sudo ps -A | grep apt > aaa
#sudo cat aaa
sudo kill -9 $(ps -ef | grep "apt" | grep -v grep | awk '{print $2}')

sudo rm /var/lib/dpkg/lock
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock

sudo dpkg --configure -a

sudo apt -y install autoconf
sudo apt -y install automake
sudo apt -y install libtool
sudo apt -y install autotools-dev
sudo apt -y install tpm2-tools

echo "Build SoftHSMv2..."
cd SoftHSMv2
sh autogen.sh
./configure
make
cd ..

echo "Build TPM2_plugin..."
cd TPM2-Plugin
./bootstrap
./configure
make
cd ..
