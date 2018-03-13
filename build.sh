#!/etc/bash

#sudo systemctl stop apt-daily.timer
#sudo systemctl start apt-daily.timer
#systemctl daemon-reload
echo "checking PID for locks ..."
echo "==========================================================="
sudo ps -A | grep apt > apt-process
cat apt-process
kill -9 $(ps -ef | grep "apt" | grep -v grep | awk '{print $2}')

echo "checking again PID for locks ..."
echo "==========================================================="
sudo ps -A | grep apt > apt-process
cat apt-process
kill -9 $(ps -ef | grep "apt" | grep -v grep | awk '{print $2}')
echo "remove locks ..."
echo "==========================================================="
sudo rm /var/lib/dpkg/lock
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock
#sudo cat aaa > /var/lib/dpkg/updates/0000
#sudo mkdir /var/lib/dpkg/updates/0000
#sudo dpkg --configure -a
echo "Update and upgrade  ..."
echo "==========================================================="
sudo apt -y update
#sudo dpkg --configure -a
sudo apt-get upgrade

echo "upgrade done ..."
#sudo apt -y upgrade
#sudo dpkg --configure -a
#sudo apt -y autoremove
echo "install autotools ..."
echo "==========================================================="
#sudo dpkg --configure -a
sudo apt-get -y install autoconf
sudo apt-get -y install automake
sudo apt-get -y install libtool
sudo apt-get -y install autotools-dev
sudo apt -y autoremove
echo "install tpm2-tools ..."
echo "==========================================================="
sudo dpkg --configure -a
sudo apt -y install tpm2-tools
sudo find / |grep tpm20.h
echo "================== list /usr/local ================"
echo "==========================================================="
sudo ls -l /usr/local
echo "==================== list /usr/include ================"
echo "==========================================================="
sudo ls -l /usr/include
echo "=============== list /usr/local/include ================"
echo "==========================================================="

sudo ls -l /usr/local/include

echo "Build SoftHSMv2..."
echo "==========================================================="
cd SoftHSMv2
sh autogen.sh
./configure
make
cd ..

echo "Build TPM2_plugin..."
echo "==========================================================="
cd TPM2-Plugin
./bootstrap
./configure
make
cd ..


