# Introduction

This is TPM2-Plugin to load asymetric key pairs to TPM2.0 module.
The private part of keys can only be used for signing when it is loaded in TPM module.
Loading Password protected Primary Keys in plugin requires the setting of the
following ENVIRONMENT Variable:
```
TPM_PRK_PASSWORD
```
The plugin will read this and setup hmac appropriately for the session.
## Build

```
./bootstrap
./configure --prefix test
```

## Installation and Uninstallation
```
make install
```

```
make clean
make distclean
```