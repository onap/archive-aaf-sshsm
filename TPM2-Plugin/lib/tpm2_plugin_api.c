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

#include <tss2/tss2_sys.h>
#include <stdbool.h>
#include <unistd.h>
#include "tpm2_plugin_api.h"
#include "tpm2_convert.h"
#include "tpm2_tcti_ldr.h"
#include "tpm2_tool.h"
#include "tpm2_hash.h"
#include "tpm2_alg_util.h"
#include "log.h"
#include "files.h"

bool output_enabled = true;

const char *tcti_path="libtss2-tcti-device.so";

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

#define SUPPORTED_ABI_VERSION \
{ \
    .tssCreator = 1, \
    .tssFamily = 2, \
    .tssLevel = 1, \
    .tssVersion = 108, \
}

static TSS2_SYS_CONTEXT* sapi_ctx_init(TSS2_TCTI_CONTEXT *tcti_ctx)
{

    TSS2_ABI_VERSION abi_version = SUPPORTED_ABI_VERSION;

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


int tpm2_plugin_init()
{
    printf("Init API done for TPM plugin ! \n");
    return 0;
}

int tpm2_plugin_uninit()
{
    printf("UnInit API done for TPM plugin ! \n");
    return 0;
}

TPM2_HANDLE srk_handle;
int tpm2_plugin_activate(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *activate_in_info)
{
    /*
    */
    char *handle;
    printf("number of buffers %d ! \n", activate_in_info->num_buffers);
    if (activate_in_info->num_buffers!=1){
        printf("activate failed ! \n");
        return 1;
    }
    printf("number of buffers %d ! \n", activate_in_info->num_buffers);
    handle = malloc(activate_in_info->buffer_info[0]->length_of_buffer);
    memcpy(handle, activate_in_info->buffer_info[0]->buffer, activate_in_info->buffer_info[0]->length_of_buffer);
    srk_handle = strtol(handle, NULL, 16);
    printf("Activate API done for TPM plugin ! \n");
    return 0;
}

TPM2_HANDLE handle_load;

typedef struct tpm_load_ctx tpm_load_ctx;
struct tpm_load_ctx {
    TPMS_AUTH_COMMAND session_data;
    TPMI_DH_OBJECT parent_handle;
    TPM2B_PUBLIC  in_public;
    TPM2B_PRIVATE in_private;
    char *out_file;
    char *context_file;
    char *context_parent_file;
    struct {
        UINT8 H : 1;
        UINT8 u : 1;
        UINT8 r : 1;
        UINT8 c : 1;
        UINT8 C : 1;
    } flags;
};

static tpm_load_ctx ctx_load = {
    .session_data = {
        .sessionHandle = TPM2_RS_PW,
        .nonce = TPM2B_EMPTY_INIT,
        .hmac = TPM2B_EMPTY_INIT,
        .sessionAttributes = 0
    }
};

int load (TSS2_SYS_CONTEXT *sapi_context) {
    UINT32 rval;
    TSS2L_SYS_AUTH_COMMAND sessionsData;
    TSS2L_SYS_AUTH_RESPONSE sessionsDataOut;

    TPM2B_NAME nameExt = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    sessionsData.count = 1;
    sessionsData.auths[0] = ctx_load.session_data;

    rval = TSS2_RETRY_EXP(Tss2_Sys_Load(sapi_context,
                         ctx_load.parent_handle,
                         &sessionsData,
                         &ctx_load.in_private,
                         &ctx_load.in_public,
                         &handle_load,
                         &nameExt,
                         &sessionsDataOut));
    if(rval != TPM2_RC_SUCCESS)
    {
        LOG_PERR(Tss2_Sys_Load, rval);
        return -1;
    }
    tpm2_tool_output("handle_load: 0x%08x\n", handle_load);

    if (ctx_load.out_file) {
        if(!files_save_bytes_to_file(ctx_load.out_file, nameExt.name, nameExt.size)) {
            return -2;
        }
    }

    return 0;
}

int tpm2_tool_load_key(TSS2_SYS_CONTEXT *sapi_context)
{

    int returnVal = 0;

    if ((!ctx_load.flags.H && !ctx_load.flags.c) || (!ctx_load.flags.u || !ctx_load.flags.r)) {
        LOG_ERR("Expected options (H or c) and u and r");
        return 1;
    }

    if(ctx_load.flags.c) {
        returnVal = files_load_tpm_context_from_path(sapi_context,
                                               &ctx_load.parent_handle,
                                               ctx_load.context_parent_file) != true;
        if (returnVal) {
            return 1;
        }
    }

    returnVal = load(sapi_context);
    if (returnVal) {
        return 1;
    }

    if (ctx_load.flags.C) {
        returnVal = files_save_tpm_context_to_path (sapi_context,
                                                    handle_load,
                                                    ctx_load.context_file) != true;
        if (returnVal) {
            return 1;
        }
    }

    return 0;
}

int tpm2_plugin_load_key(
           SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
           void **keyHandle
        )
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

    ret = tpm2_tool_load_key(sapi_context);
    if (ret != 0) {
        LOG_ERR("Unable to run tpm2_tool_iload_key");
    sapi_teardown_full(sapi_context);

free_tcti:
    tpm2_tcti_ldr_unload();
    return ret;
    }

    printf("Load key API done for TPM plugin ! \n");
    return 0;

}

typedef struct tpm_sign_ctx tpm_sign_ctx;
struct tpm_sign_ctx {
    TPMT_TK_HASHCHECK validation;
    TPMS_AUTH_COMMAND sessionData;
    TPMI_DH_OBJECT keyHandle;
    TPMI_ALG_HASH halg;
    TPM2B_DIGEST digest;
    char *outFilePath;
    BYTE *msg;
    UINT16 length;
    char *contextKeyFile;
    char *inMsgFileName;
    tpm2_convert_sig_fmt sig_format;
    struct {
        UINT16 k : 1;
        UINT16 P : 1;
        UINT16 g : 1;
        UINT16 m : 1;
        UINT16 t : 1;
        UINT16 s : 1;
        UINT16 c : 1;
        UINT16 f : 1;
        UINT16 D : 1;
    } flags;
};

tpm_sign_ctx ctx_sign = {
        .msg = NULL,
        .sessionData = TPMS_AUTH_COMMAND_INIT(TPM2_RS_PW),
        .halg = TPM2_ALG_SHA1,
        .digest = TPM2B_TYPE_INIT(TPM2B_DIGEST, buffer),
};


int tpm2_plugin_rsa_sign_init(
        void *keyHandle,
        unsigned long mechanish,
        void *param,
        int len)
{
    printf("rsa_sign_init API done for tpm2_plugin... \n");
    return 0;
}

static bool init_sign(TSS2_SYS_CONTEXT *sapi_context) {

    if (!((ctx_sign.flags.k || ctx_sign.flags.c) && (ctx_sign.flags.m || ctx_sign.flags.D) && ctx_sign.flags.s)) {
        LOG_ERR("Expected options (k or c) and (m or D) and s");
        return false;
    }

    if (ctx_sign.flags.D && (ctx_sign.flags.t || ctx_sign.flags.m)) {
        LOG_WARN("Option D provided, options m and t are ignored.");
    }

    if (ctx_sign.flags.D || !ctx_sign.flags.t) {
        ctx_sign.validation.tag = TPM2_ST_HASHCHECK;
        ctx_sign.validation.hierarchy = TPM2_RH_NULL;
        memset(&ctx_sign.validation.digest, 0, sizeof(ctx_sign.validation.digest));
    }

    /*
     * load tpm context from a file if -c is provided
     */
    if (ctx_sign.flags.c) {
        bool result = files_load_tpm_context_from_path(sapi_context, &ctx_sign.keyHandle,
                ctx_sign.contextKeyFile);
        if (!result) {
            return false;
        }
    }

    /*
     * Process the msg file if needed
     */
    if (ctx_sign.flags.m && !ctx_sign.flags.D) {
      unsigned long file_size;
      bool result = files_get_file_size_path(ctx_sign.inMsgFileName, &file_size);
      if (!result) {
          return false;
      }
      if (file_size == 0) {
          LOG_ERR("The message file \"%s\" is empty!", ctx_sign.inMsgFileName);
          return false;
      }

      if (file_size > UINT16_MAX) {
          LOG_ERR(
                  "The message file \"%s\" is too large, got: %lu bytes, expected less than: %u bytes!",
                  ctx_sign.inMsgFileName, file_size, UINT16_MAX + 1);
          return false;
      }

      ctx_sign.msg = (BYTE*) calloc(required_argument, file_size);
      if (!ctx_sign.msg) {
          LOG_ERR("oom");
          return false;
      }

      ctx_sign.length = file_size;
      result = files_load_bytes_from_path(ctx_sign.inMsgFileName, ctx_sign.msg, &ctx_sign.length);
      if (!result) {
          free(ctx_sign.msg);
          return false;
      }
    }

    return true;
}


static bool sign_and_save(TSS2_SYS_CONTEXT *sapi_context) {

    TPMT_SIG_SCHEME in_scheme;
    TPMT_SIGNATURE signature;

    TSS2L_SYS_AUTH_COMMAND sessions_data = { 1, { ctx_sign.sessionData }};
    TSS2L_SYS_AUTH_RESPONSE sessions_data_out;

    if (!ctx_sign.flags.D) {
      bool res = tpm2_hash_compute_data(sapi_context, ctx_sign.halg, TPM2_RH_NULL,
              ctx_sign.msg, ctx_sign.length, &ctx_sign.digest, NULL);
      if (!res) {
          LOG_ERR("Compute message hash failed!");
          return false;
      }
    }

    bool result = get_signature_scheme(sapi_context, ctx_sign.keyHandle, ctx_sign.halg, &in_scheme);
    if (!result) {
        return false;
    }

    TSS2_RC rval = TSS2_RETRY_EXP(Tss2_Sys_Sign(sapi_context, ctx_sign.keyHandle,
            &sessions_data, &ctx_sign.digest, &in_scheme, &ctx_sign.validation, &signature,
            &sessions_data_out));
    if (rval != TPM2_RC_SUCCESS) {
        LOG_PERR(Tss2_Sys_Sign, rval);
        return false;
    }

    return tpm2_convert_sig(&signature, ctx_sign.sig_format, ctx_sign.outFilePath);
}


int tpm2_tool_sign(TSS2_SYS_CONTEXT *sapi_context)
{

    bool result = init_sign(sapi_context);
    if (!result) {
        return 1;
    }

    result = sign_and_save(sapi_context);

    free(ctx_sign.msg);

    return result != true;
}


int tpm2_plugin_rsa_sign(
        void  *keyHandle,
        unsigned long mechanism,
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
    printf("rsa_sign API done for tpm2_plugin... \n");
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
