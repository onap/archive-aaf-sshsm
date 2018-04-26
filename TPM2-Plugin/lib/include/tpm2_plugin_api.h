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


//#define HAVE_TCTI_DEV 1
#define HAVE_TCTI_TABRMD 1
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

#define MAX_DATA_SIGNUPDATE  0x2000
#define MAX_SESSIONS  0x1000

typedef struct concatenate_data_signupdate {
    unsigned long int session_handle;
    unsigned char data_signupdate[MAX_DATA_SIGNUPDATE];
    int data_length;
}CONCATENATE_DATA_SIGNUPDATE_t;

int tpm2_plugin_init();
int tpm2_plugin_uninit();
int tpm2_plugin_activate(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *activate_in_info);
int tpm2_plugin_load_key(
        SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
        void **keyHandle
        );

int tpm2_plugin_rsa_sign_init(
        void *keyHandle,
        unsigned long mechanism,
        void *param,
        int len,
        void **plugin_data_ref
        );

int tpm2_plugin_rsa_sign(
        void *keyHandle,
        unsigned long mechanism,
        unsigned char *msg,
        int msg_len,
        void *plugin_data_ref,
        unsigned char *sig,
        int *sig_len
        );

int tpm2_plugin_rsa_sign_update(
        void *keyHandle,
        unsigned long mechnaism,
        unsigned char *msg,
        int msg_len,
        void *plugin_data_ref
        );

int tpm2_plugin_rsa_sign_final(
        void *keyHandle,
        unsigned long mechnaism,
        void *plugin_data_ref,
        unsigned char *outsig,
        int *outsiglen
        );

/** This function is called by SSHSM only if there sign_final function is not called.
If sign_final function is called, it is assumed that plugin would have cleaned this up.
***/

typedef int (*sshsm_hw_plugin_rsa_sign_cleanup)(
        void *keyHandle,
        unsigned long mechnaism,
        void *plugin_data_ref
        );


#ifdef __cplusplus
}
#endif


#endif
