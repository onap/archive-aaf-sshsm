#!/etc/bash

#set -e
sudo apt -y upgrade
echo "upgrade done ..."
sudo cat /var/lib/dpkg/lock
lsof /var/lib/dpkg/lock > aaa
cat aaa

#sudo rm /var/lib/dpkg/lock
#sudo rm /var/cache/apt/archives/lock
sudo dpkg --configure -a
sudo apt-get -y install autoconf
sudo apt-get -y install automake
sudo apt-get -y install libtool
sudo apt-get -y install autotools-dev


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


