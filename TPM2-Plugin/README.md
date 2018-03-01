## Introduction

This is TPM2-Plugin to gererate asymetric key pairs from TPM2.0 module 
and save them in SoftHSM Token object folder in encryped fasion.
The private part of keys can only be used for signing when it is loaded in TPM module.
### Build

./bootstrap
./configure --prefix test
