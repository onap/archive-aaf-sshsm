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
#include "tpm2_plugin_api.h"
#include "plugin_register.h"

void main(void)
{
    unsigned long mechanish =1;
    void *param = NULL;
    size_t len = 100;
    void *ctx = NULL;
    
    unsigned char *msg;
    int msg_len;
    unsigned char *sig;
    int *sig_len;

    SSHSM_HW_PLUGIN_ACTIVATE_IN_INFO_t *activate_in_info;
    SSHSM_HW_PLUGIN_LOAD_KEY_IN_INFO_t *loadkey_in_info;
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
    tpm2_plugin_load_key(loadkey_in_info, keyHandle );
    
    printf("---------------------------------------------\n");
    tpm2_plugin_rsa_sign_init(mechanish, param, len, ctx);
    
    printf("---------------------------------------------\n");
    tpm2_plugin_rsa_sign(ctx, msg, msg_len, sig, sig_len);


}
