#!/bin/bash

#set -e
sudo kill -9 $(ps -ef | grep "apt" | grep -v grep | awk '{print $2}')

sudo rm /var/lib/dpkg/lock
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock

sudo dpkg --configure -a

sudo dpkg --remove --force-remove-reinstreq linux-headers-generic
sudo dpkg --remove --force-remove-reinstreq linux-generic
sudo apt-get -f install
sudo apt-get install linux-image-generic

sudo apt -y update
sudo apt -y install autoconf
sudo apt -y install autoconf-archive
sudo apt -y install libglib2.0-dev
sudo apt -y install libdbus-1-dev
sudo apt -y install automake
sudo apt -y install libtool
sudo apt -y install autotools-dev
sudo apt -y install libcppunit-dev
sudo apt -y install p11-kit
sudo apt -y install libcurl4-gnutls-dev
sudo apt -y autoremove

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

echo "Build SoftHSMv2..."
cd SoftHSMv2
sh autogen.sh
./configure --with-openssl=/usr/local --disable-gost
make check
make
make install
cd ..

sudo apt -y install \
  libcmocka0 \
  libcmocka-dev \
  build-essential \
  git \
  pkg-config \
  gcc \
  g++ \
  m4 \
  liburiparser-dev \
  libgcrypt20-dev \
  libssl-dev \
  pandoc

echo "Install tpm2-tss 2.0.0"
wget https://github.com/tpm2-software/tpm2-tss/releases/download/2.0.0/tpm2-tss-2.0.0.tar.gz
tar -xvf tpm2-tss-2.0.0.tar.gz
wget https://github.com/tpm2-software/tpm2-abrmd/releases/download/2.0.0/tpm2-abrmd-2.0.0.tar.gz
tar -xvf tpm2-abrmd-2.0.0.tar.gz
wget https://github.com/tpm2-software/tpm2-tools/releases/download/3.1.0/tpm2-tools-3.1.0.tar.gz
tar -xvf tpm2-tools-3.1.0.tar.gz

cd tpm2-tss-2.0.0
./configure
make
make install
cp /tpm2-tss-2.0.0/src/util/tpm2b.h /usr/local/include/tss2/
cd ..
rm -rf tpm2-tss-2.0.0

cd tpm2-abrmd-2.0.0
useradd --system --user-group tss
./configure --with-dbuspolicydir=/etc/dbus-1/system.d \
    --with-udevrulesdir=/etc/udev/rules.d/ \
    --with-systemdsystemunitdir=/lib/systemd/system
make
make install
cd ..
rm -rf tpm2-abrmd-2.0.0

cd tpm2-tools-3.1.0
  ./configure
  make
  make install
cd ..
rm -rf tpm2-tools-3.1.0

echo "Build Duplicate utility tool"
cd tpm-util
cd duplicate
make -f sampleMakefile
cd ..

echo "Build Import utility tool"
cd tpm-util
cd import
make -f sampleMakefile
cd ..

echo "Build TPM2_plugin..."
cd TPM2-Plugin
./bootstrap
./configure
make
sudo make install
sudo ldconfig
cd ..
