#!/bin/bash

set -e
sudo apt -y update

sudo apt-get -y install \
    autoconf \
    autoconf-archive \
    libglib2.0-dev \
    libdbus-1-dev \
    automake \
    libtool \
    autotools-dev \
    libcppunit-dev \
    p11-kit \
    libcurl4-gnutls-dev \
    libcmocka0 \
    libcmocka-dev \
    build-essential \
    git \
    pkg-config \
    vim \
    gcc \
    g++ \
    m4 \
    curl \
    wget \
    liburiparser-dev \
    libssl-dev \
    pandoc \
    opensc \
    default-jdk \
    libgcrypt20-dev

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/

echo "Build SoftHSMv2..."
cd SoftHSMv2
sh autogen.sh
./configure --disable-gost
sudo make install
cd ..

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
sudo make install
sudo cp /tpm2-tss-2.0.0/src/util/tpm2b.h /usr/local/include/tss2/
cd ..
rm -rf tpm2-tss-2.0.0

cd tpm2-abrmd-2.0.0
useradd --system --user-group tss
./configure --with-dbuspolicydir=/etc/dbus-1/system.d \
    --with-udevrulesdir=/etc/udev/rules.d/ \
    --with-systemdsystemunitdir=/lib/systemd/system
make
sudo make install
cd ..
rm -rf tpm2-abrmd-2.0.0

cd tpm2-tools-3.1.0
./configure
make
sudo make install
cd ..
rm -rf tpm2-tools-3.1.0

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
