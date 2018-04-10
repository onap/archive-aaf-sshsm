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

#include <sapi/tpm20.h>
#include "hwpluginif.h"

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


#define HAVE_TCTI_DEV 1
//#define TCTI_DEFAULT HAVE_TCTI_DEV
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


#ifdef HAVE_TCTI_TABRMD
  #define TCTI_DEFAULT      TABRMD_TCTI
  #define TCTI_DEFAULT_STR  "tabrmd"
#elif HAVE_TCTI_SOCK
  #define TCTI_DEFAULT      SOCKET_TCTI
  #define TCTI_DEFAULT_STR  "socket"
#elif  HAVE_TCTI_DEV
  #define TCTI_DEFAULT      DEVICE_TCTI
  #define TCTI_DEFAULT_STR  "device"
#endif


/* Defaults for Device TCTI */
#define TCTI_DEVICE_DEFAULT_PATH "/dev/tpm0"

/* Deafults for Socket TCTI connections, port default is for resourcemgr */
#define TCTI_SOCKET_DEFAULT_ADDRESS "127.0.0.1"
#define TCTI_SOCKET_DEFAULT_PORT     2321

/* Environment variables usable as alternatives to command line options */
#define TPM2TOOLS_ENV_TCTI_NAME      "TPM2TOOLS_TCTI_NAME"
#define TPM2TOOLS_ENV_DEVICE_FILE    "TPM2TOOLS_DEVICE_FILE"
#define TPM2TOOLS_ENV_SOCKET_ADDRESS "TPM2TOOLS_SOCKET_ADDRESS"
#define TPM2TOOLS_ENV_SOCKET_PORT    "TPM2TOOLS_SOCKET_PORT"

#define COMMON_OPTS_INITIALIZER { \
    .tcti_type      = TCTI_DEFAULT, \
    .device_file    = TCTI_DEVICE_DEFAULT_PATH, \
    .socket_address = TCTI_SOCKET_DEFAULT_ADDRESS, \
    .socket_port    = TCTI_SOCKET_DEFAULT_PORT, \
    .help           = false, \
    .verbose        = false, \
    .version        = false, \
}

typedef enum {
#ifdef HAVE_TCTI_DEV
    DEVICE_TCTI,
#endif
#ifdef HAVE_TCTI_SOCK
    SOCKET_TCTI,
#endif
#ifdef HAVE_TCTI_TABRMD
    TABRMD_TCTI,
#endif
    UNKNOWN_TCTI,
    N_TCTI,
} TCTI_TYPE;

typedef struct {
    TCTI_TYPE tcti_type;
    char     *device_file;
    char     *socket_address;
    uint16_t  socket_port;
    int       help;
    int       verbose;
    int       version;
} common_opts_t;

int tpm2_plugin_init();
int tpm2_plugin_uninit();
int tpm2_plugin_activate(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *activate_in_info);
int tpm2_plugin_load_key(
           SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
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

int tpm2_plugin_rsa_sign_init(
        void *keyHandle,
        unsigned long mechanism,
        void *param,
        int len);

int tpm2_plugin_rsa_sign(
        void *keyHandle,
        unsigned long mechanism,
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
