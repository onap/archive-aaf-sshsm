#!/etc/bash

#set -e
sudo systemctl stop apt-daily.timer

sudo apt-get -y install autoconf
sudo apt-get -y install automake
sudo apt-get -y install libtool
sudo apt-get -y install autotools-dev

sudo systemctl start apt-daily.timer

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


