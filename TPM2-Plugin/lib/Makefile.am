TPM2_TOOL_LIB_DIR=/root/tpm2-stacks/tpm2-tools/lib
TPM2_TOOL_TOOLS_DIR=/root/tpm2-stacks/tpm2-tools/tools
OPENSSL_DIR=/opt/openssl
AM_CPPFLAGS = -I ./include -I $(OPENSSL_DIR) -I $(TPM2_TOOL_LIB_DIR) -I $(TPM2_TOOL_TOOLS_DIR)
lib_LTLIBRARIES = libtpm2-plugin.la
libtpm2_plugin_la_SOURCES = tpm2_plugin_api.c tpm2_plugin_init.c 
libtpm2_plugin_la_LDFLAGS = -version-info @VERSION_INFO@  -lsapi -ltcti-tabrmd -ltcti-socket -ltcti-device -lcrypto -lssl -ldl
