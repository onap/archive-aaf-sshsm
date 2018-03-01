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

#include <sapi/tpm20.h>
#include <stdbool.h>
#include <unistd.h>
#include "tpm2_plugin_api.h"
#include "log.h"
#include "tpm2_tcti_ldr.h"


const char *tcti_path="libtcti-device.so";

static void tcti_teardown (TSS2_TCTI_CONTEXT *tcti_context)
{

    Tss2_Tcti_Finalize (tcti_context);
    free (tcti_context);
}

static void sapi_teardown (TSS2_SYS_CONTEXT *sapi_context)
{

    if (sapi_context == NULL)
        return;
    Tss2_Sys_Finalize (sapi_context);
    free (sapi_context);
}

static void sapi_teardown_full (TSS2_SYS_CONTEXT *sapi_context)
{

    TSS2_TCTI_CONTEXT *tcti_context = NULL;
    TSS2_RC rc;

    rc = Tss2_Sys_GetTctiContext (sapi_context, &tcti_context);
    if (rc != TPM2_RC_SUCCESS)
        return;
    sapi_teardown (sapi_context);
    tcti_teardown (tcti_context);
}

static TSS2_SYS_CONTEXT* sapi_ctx_init(TSS2_TCTI_CONTEXT *tcti_ctx)
{

    TSS2_ABI_VERSION abi_version = {
            .tssCreator = TSSWG_INTEROP,
            .tssFamily = TSS_SAPI_FIRST_FAMILY,
            .tssLevel = TSS_SAPI_FIRST_LEVEL,
            .tssVersion = TSS_SAPI_FIRST_VERSION,
    };

    size_t size = Tss2_Sys_GetContextSize(0);
    TSS2_SYS_CONTEXT *sapi_ctx = (TSS2_SYS_CONTEXT*) calloc(1, size);
    if (sapi_ctx == NULL) {
        LOG_ERR("Failed to allocate 0x%zx bytes for the SAPI context\n",
                size);
        return NULL;
    }

    TSS2_RC rval = Tss2_Sys_Initialize(sapi_ctx, size, tcti_ctx, &abi_version);
    if (rval != TPM2_RC_SUCCESS) {
        LOG_PERR(Tss2_Sys_Initialize, rval);
        free(sapi_ctx);
        return NULL;
    }

    return sapi_ctx;
}
int tpm2_rsa_sign_init(
        unsigned long mechanish,
        void *param,
        size_t len,
        void *ctx)
{
    printf("executing tpm2_rsa_sign_init in tpm2_plugin... \n");
    return 0;
}

int tpm2_tool_sign(TSS2_SYS_CONTEXT *sapi_context)
{
    return 0;
}

int tpm2_rsa_sign(
        void  *ctx,
        unsigned char *msg,
        int msg_len,
        unsigned char *sig,
        int *sig_len)
{
    int ret = 1;
    TSS2_TCTI_CONTEXT *tcti;
    tcti = tpm2_tcti_ldr_load(tcti_path, NULL);
    if (!tcti) {
        LOG_ERR("Could not load tcti, got: \"%s\"", tcti_path);
        return -1;
    }
    
    TSS2_SYS_CONTEXT *sapi_context = NULL;
    if (tcti) {
        sapi_context = sapi_ctx_init(tcti);
        if (!sapi_context) {
            goto free_tcti;
        }
    }
    
    ret = tpm2_tool_sign(sapi_context);
    if (ret != 0) {
        LOG_ERR("Unable to run tpm2_tool_sign");
    sapi_teardown_full(sapi_context);

free_tcti:
    tpm2_tcti_ldr_unload();
    return ret;
}
}

int tpm2_rsa_create_object(
                        unsigned long appHandle,
                        //DhsmWPKRSAFormat* wpk,
                        void *wpk,
                        unsigned char* swk,
                        int swk_len,
                        unsigned char* iv,
                        int iv_len,
                        int tag_len,
                        void **cb_object)
{
    return 0;
}

int tpm2_rsa_delete_object(void *cb_object)
{
    return 0;
}

int tpm2_import_object(unsigned long appHandle, 
                       unsigned char* tlvbuffer, 
                       int buflen, 
                       unsigned char* iv, 
                       int iv_len, 
                       unsigned char* tpm_pwd, 
                       int tpm_pwd_len)

{
    return 0;
}
