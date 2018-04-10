#!/etc/bash

#set -e
sudo kill -9 $(ps -ef | grep "apt" | grep -v grep | awk '{print $2}')

sudo rm /var/lib/dpkg/lock
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock

sudo dpkg --configure -a

#sudo apt -y update
sudo apt -y install autoconf
sudo apt -y install autoconf-archive
sudo apt -y install libglib2.0-dev
sudo apt -y install libdbus-1-dev
sudo apt -y install automake
sudo apt -y install libtool
sudo apt -y install autotools-dev

echo "Build SoftHSMv2..."
cd SoftHSMv2
sh autogen.sh
./configure --disable-gost
make -j$(nproc)
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

#echo "Install openssl..."
#git clone https://github.com/openssl/openssl.git
#cd openssl
#./config
#make -j$(nproc)
#sudo make install
#cd ..
#rm -rf openssl

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
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
