#include <stdio.h>

#include "plugin_register.h"

#include "tpm2_plugin_api.h"

int __plugin_init(char* configPath)
{
// if tpm_plugin, do this
   printf("Init module done for TPM plug-in mode ! \n");
// if SGX_plugin, do this

	return 0;
}

int __plugin_finialize()
{
// if tpm_plugin, do this
	printf("Finalize module done for SW mode ! \n");
// if SGX_plugin, do this

	return 0;
}

int __plugin_functions_mapping(plugin_register *plugin_fp)
{
    printf("%s(): Assigning Function pointers for TPM (dTPM or PTT) mode \n", __func__);
    plugin_fp->cb_crypto_rsa_decrypt_init     = NULL;
    plugin_fp->cb_crypto_rsa_decrypt          = NULL;
    plugin_fp->cb_crypto_rsa_sign_init        = &tpm2_rsa_sign_init;
    plugin_fp->cb_crypto_rsa_sign             = &tpm2_rsa_sign;
    plugin_fp->cb_crypto_rsa_sign_update      = NULL;
    plugin_fp->cb_crypto_rsa_sign_final       = NULL;
    plugin_fp->cb_crypto_ecdsa_sign           = NULL;
    plugin_fp->cb_crypto_ecdsa_verify         = NULL;
    plugin_fp->cb_crypto_del_apphandle        = NULL;
    plugin_fp->cb_crypto_swk_getParentKey     = NULL;
    plugin_fp->cb_crypto_swk_import   	      = &tpm2_import_object;
    plugin_fp->cb_crypto_rsa_create_object    = &tpm2_rsa_create_object;
    plugin_fp->cb_crypto_rsa_delete_object    = &tpm2_rsa_delete_object;
    plugin_fp->cb_crypto_ecdsa_create_object  = NULL;
    plugin_fp->cb_crypto_ecdsa_delete_object  = NULL;

    return 0;
}

