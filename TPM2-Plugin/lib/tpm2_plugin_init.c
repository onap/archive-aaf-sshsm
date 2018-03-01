//**********************************************************************;
// Copyright (c) 2017, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of Intel Corporation nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//**********************************************************************;

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

