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
//#include "plugin_register.h"
//#include "hwpluginif.h"

void main(void)
{
    unsigned long mechanism =1;
    void *param = NULL;
    size_t len = 100;
    void *keyHandle_sign = NULL;
    unsigned long int hSession = 1;

    unsigned char *msg;
    int msg_len;
    unsigned char *sig;
    int *sig_len;

    SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *activate_in_info;
    activate_in_info = malloc(sizeof(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t));
    SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info;
    loadkey_in_info = malloc(sizeof(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t));
    loadkey_in_info->num_buffers = 2;
    unsigned char *str ="abcde";
    //loadkey_in_info->buffer_info[0]->buffer = str;
    //loadkey_in_info->buffer_info[0]->length_of_buffer = 5;
    //loadkey_in_info->buffer_info[1]->buffer = str;
    //loadkey_in_info->buffer_info[1]->length_of_buffer = 5;

    void **keyHandle;

    printf("---------------------------------------------\n");
    printf("Test app calling tpm2_plugin APIs\n");

    printf("---------------------------------------------\n");
    tpm2_plugin_init();

    printf("---------------------------------------------\n");
    tpm2_plugin_uninit();

    printf("---------------------------------------------\n");
    tpm2_plugin_activate(activate_in_info);

    printf("---------------------------------------------\n");
    tpm2_plugin_rsa_sign_init(hSession, keyHandle_sign, mechanism, param, len);

    printf("---------------------------------------------\n");
    tpm2_plugin_load_key(hSession, loadkey_in_info, keyHandle);

    printf("---------------------------------------------\n");
    tpm2_plugin_rsa_sign(hSession, keyHandle_sign, mechanism, msg, msg_len, sig, sig_len);

}
