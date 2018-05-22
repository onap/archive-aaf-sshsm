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

#include <sapi/tpm20.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include "tpm2_plugin_api.h"
#ifdef HAVE_TCTI_DEV
#include <tcti/tcti_device.h>
#endif
#ifdef HAVE_TCTI_SOCK
#include <tcti/tcti_socket.h>
#endif
#ifdef HAVE_TCTI_TABRMD
#include <tcti/tcti-tabrmd.h>
#endif
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

bool output_enabled = true;
bool hexPasswd = false;
TPM_HANDLE handle2048rsa;
const char *tcti_path="libtss2-tcti-device.so";

static void tcti_teardown(TSS2_TCTI_CONTEXT *tcti_context)
{
    if (tcti_context == NULL)
        return;
    tss2_tcti_finalize (tcti_context);
    free (tcti_context);
}

static void sapi_teardown(TSS2_SYS_CONTEXT *sapi_context)
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
    if (rc != TSS2_RC_SUCCESS)
        return;
    sapi_teardown (sapi_context);
    tcti_teardown (tcti_context);
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

TPM_HANDLE srk_handle;
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

TPMI_DH_OBJECT handle_load;

#ifdef HAVE_TCTI_DEV
TSS2_TCTI_CONTEXT*
tcti_device_init (char const *device_file)
{
    TCTI_DEVICE_CONF conf = {
        .device_path = device_file,
        .logCallback = NULL,
        .logData     = NULL,
    };
    size_t size;
    TSS2_RC rc;
    TSS2_TCTI_CONTEXT *tcti_ctx;

    rc = InitDeviceTcti (NULL, &size, 0);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr,
                 "Failed to get allocation size for device tcti context: "
                 "0x%x\n", rc);
        return NULL;
    }
    tcti_ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (tcti_ctx == NULL) {
        fprintf (stderr,
                 "Allocation for device TCTI context failed: %s\n",
                 strerror (errno));
        return NULL;
    }
    rc = InitDeviceTcti (tcti_ctx, &size, &conf);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr,
                 "Failed to initialize device TCTI context: 0x%x\n",
                 rc);
        free (tcti_ctx);
        return NULL;
    }
    return tcti_ctx;
}
#endif


#ifdef HAVE_TCTI_SOCK
TSS2_TCTI_CONTEXT* tcti_socket_init (char const *address, uint16_t port)
{
    TCTI_SOCKET_CONF conf = {
        .hostname          = address,
        .port              = port,
        .logCallback       = NULL,
        .logBufferCallback = NULL,
        .logData           = NULL,
    };
    size_t size;
    TSS2_RC rc;
    TSS2_TCTI_CONTEXT *tcti_ctx;

    rc = InitSocketTcti (NULL, &size, &conf, 0);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr, "Faled to get allocation size for tcti context: "
                 "0x%x\n", rc);
        return NULL;
    }
    tcti_ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (tcti_ctx == NULL) {
        fprintf (stderr, "Allocation for tcti context failed: %s\n",
                 strerror (errno));
        return NULL;
    }
    rc = InitSocketTcti (tcti_ctx, &size, &conf, 0);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr, "Failed to initialize tcti context: 0x%x\n", rc);
        free (tcti_ctx);
        return NULL;
    }
    return tcti_ctx;
}
#endif
#ifdef HAVE_TCTI_TABRMD
TSS2_TCTI_CONTEXT *tcti_tabrmd_init (void)
{
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;
    size_t size;

    rc = tss2_tcti_tabrmd_init(NULL, &size);
    if (rc != TSS2_RC_SUCCESS) {
        printf("Failed to get size for TABRMD TCTI context: 0x%x", rc);
        return NULL;
    }
    tcti_ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (tcti_ctx == NULL) {
        printf("Allocation for TABRMD TCTI context failed: %s", strerror (errno));
        return NULL;
    }
    rc = tss2_tcti_tabrmd_init (tcti_ctx, &size);
    if (rc != TSS2_RC_SUCCESS) {
        printf("Failed to initialize TABRMD TCTI context: 0x%x", rc);
        free(tcti_ctx);
        return NULL;
    }
    return tcti_ctx;
}
#endif
TSS2_TCTI_CONTEXT *tcti_init_from_options(common_opts_t *options)
{
    switch (options->tcti_type) {
#ifdef HAVE_TCTI_DEV
    case DEVICE_TCTI:
        return tcti_device_init (options->device_file);
#endif
#ifdef HAVE_TCTI_SOCK
    case SOCKET_TCTI:
        return tcti_socket_init (options->socket_address,
                                 options->socket_port);
#endif
#ifdef HAVE_TCTI_TABRMD
    case TABRMD_TCTI:
        return tcti_tabrmd_init ();
#endif
    default:
        return NULL;
    }
}

static TSS2_SYS_CONTEXT *sapi_ctx_init (TSS2_TCTI_CONTEXT *tcti_ctx)
{
    TSS2_SYS_CONTEXT *sapi_ctx;
    TSS2_RC rc;
    size_t size;
    TSS2_ABI_VERSION abi_version = {
        .tssCreator = TSSWG_INTEROP,
        .tssFamily  = TSS_SAPI_FIRST_FAMILY,
        .tssLevel   = TSS_SAPI_FIRST_LEVEL,
        .tssVersion = TSS_SAPI_FIRST_VERSION,
    };

    size = Tss2_Sys_GetContextSize (0);
    sapi_ctx = (TSS2_SYS_CONTEXT*)calloc (1, size);
    if (sapi_ctx == NULL) {
        fprintf (stderr,
                 "Failed to allocate 0x%zx bytes for the SAPI context\n",
                 size);
        return NULL;
    }
    rc = Tss2_Sys_Initialize (sapi_ctx, size, tcti_ctx, &abi_version);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr, "Failed to initialize SAPI context: 0x%x\n", rc);
        free (sapi_ctx);
        return NULL;
    }
    return sapi_ctx;
}

#define BUFFER_SIZE(type, field) (sizeof((((type *)NULL)->t.field)))
#define TPM2B_TYPE_INIT(type, field) { .t = { .size = BUFFER_SIZE(type, field), }, }
TPMS_AUTH_COMMAND sessionData;
int hex2ByteStructure(const char *inStr, UINT16 *byteLength, BYTE *byteBuffer)
{
    int strLength;//if the inStr likes "1a2b...", no prefix "0x"
    int i = 0;
    if(inStr == NULL || byteLength == NULL || byteBuffer == NULL)
        return -1;
    strLength = strlen(inStr);
    if(strLength%2)
        return -2;
    for(i = 0; i < strLength; i++)
    {
        if(!isxdigit(inStr[i]))
            return -3;
    }

    if(*byteLength < strLength/2)
        return -4;

    *byteLength = strLength/2;

    for(i = 0; i < *byteLength; i++)
    {
        char tmpStr[4] = {0};
        tmpStr[0] = inStr[i*2];
        tmpStr[1] = inStr[i*2+1];
        byteBuffer[i] = strtol(tmpStr, NULL, 16);
    }
    return 0;
}
int load_key(TSS2_SYS_CONTEXT *sapi_context,
             TPMI_DH_OBJECT    parentHandle,
             TPM2B_PUBLIC     *inPublic,
             TPM2B_PRIVATE    *inPrivate,
             int               P_flag)
{
    UINT32 rval;
    TPMS_AUTH_RESPONSE sessionDataOut;
    TSS2_SYS_CMD_AUTHS sessionsData;
    TSS2_SYS_RSP_AUTHS sessionsDataOut;
    TPMS_AUTH_COMMAND *sessionDataArray[1];
    TPMS_AUTH_RESPONSE *sessionDataOutArray[1];

    TPM2B_NAME nameExt = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    sessionDataArray[0] = &sessionData;
    sessionDataOutArray[0] = &sessionDataOut;

    sessionsDataOut.rspAuths = &sessionDataOutArray[0];
    sessionsData.cmdAuths = &sessionDataArray[0];

    sessionsDataOut.rspAuthsCount = 1;
    sessionsData.cmdAuthsCount = 1;

    sessionData.sessionHandle = TPM_RS_PW;
    sessionData.nonce.t.size = 0;

    if(P_flag == 0)
        sessionData.hmac.t.size = 0;

    *((UINT8 *)((void *)&sessionData.sessionAttributes)) = 0;
    if (sessionData.hmac.t.size > 0 && hexPasswd)
    {
        sessionData.hmac.t.size = sizeof(sessionData.hmac) - 2;
        if (hex2ByteStructure((char *)sessionData.hmac.t.buffer,
                              &sessionData.hmac.t.size,
                              sessionData.hmac.t.buffer) != 0)
        {
            printf( "Failed to convert Hex format password for parent Passwd.\n");
            return -1;
        }
    }

    rval = Tss2_Sys_Load (sapi_context,
                          parentHandle,
                          &sessionsData,
                          inPrivate,
                          inPublic,
                          &handle2048rsa,
                          &nameExt,
                          &sessionsDataOut);
    if(rval != TPM_RC_SUCCESS)
    {
        printf("\nLoad Object Failed ! ErrorCode: 0x%0x\n\n",rval);
        return -1;
    }
    printf("\nLoad succ.\nLoadedHandle: 0x%08x\n\n",handle2048rsa);

    return 0;
}

int read_public(TSS2_SYS_CONTEXT *sapi_context,
                TPM_HANDLE handle,
                SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info)
{

    TPMS_AUTH_RESPONSE session_out_data;
    TSS2_SYS_RSP_AUTHS sessions_out_data;
    TPMS_AUTH_RESPONSE *session_out_data_array[1];

    TPM2B_PUBLIC public = {
            { 0, }
    };

    TPM2B_NAME name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2B_NAME qualified_name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    session_out_data_array[0] = &session_out_data;
    sessions_out_data.rspAuths = &session_out_data_array[0];
    sessions_out_data.rspAuthsCount = ARRAY_LEN(session_out_data_array);

    TPM_RC rval = Tss2_Sys_ReadPublic(sapi_context, handle, 0,
            &public, &name, &qualified_name, &sessions_out_data);
    if (rval != TPM_RC_SUCCESS) {
        printf("TPM2_ReadPublic error: rval = 0x%0x", rval);
        return false;
    }

    printf("\nTPM2_ReadPublic OutPut: \n");
    printf("name: \n");
    UINT16 i;
    for (i = 0; i < name.t.size; i++)
        printf("%02x ", name.t.name[i]);
    printf("\n");

    printf("qualified_name: \n");
    for (i = 0; i < qualified_name.t.size; i++)
        printf("%02x ", qualified_name.t.name[i]);
    printf("\n");

    printf("public.t.publicArea.parameters.rsaDetail.keyBits = %d \n", public.t.publicArea.parameters.rsaDetail.keyBits);
    printf("public.t.publicArea.parameters.rsaDetail.exponent = %d \n", public.t.publicArea.parameters.rsaDetail.exponent);

    importkey_info->modulus_size = public.t.publicArea.unique.rsa.t.size;
    printf("importkey_info->modulus_size = %ld \n", importkey_info->modulus_size);
    importkey_info->modulus = (unsigned char *) malloc(importkey_info->modulus_size);
    if (importkey_info->modulus != NULL) {
        memcpy(importkey_info->modulus, &public.t.publicArea.unique.rsa.t.buffer, importkey_info->modulus_size);
    }

    importkey_info->exponent_size = sizeof(public.t.publicArea.parameters.rsaDetail.exponent);
    printf("importkey_info->exponent_size = %ld \n", importkey_info->exponent_size);
    importkey_info->exponent = (unsigned char *) malloc(importkey_info->exponent_size);
    if (importkey_info->exponent != NULL) {
        memcpy(importkey_info->exponent, &public.t.publicArea.parameters.rsaDetail.exponent, importkey_info->exponent_size);
    }

    //*importkey_info->exponent = public.t.publicArea.parameters.rsaDetail.exponent;

    return 0;
}

TPMS_CONTEXT loaded_key_context;

int load_key_execute(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
                     void **keyHandle, TSS2_SYS_CONTEXT *sapi_context,
                     SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info)
{

    TPMI_DH_OBJECT parentHandle;
    TPM2B_PUBLIC  inPublic;
    TPM2B_PRIVATE inPrivate;
    UINT16 size;
    int returnVal = 0;

    memset(&inPublic,0,sizeof(TPM2B_PUBLIC));
    memset(&inPrivate,0,sizeof(TPM2B_SENSITIVE));

    setbuf(stdout, NULL);
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);

    //parentHandle = 0x81000011;
    parentHandle = srk_handle;

    if (loadkey_in_info->num_buffers != 2)
        return -1;
    memcpy(&inPublic, loadkey_in_info->buffer_info[0]->buffer,
           loadkey_in_info->buffer_info[0]->length_of_buffer);
    memcpy(&inPrivate, loadkey_in_info->buffer_info[1]->buffer,
           loadkey_in_info->buffer_info[1]->length_of_buffer);

    returnVal = load_key (sapi_context,
                          parentHandle,
                          &inPublic,
                          &inPrivate,
                          0);
    returnVal = read_public(sapi_context,
                            handle2048rsa,
                            importkey_info);

    TPM_RC rval = Tss2_Sys_ContextSave(sapi_context, handle2048rsa, &loaded_key_context);
    if (rval != TPM_RC_SUCCESS) {
        printf("Tss2_Sys_ContextSave: Saving handle 0x%x context failed. TPM Error:0x%x", handle2048rsa, rval);
        return -1;
    }
    *keyHandle = &handle2048rsa;
    return 0;
}

int tpm2_plugin_load_key(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
                         void **keyHandle,
                         SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info)
{
    int ret = 1;
    common_opts_t opts = COMMON_OPTS_INITIALIZER;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    tcti_ctx = tcti_init_from_options(&opts);
    if (tcti_ctx == NULL)
        return -1;

    TSS2_SYS_CONTEXT *sapi_context = NULL;
    if (tcti_ctx) {
        sapi_context = sapi_ctx_init(tcti_ctx);
        if (!sapi_context) {
            free(tcti_ctx);
            return -1;
        }
    }

    ret = load_key_execute(loadkey_in_info, keyHandle, sapi_context, importkey_info);
    if (ret !=0)
        printf("Load key API failed in TPM plugin ! \n");

    sapi_teardown_full(sapi_context);

    printf("Load key API successful in TPM plugin ! \n");
    return 0;

}

typedef struct tpm_sign_ctx tpm_sign_ctx;
struct tpm_sign_ctx {
    TPMT_TK_HASHCHECK validation;
    TPMS_AUTH_COMMAND sessionData;
    TPMI_DH_OBJECT keyHandle;
    TPMI_ALG_HASH halg;
    char outFilePath[PATH_MAX];
    BYTE *msg;
    UINT16 length;
    TSS2_SYS_CONTEXT *sapi_context;
};

int tpm2_plugin_rsa_sign_init(
        void *keyHandle,
        unsigned long mechanism,
        void *param,
        int len)
{
    printf("rsa_sign_init API mechanism is %lx \n", mechanism);
    printf("rsa_sign_init API done for tpm2_plugin... \n");
    return 0;
}

UINT32 tpm_hash(TSS2_SYS_CONTEXT *sapi_context, TPMI_ALG_HASH hashAlg,
        UINT16 size, BYTE *data, TPM2B_DIGEST *result) {
    TPM2B_MAX_BUFFER dataSizedBuffer;

    dataSizedBuffer.t.size = size;
    memcpy(dataSizedBuffer.t.buffer, data, size);
    return Tss2_Sys_Hash(sapi_context, 0, &dataSizedBuffer, hashAlg,
            TPM_RH_NULL, result, 0, 0);
}

static TPM_RC hash_sequence_ex(TSS2_SYS_CONTEXT *sapi_context,

    TPMI_ALG_HASH hashAlg, UINT32 numBuffers, TPM2B_MAX_BUFFER *bufferList,
    TPM2B_DIGEST *result) {
    TPM_RC rval;
    TPM2B_AUTH nullAuth;
    TPMI_DH_OBJECT sequenceHandle;
    TPM2B emptyBuffer;
    TPMT_TK_HASHCHECK validation;

    TPMS_AUTH_COMMAND cmdAuth;
    TPMS_AUTH_COMMAND *cmdSessionArray[1] = { &cmdAuth };
    TSS2_SYS_CMD_AUTHS cmdAuthArray = { 1, &cmdSessionArray[0] };

    nullAuth.t.size = 0;
    emptyBuffer.size = 0;

    // Set result size to 0, in case any errors occur
    result->b.size = 0;

    // Init input sessions struct
    cmdAuth.sessionHandle = TPM_RS_PW;
    cmdAuth.nonce.t.size = 0;
    *((UINT8 *) ((void *) &cmdAuth.sessionAttributes)) = 0;
    cmdAuth.hmac.t.size = 0;

    rval = Tss2_Sys_HashSequenceStart(sapi_context, 0, &nullAuth, hashAlg,
            &sequenceHandle, 0);
    if (rval != TPM_RC_SUCCESS) {
        return rval;
    }

    unsigned i;
    for (i = 0; i < numBuffers; i++) {
        rval = Tss2_Sys_SequenceUpdate(sapi_context, sequenceHandle,
                &cmdAuthArray, &bufferList[i], 0);

        if (rval != TPM_RC_SUCCESS) {
            return rval;
        }
    }

    rval = Tss2_Sys_SequenceComplete(sapi_context, sequenceHandle,
            &cmdAuthArray, (TPM2B_MAX_BUFFER *) &emptyBuffer,
            TPM_RH_PLATFORM, result, &validation, 0);

    if (rval != TPM_RC_SUCCESS) {
        return rval;
    }

    return rval;
}

int tpm_hash_compute_data(TSS2_SYS_CONTEXT *sapi_context, BYTE *buffer,
        UINT16 length, TPMI_ALG_HASH halg, TPM2B_DIGEST *result) {

    if (length <= MAX_DIGEST_BUFFER) {
        if (tpm_hash(sapi_context, halg, length, buffer,
                result) == TPM_RC_SUCCESS)
            return 0;
        else
            return -1;
    }

    UINT8 numBuffers = (length - 1) / MAX_DIGEST_BUFFER + 1;

    TPM2B_MAX_BUFFER *bufferList = (TPM2B_MAX_BUFFER *) calloc(numBuffers,
            sizeof(TPM2B_MAX_BUFFER));
    if (bufferList == NULL)
        return -2;

    UINT32 i;
    for (i = 0; i < (UINT32)(numBuffers - 1); i++) {
        bufferList[i].t.size = MAX_DIGEST_BUFFER;
        memcpy(bufferList[i].t.buffer, buffer + i * MAX_DIGEST_BUFFER,
                MAX_DIGEST_BUFFER);
    }
    bufferList[i].t.size = length - i * MAX_DIGEST_BUFFER;
    memcpy(bufferList[i].t.buffer, buffer + i * MAX_DIGEST_BUFFER,
            bufferList[i].t.size);

    TPM_RC rval = hash_sequence_ex(sapi_context, halg, numBuffers, bufferList, result);
    free(bufferList);
    return rval == TPM_RC_SUCCESS ? 0 : -3;
}


static bool get_key_type(TSS2_SYS_CONTEXT *sapi_context, TPMI_DH_OBJECT objectHandle,
        TPMI_ALG_PUBLIC *type) {

    TPMS_AUTH_RESPONSE session_data_out;

    TPMS_AUTH_RESPONSE *session_data_out_array[1] = {
            &session_data_out
    };

    TSS2_SYS_RSP_AUTHS sessions_data_out = {
            1,
            &session_data_out_array[0]
    };

    TPM2B_PUBLIC out_public = {
            { 0, }
    };

    TPM2B_NAME name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2B_NAME qaulified_name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM_RC rval = Tss2_Sys_ReadPublic(sapi_context, objectHandle, 0, &out_public, &name,
            &qaulified_name, &sessions_data_out);
    if (rval != TPM_RC_SUCCESS) {
        printf("Sys_ReadPublic failed, error code: 0x%x", rval);
        return false;
    }
    *type = out_public.t.publicArea.type;
    return true;
}

static bool set_scheme(TSS2_SYS_CONTEXT *sapi_context, TPMI_DH_OBJECT keyHandle,
        TPMI_ALG_HASH halg, TPMT_SIG_SCHEME *inScheme) {

    TPM_ALG_ID type;
    bool result = get_key_type(sapi_context, keyHandle, &type);
    if (!result) {
        return false;
    }

    switch (type) {
    case TPM_ALG_RSA :
        inScheme->scheme = TPM_ALG_RSASSA;
        inScheme->details.rsassa.hashAlg = halg;
        break;
    case TPM_ALG_KEYEDHASH :
        inScheme->scheme = TPM_ALG_HMAC;
        inScheme->details.hmac.hashAlg = halg;
        break;
    case TPM_ALG_ECC :
        inScheme->scheme = TPM_ALG_ECDSA;
        inScheme->details.ecdsa.hashAlg = halg;
        break;
    case TPM_ALG_SYMCIPHER :
    default:
        printf("Unknown key type, got: 0x%x", type);
        return false;
    }

    return true;
}
static bool sign_and_save(tpm_sign_ctx *ctx,  unsigned char *sig, int *sig_len) {
    TPM2B_DIGEST digest = TPM2B_TYPE_INIT(TPM2B_DIGEST, buffer);

    TPMT_SIG_SCHEME in_scheme;
    TPMT_SIGNATURE signature;
    int signature_len;
    TSS2_SYS_CMD_AUTHS sessions_data;
    TPMS_AUTH_RESPONSE session_data_out;
    TSS2_SYS_RSP_AUTHS sessions_data_out;
    TPMS_AUTH_COMMAND *session_data_array[1];
    TPMS_AUTH_RESPONSE *session_data_out_array[1];

    session_data_array[0] = &ctx->sessionData;
    sessions_data.cmdAuths = &session_data_array[0];
    session_data_out_array[0] = &session_data_out;
    sessions_data_out.rspAuths = &session_data_out_array[0];
    sessions_data_out.rspAuthsCount = 1;
    sessions_data.cmdAuthsCount = 1;

    int rc = tpm_hash_compute_data(ctx->sapi_context, ctx->msg, ctx->length, ctx->halg, &digest);
    if (rc) {
        printf("Compute message hash failed!");
        return false;
    }

    bool result = set_scheme(ctx->sapi_context, ctx->keyHandle, ctx->halg, &in_scheme);
    if (!result) {
        return false;
    }

    TPM_RC rval = Tss2_Sys_Sign(ctx->sapi_context, ctx->keyHandle,
                                &sessions_data, &digest, &in_scheme,
                                &ctx->validation, &signature,
                                &sessions_data_out);

    if (rval != TPM_RC_SUCCESS) {
        printf("Sys_Sign failed, error code: 0x%x", rval);
        return false;
    }
    signature_len = sizeof(signature);
    sig_len = &signature_len;
    sig = (unsigned char *)&signature;

    return true;
}

int tpm2_plugin_rsa_sign(
        void  *keyHandle,
        unsigned long mechanism,
        unsigned char *msg,
        int msg_len,
        unsigned char *sig,
        int *sig_len)
{
    TPM_RC rval;
    common_opts_t opts = COMMON_OPTS_INITIALIZER;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    tcti_ctx = tcti_init_from_options(&opts);
    if (tcti_ctx == NULL)
        return -1;

    TSS2_SYS_CONTEXT *sapi_context = NULL;
    if (tcti_ctx) {
       sapi_context = sapi_ctx_init(tcti_ctx);
       if (!sapi_context) {
           free(tcti_ctx);
           return -1;
       }
    }

    tpm_sign_ctx ctx = {
            .msg = NULL,
            .sessionData = { 0 },
            .halg = 0,
            .keyHandle = 0,
            .validation = { 0 },
            .sapi_context = sapi_context
    };
    
    printf("rsa_sign API mechanism is %lx \n", mechanism);
    ctx.sessionData.sessionHandle = TPM_RS_PW;
    ctx.validation.tag = TPM_ST_HASHCHECK;
    ctx.validation.hierarchy = TPM_RH_NULL;
    ctx.halg = TPM_ALG_SHA256;
    ctx.keyHandle = *(TPMI_DH_OBJECT *)keyHandle;

    rval = Tss2_Sys_ContextLoad(ctx.sapi_context, &loaded_key_context, &ctx.keyHandle);
    if (rval != TPM_RC_SUCCESS) {
        printf("ContextLoad Error in RSA Sign API. TPM Error:0x%x", rval);
        goto out;
    }
    ctx.length = msg_len;
    ctx.msg = msg;

    if (!sign_and_save(&ctx, sig, sig_len)){
        printf("RSA sign failed\n");
        goto out;
    }

    printf("RSA sign API successful in TPM plugin ! \n");

out:
    sapi_teardown_full(sapi_context);

    return 0;

}


