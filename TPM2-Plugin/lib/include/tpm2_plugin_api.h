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

#ifndef __TPM_API_H__
#define __TPM_API_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <getopt.h>

#include <tss2/tss2_sys.h>

#include "plugin_register.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TPM_SKM_SRK_HANDLE 0x81000011

#define TPM_SKM_AC0_HANDLE 0x90000000
#define TPM_SKM_AC1_HANDLE 0x90000001
#define TPM_SKM_AC2_HANDLE 0x90000002

#define TPM_SKM_APP_HANDLE 0x91100001

#define INIT_SIMPLE_TPM2B_SIZE( type ) (type).t.size = sizeof( type ) - 2;

#define APP_RC_OFFSET 0x100

#define TSS2_APP_RC_PASSED                      (APP_RC_PASSED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_NAME_FAILED             (APP_RC_GET_NAME_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_CREATE_SESSION_KEY_FAILED   (APP_RC_CREATE_SESSION_KEY_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_SESSION_SLOT_NOT_FOUND      (APP_RC_SESSION_SLOT_NOT_FOUND + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_BAD_ALGORITHM               (APP_RC_BAD_ALGORITHM + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_SYS_CONTEXT_CREATE_FAILED   (APP_RC_SYS_CONTEXT_CREATE_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_SESSION_STRUCT_FAILED   (APP_RC_GET_SESSION_STRUCT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_SESSION_ALG_ID_FAILED   (APP_RC_GET_SESSION_ALG_ID_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_INIT_SYS_CONTEXT_FAILED     (APP_RC_INIT_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_TEARDOWN_SYS_CONTEXT_FAILED (APP_RC_TEARDOWN_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_BAD_LOCALITY                (APP_RC_BAD_LOCALITY + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)

enum TSS2_APP_RC_CODE
{
    APP_RC_PASSED,
    APP_RC_GET_NAME_FAILED,
    APP_RC_CREATE_SESSION_KEY_FAILED,
    APP_RC_SESSION_SLOT_NOT_FOUND,
    APP_RC_BAD_ALGORITHM,
    APP_RC_SYS_CONTEXT_CREATE_FAILED,
    APP_RC_GET_SESSION_STRUCT_FAILED,
    APP_RC_GET_SESSION_ALG_ID_FAILED,
    APP_RC_INIT_SYS_CONTEXT_FAILED,
    APP_RC_TEARDOWN_SYS_CONTEXT_FAILED,
    APP_RC_BAD_LOCALITY
};

TSS2_SYS_CONTEXT *InitSysContext (UINT16 maxCommandSize,
                                    TSS2_TCTI_CONTEXT *tctiContext,
                                    TSS2_ABI_VERSION *abiVersion );

void TeardownSysContext( TSS2_SYS_CONTEXT **sysContext );

TSS2_RC TeardownTctiResMgrContext( TSS2_TCTI_CONTEXT *tctiContext );

int tpm2_plugin_init();
int tpm2_plugin_uninit();
int tpm2_plugin_activate(SSHSM_HW_PLUGIN_ACTIVATE_IN_INFO_t *activate_in_info);
int tpm2_plugin_load_key(
           SSHSM_HW_PLUGIN_LOAD_KEY_IN_INFO_t *loadkey_in_info,
           void **keyHandle
        );

int tpm2_rsa_create_object(
                        unsigned long appHandle,
                        //DhsmWPKRSAFormat* wpk,
                        void *wpk,
                        unsigned char* swk,
                        int swk_len,
                        unsigned char* iv,
                        int iv_len,
                        int tag_len,
                        void **cb_object);

int tpm2_rsa_delete_object(
                        void *cb_object);

int tpm2_rsa_sign_init(
        unsigned long mechanish,
        void *param,
        size_t len,
        void *ctx);

int tpm2_rsa_sign(
        void *ctx,
        unsigned char *msg,
        int msg_len,
        unsigned char *sig,
        int *sig_len);


int tpm2_import_object(
        unsigned long appHandle, 
        unsigned char* tlvbuffer, 
        int buflen, 
        unsigned char* iv, 
        int iv_len, 
        unsigned char* tpm_pwd, 
        int tpm_pwd_len);


#ifdef __cplusplus
}
#endif


#endif
