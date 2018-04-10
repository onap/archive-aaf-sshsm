## Introduction

This is TPM2-Plugin to load asymetric key pairs to TPM2.0 module.
The private part of keys can only be used for signing when it is loaded in TPM module.
### Build

./bootstrap
./configure --prefix test

### Installation
make install

###Uninstall

make clean
make distclean
