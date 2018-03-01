AM_CPPFLAGS = -I ./include
lib_LTLIBRARIES = libtpm2-plugin.la
libtpm2_plugin_la_SOURCES =  tpm2_error.c tpm2_plugin_api.c tpm2_plugin_init.c tpm2_tcti_ldr.c tpm2_util.c  log.c  plugin_register.c files.c tpm2_attr_util.c tpm2_alg_util.c tpm2_hash.c
libtpm2_plugin_la_LDFLAGS = -version-info @VERSION_INFO@  -lsapi -ltcti-socket -ltcti-device -lcrypto -lssl -ldl
