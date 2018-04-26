ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libtpm2-plugin.la
libtpm2_plugin_la_SOURCES = tpm2_plugin_api.c tpm2_plugin_init.c
libtpm2_plugin_la_LDFLAGS = -version-info @VERSION_INFO@  -lsapi -ltcti-tabrmd -ltcti-socket -ltcti-device -lcrypto -lssl -ldl
