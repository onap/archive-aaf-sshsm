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
    default-jdk

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/

echo "Build SoftHSMv2..."
cd SoftHSMv2
sh autogen.sh
./configure --disable-gost
sudo make install
cd ..

echo "Install tpm2-tss 1.2.0..."
git clone https://github.com/tpm2-software/tpm2-tss.git
cd tpm2-tss
git checkout 1.2.0
./bootstrap
./configure --enable-unit
#cp ../implementation.h ./include/sapi/implementation.h
make -j$(nproc) check
sudo make install
sudo ldconfig
cd ..
rm -rf tpm2-tss

echo "Install tpm2-abrmd 1.1.1..."
#sudo useradd --system --user-group tss
git clone https://github.com/tpm2-software/tpm2-abrmd.git
cd tpm2-abrmd
git checkout 1.1.1
./bootstrap
./configure --with-dbuspolicydir=/etc/dbus-1/system.d --with-systemdsystemunitdir=/lib/systemd/system --with-systemdpresetdir=/lib/systemd/system-preset --with-udevrulesdir=/etc/udev/rules.d --datarootdir=/usr/share --enable-unit
make -j$(nproc) check
sudo make install
sudo ldconfig
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo pkill -HUP dbus-daemon
sudo systemctl daemon-reload
cd ..
rm -rf tpm2-abrmd

echo "Install tpm2-tools 2.1.1..."
git clone https://github.com/tpm2-software/tpm2-tools.git
cd tpm2-tools
git checkout 2.1.1
./bootstrap
./configure --enable-unit
make -j$(nproc) check
sudo make install
sudo ldconfig
cd ..
rm -rf tpm2-tools

echo "Build TPM2_plugin..."
cd TPM2-Plugin
./bootstrap
./configure
make
sudo make install
sudo ldconfig
cd ..
