#!/etc/bash

sudo systemctl stop apt-daily.timer
systemctl daemon-reload
sudo ps -A | grep apt > apt-process
cat apt-process
ps -ef | grep apt | grep -v grep | awk '{print $2}' | xargs kill -9
sudo ps -A | grep apt > apt-process
cat apt-process
ps -ef | grep apt | grep -v grep | awk '{print $2}' | xargs kill -9
#sudo systemctl start apt-daily.timer
sudo rm /var/lib/dpkg/lock
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock
echo "remove locks ..."
echo "==========================================================="
sudo apt -y update
sudo apt-get dist-upgrade
sudo dpkg --configure -a

#sudo apt -y upgrade
#sudo dpkg --configure -a
#sudo apt -y autoremove
#echo "upgrade done ..."
echo "install autotools ..."
echo "==========================================================="
sudo apt-get -y install autoconf
sudo apt-get -y install automake
sudo apt-get -y install libtool
sudo apt-get -y install autotools-dev
sudo apt -y autoremove
echo "install tpm2-tools ..."
echo "==========================================================="
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


