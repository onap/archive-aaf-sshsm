/* Copyright 2018 Intel Corporation, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <stdio.h>

#include "tpm2_plugin_api.h"

int sshsm_hw_plugin_get_plugin_functions(SSHSM_HW_FUNCTIONS_t *funcs)
{
    printf("%s(): Assigning Function pointers for TPM (dTPM or PTT) mode \n", __func__);
    funcs->xxx_init             = &tpm2_plugin_init;
    funcs->xxx_uninit           = &tpm2_plugin_uninit;
    funcs->xxx_activate         = &tpm2_plugin_activate;
    funcs->xxx_load_key         = &tpm2_plugin_load_key;
    funcs->xxx_unload_key       = NULL;
    funcs->xxx_rsa_sign_init    = &tpm2_plugin_rsa_sign_init;
    funcs->xxx_rsa_sign_update  = &tpm2_plugin_rsa_sign_update;
    funcs->xxx_rsa_sign_final   = &tpm2_plugin_rsa_sign_final;
    funcs->xxx_rsa_sign         = &tpm2_plugin_rsa_sign;

    return 0;
}
