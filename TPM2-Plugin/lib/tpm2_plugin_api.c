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

#include <tss2/tss2_sys.h>
#include <tss2/tss2-tcti-tabrmd.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_esys.h>
#include <tss2/tss2_mu.h>
#include <tss2/tss2_tcti.h>
#include <tss2/tss2_tcti_device.h>
#include <tss2/tss2_tcti_mssim.h>
#include <tss2/tss2_tpm2_types.h>

#ifndef TSS2_SYS_H
#define TSS2_SYS_H
#endif
#ifndef TSS2_API_VERSION_1_2_1_108
#error Version mismatch among TSS2 header files.
#endif  /* TSS2_API_VERSION_1_2_1_108 */
#ifdef __cplusplus
extern "C" {
#endif


/* SAPI context blob */
typedef struct _TSS2_SYS_OPAQUE_CONTEXT_BLOB TSS2_SYS_CONTEXT;

#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include "tpm2_plugin_api.h"
#ifdef HAVE_TCTI_DEV
#include <tss2/tss2_tcti.h>
#endif
#ifdef HAVE_TCTI_SOCK
#include <tss2/tss2-tcti-tabrmd.h>
#endif
#ifdef HAVE_TCTI_TABRMD
#include <tss2/tss2-tcti-tabrmd.h>
#endif
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))
#define TSSWG_INTEROP 1
#define TSS_SAPI_FIRST_FAMILY 2
#define TSS_SAPI_FIRST_LEVEL 1
#define TSS_SAPI_FIRST_VERSION 108

bool output_enabled = true;
bool hexPasswd = false;
TPM2_HANDLE handle2048rsa;
const char *tcti_path="libtss2-tcti-device.so";

static void tcti_teardown(TSS2_TCTI_CONTEXT *tcticontext)
{
    if (tcticontext == NULL)
        return;
    Tss2_Tcti_Finalize (tcticontext);
    free (tcticontext);
}

static void sapi_teardown(TSS2_SYS_CONTEXT *sysContext)
{
    if (sysContext == NULL)
        return;
    Tss2_Sys_Finalize (sysContext);
    free (sysContext);
}

static void sapi_teardown_full (TSS2_SYS_CONTEXT *sysContext)
{
    TSS2_TCTI_CONTEXT *tcticontext = NULL;
    TSS2_RC rc;

    rc = Tss2_Sys_GetTctiContext (sysContext, &tcticontext);
    if (rc != TSS2_RC_SUCCESS)
        return;
    sapi_teardown (sysContext);
    tcti_teardown (tcticontext);
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

    //rc = tss2_tcti_tabrmd_init(NULL, &size);
    rc = Tss2_Tcti_Tabrmd_Init(NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf("Failed to get size for TABRMD TCTI context: 0x%x", rc);
        return NULL;
    }
    tcti_ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (tcti_ctx == NULL) {
        printf("Allocation for TABRMD TCTI context failed: %s", strerror (errno));
        return NULL;
    }
    rc = Tss2_Tcti_Tabrmd_Init (tcti_ctx, &size, NULL);
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
    TSS2_SYS_CONTEXT *sysContext;
    TSS2_RC rc;
    size_t size;
    TSS2_ABI_VERSION abi_version = {
        .tssCreator = TSSWG_INTEROP,
        .tssFamily  = TSS_SAPI_FIRST_FAMILY,
        .tssLevel   = TSS_SAPI_FIRST_LEVEL,
        .tssVersion = TSS_SAPI_FIRST_VERSION,
    };

    size = Tss2_Sys_GetContextSize (0);
    sysContext = (TSS2_SYS_CONTEXT*)calloc (1, size);
    if (sysContext == NULL) {
        fprintf (stderr,
                 "Failed to allocate 0x%zx bytes for the SAPI context\n",
                 size);
        return NULL;
    }
    rc = Tss2_Sys_Initialize (sysContext, size, tcti_ctx, &abi_version);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr, "Failed to initialize SAPI context: 0x%x\n", rc);
        free (sysContext);
        return NULL;
    }
    return sysContext;
}

#define BUFFER_SIZE(type, field) (sizeof((((type *)NULL)->field)))
#define TPM2B_TYPE_INIT(type, field) { .size = BUFFER_SIZE(type, field), }

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
int load_key(TSS2_SYS_CONTEXT *sysContext,
             TSS2L_SYS_AUTH_COMMAND sessionData,
             TPMI_DH_OBJECT    parentHandle,
             TPM2B_PUBLIC     *inPublic,
             TPM2B_PRIVATE    *inPrivate)
{
    UINT32 rval;
    TSS2L_SYS_AUTH_RESPONSE sessionsDataOut;
    TPM2B_NAME nameExt = TPM2B_TYPE_INIT(TPM2B_NAME, name);

TSS2L_SYS_AUTH_COMMAND sessionsData = { .count = 1, .auths = {{
        .sessionHandle = TPM2_RS_PW,
        .sessionAttributes = 0,
        .nonce = {.size = 0},
        .hmac = {.size = 0}}}};

    if (sessionsData.auths[0].hmac.size > 0 && hexPasswd)
    {
        sessionsData.auths[0].hmac.size = sizeof(sessionsData.auths[0].hmac) - 2;
        if (hex2ByteStructure((char *)sessionsData.auths[0].hmac.buffer,
                              &sessionsData.auths[0].hmac.size,
                              sessionsData.auths[0].hmac.buffer) != 0)
        {
            printf( "Failed to convert Hex format password for parent Passwd.\n");
            return -1;
        }
    }

    rval = Tss2_Sys_Load (sysContext,
                          parentHandle,
                          &sessionsData,
                          inPrivate,
                          inPublic,
                          &handle2048rsa,
                          &nameExt,
                          &sessionsDataOut);
    if(rval != TPM2_RC_SUCCESS)
    {
        printf("\nLoad Object Failed ! ErrorCode: 0x%0x\n\n",rval);
        return -1;
    }
    printf("\nLoad succ.\nLoadedHandle: 0x%08x\n\n",handle2048rsa);

    return 0;
}

int read_public(TSS2_SYS_CONTEXT *sysContext,
                TPM2_HANDLE handle,
                SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info)
{
    TSS2L_SYS_AUTH_RESPONSE sessionsDataOut;

    TPM2B_PUBLIC public = {
             0
    };

    TPM2B_NAME name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2B_NAME qualified_name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2_RC rval = Tss2_Sys_ReadPublic(sysContext, handle, 0,
            &public, &name, &qualified_name, &sessionsDataOut);
    if (rval != TPM2_RC_SUCCESS) {
        printf("TPM2_ReadPublic error: rval = 0x%0x", rval);
        return false;
    }

    printf("\nTPM2_ReadPublic OutPut: \n");
    printf("name: \n");
    UINT16 i;
    for (i = 0; i < name.size; i++)
        printf("%02x ", name.name[i]);
    printf("\n");

    printf("qualified_name: \n");
    for (i = 0; i < qualified_name.size; i++)
        printf("%02x ", qualified_name.name[i]);
    printf("\n");

    printf("public.publicArea.parameters.rsaDetail.keyBits = %d \n", public.publicArea.parameters.rsaDetail.keyBits);
    printf("public.publicArea.parameters.rsaDetail.exponent = %d \n", public.publicArea.parameters.rsaDetail.exponent);

    importkey_info->modulus_size = public.publicArea.unique.rsa.size;
    printf("importkey_info->modulus_size = %ld \n", importkey_info->modulus_size);
    importkey_info->modulus = (unsigned char *) malloc(importkey_info->modulus_size);
    if (importkey_info->modulus != NULL) {
        memcpy(importkey_info->modulus, &public.publicArea.unique.rsa.buffer, importkey_info->modulus_size);
    }

    importkey_info->exponent_size = sizeof(public.publicArea.parameters.rsaDetail.exponent);
    printf("importkey_info->exponent_size = %ld \n", importkey_info->exponent_size);
    importkey_info->exponent = (unsigned int *) malloc(importkey_info->exponent_size);
    if (importkey_info->exponent != NULL) {
        memcpy(importkey_info->exponent, &public.publicArea.parameters.rsaDetail.exponent, importkey_info->exponent_size);
    }

    //*importkey_info->exponent = public.publicArea.parameters.rsaDetail.exponent;

    return 0;
}

/*
Reads the PRK_PASSWORD Environment variable
and populates that information into the
provided sessionData variable
*/
int readPassword(TSS2L_SYS_AUTH_COMMAND *sessionData)
{
    char *prk_passwd;

    prk_passwd = getenv("TPM_PRK_PASSWORD");
    if (prk_passwd != NULL) {
        sessionData->auths[0].hmac.size = strlen(prk_passwd);
        if (sessionData->auths[0].hmac.size > sizeof(sessionData->auths[0].hmac.buffer)) {
            return -1;
        }
        memcpy(sessionData->auths[0].hmac.buffer, prk_passwd, sessionData->auths[0].hmac.size);
        return 0;
    }
    return 0;
}

TPMS_CONTEXT loaded_key_context;

int load_key_execute(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
                     void **keyHandle, TSS2_SYS_CONTEXT *sysContext,
                     SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info)
{

    TPMI_DH_OBJECT parentHandle;
    TPM2B_PUBLIC  inPublic;
    TPM2B_PRIVATE inPrivate;
    TSS2L_SYS_AUTH_COMMAND sessionData;
    UINT16 size;
    int returnVal = 0;

    /*
        Initializing the sessionData structure to the 0 values
        sessionAttributes is a union and the following assignment
        is based on the method used in other tpm2 tools.
    */
     TSS2L_SYS_AUTH_COMMAND sessionsData = { .count = 1, .auths = {{
        .sessionHandle = TPM2_RS_PW,
        .sessionAttributes = 0,
        .nonce = {.size = 0},
        .hmac = {.size = 0}}}};
    memset(&inPublic,0,sizeof(TPM2B_PUBLIC));
    memset(&inPrivate,0,sizeof(TPM2B_PRIVATE));

    setbuf(stdout, NULL);
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);

    parentHandle = srk_handle;

    if (loadkey_in_info->num_buffers != 2)
        return -1;

    /*
        Identify which buffer is public vs which is private
        TPM2B_PUBLIC should be 360 bytes
        TPM2B_PRIVATE should be 912 bytes
    */

    for (int i=0; i<2; i++) {
        if (loadkey_in_info->buffer_info[i]->length_of_buffer == sizeof(TPM2B_PUBLIC)) {
            memcpy(&inPublic, loadkey_in_info->buffer_info[i]->buffer,
                loadkey_in_info->buffer_info[i]->length_of_buffer);
            continue;
        }
        if (loadkey_in_info->buffer_info[i]->length_of_buffer == sizeof(TPM2B_PRIVATE)) {
            memcpy(&inPrivate, loadkey_in_info->buffer_info[i]->buffer,
                loadkey_in_info->buffer_info[i]->length_of_buffer);
            continue;
        }
    }

    // Read TPM_PRK_PASSWORD and setup sessionsData appropriately
    if (readPassword(&sessionData) != 0) {
        // Password read failure
        return -1;
    }

    returnVal = load_key (sysContext,
                          sessionData,
                          parentHandle,
                          &inPublic,
                          &inPrivate);
    returnVal = read_public(sysContext,
                            handle2048rsa,
                            importkey_info);

    TPM2_RC rval = Tss2_Sys_ContextSave(sysContext, handle2048rsa, &loaded_key_context);
    if (rval != TPM2_RC_SUCCESS) {
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

    TSS2_SYS_CONTEXT *sysContext = NULL;
    if (tcti_ctx) {
        sysContext = sapi_ctx_init(tcti_ctx);
        if (!sysContext) {
            free(tcti_ctx);
            return -1;
        }
    }

    ret = load_key_execute(loadkey_in_info, keyHandle, sysContext, importkey_info);
    if (ret !=0)
        printf("Load key API failed in TPM plugin ! \n");

    sapi_teardown_full(sysContext);

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
    TSS2_SYS_CONTEXT *sysContext;
};

//create a table to consolidate all parts of data from multiple SignUpdate from sessions
CONCATENATE_DATA_SIGNUPDATE_t data_signupdate_session[MAX_SESSIONS];
unsigned long sign_sequence_id = 0;
int tpm2_plugin_rsa_sign_init(
        void *keyHandle,
        unsigned long mechanism,
        void *param,
        int len,
        void **plugin_data_ref
       )
{
    printf("rsa_sign_init API mechanism is %ld \n", mechanism);
    printf("rsa_sign_init API len is %d \n", len);
    int i, j;

    sign_sequence_id++;
    unsigned long hSession = sign_sequence_id;

    for (i = 0; i < MAX_SESSIONS; i++){
        if (data_signupdate_session[i].session_handle == 0){
            data_signupdate_session[i].session_handle = hSession;
            for (j = 0; j < MAX_DATA_SIGNUPDATE; j++ )
                data_signupdate_session[i].data_signupdate[j] = 0;
            data_signupdate_session[i].data_length = 0;
        }
    }
    *plugin_data_ref = (void *)hSession;

    printf("rsa_sign_init API done for tpm2_plugin... \n");
    return 0;
}

/** This function is called by SSHSM only if there sign_final function is not called.
If sign_final function is called, it is assumed that plugin would have cleaned this up.
***/

int tpm2_plugin_rsa_sign_cleanup(
         void *keyHandle,
         unsigned long mechnaism,
         void *plugin_data_ref
        )
{
    int i, j;
    unsigned long hSession = (unsigned long)plugin_data_ref;
    for (i = 0; i < MAX_SESSIONS; i++)    {
        if (data_signupdate_session[i].session_handle == hSession){
            data_signupdate_session[i].session_handle = 0;
            for (j =0; j < MAX_DATA_SIGNUPDATE; j++ )
                data_signupdate_session[i].data_signupdate[j] =0;
            data_signupdate_session[i].data_length = 0;
        }
    }

    if (sign_sequence_id>0xfffffffe)
        sign_sequence_id =0;
    return 0;
}


UINT32 tpm_hash(TSS2_SYS_CONTEXT *sysContext, TPMI_ALG_HASH hashAlg,
        UINT16 size, BYTE *data, TPM2B_DIGEST *result) {
    TPM2B_MAX_BUFFER dataSizedBuffer;

    dataSizedBuffer.size = size;
    memcpy(dataSizedBuffer.buffer, data, size);
    return Tss2_Sys_Hash(sysContext, 0, &dataSizedBuffer, hashAlg,
            TPM2_RH_NULL, result, 0, 0);
}

static TPM2_RC hash_sequence_ex(TSS2_SYS_CONTEXT *sysContext,

    TPMI_ALG_HASH hashAlg, UINT32 numBuffers, TPM2B_MAX_BUFFER *bufferList,
    TPM2B_DIGEST *result) {
    TPM2_RC rval;
    TPM2B_AUTH nullAuth;
    TPMI_DH_OBJECT sequenceHandle;
    TPM2B_MAX_BUFFER emptyBuffer;
    TPMT_TK_HASHCHECK validation;

    TPMS_AUTH_COMMAND cmdAuth;
    TPMS_AUTH_COMMAND *cmdSessionArray[1] = { &cmdAuth };
    TSS2L_SYS_AUTH_COMMAND cmdAuthArray = { .count = 1, .auths = {{
        .sessionHandle = TPM2_RS_PW,
        .sessionAttributes = 0,
        .nonce = {.size = 0},
        .hmac = {.size = 0}}}};

    nullAuth.size = 0;
    emptyBuffer.size = 0;

    // Set result size to 0, in case any errors occur
    result->size = 0;

    // Init input sessions struct
    cmdAuth.sessionHandle = TPM2_RS_PW;
    cmdAuth.nonce.size = 0;
    *((UINT8 *) ((void *) &cmdAuth.sessionAttributes)) = 0;
    cmdAuth.hmac.size = 0;

    rval = Tss2_Sys_HashSequenceStart(sysContext, 0, &nullAuth, hashAlg,
            &sequenceHandle, 0);
    if (rval != TPM2_RC_SUCCESS) {
        return rval;
    }

    unsigned i;
    for (i = 0; i < numBuffers; i++) {
        rval = Tss2_Sys_SequenceUpdate(sysContext, sequenceHandle,
                &cmdAuthArray, &bufferList[i], 0);

        if (rval != TPM2_RC_SUCCESS) {
            return rval;
        }
    }

    rval = Tss2_Sys_SequenceComplete(sysContext, sequenceHandle,
            &cmdAuthArray, (TPM2B_MAX_BUFFER *) &emptyBuffer,
            TPM2_RH_PLATFORM, result, &validation, 0);

    if (rval != TPM2_RC_SUCCESS) {
        return rval;
    }

    return rval;
}

int tpm_hash_compute_data(TSS2_SYS_CONTEXT *sysContext, BYTE *buffer,
        UINT16 length, TPMI_ALG_HASH halg, TPM2B_DIGEST *result) {

    if (length <= TPM2_MAX_DIGEST_BUFFER) {
        if (tpm_hash(sysContext, halg, length, buffer,
                result) == TPM2_RC_SUCCESS){
            printf("Single hash result size: %d\n", result->size);
            return 0;
        }
        else
            return -1;
    }

    UINT8 numBuffers = (length - 1) / TPM2_MAX_DIGEST_BUFFER + 1;

    TPM2B_MAX_BUFFER *bufferList = (TPM2B_MAX_BUFFER *) calloc(numBuffers,
            sizeof(TPM2B_MAX_BUFFER));
    if (bufferList == NULL)
        return -2;

    UINT32 i;
    for (i = 0; i < (UINT32)(numBuffers - 1); i++) {
        bufferList[i].size = TPM2_MAX_DIGEST_BUFFER;
        memcpy(bufferList[i].buffer, buffer + i * TPM2_MAX_DIGEST_BUFFER,
                TPM2_MAX_DIGEST_BUFFER);
    }
    bufferList[i].size = length - i * TPM2_MAX_DIGEST_BUFFER;
    memcpy(bufferList[i].buffer, buffer + i * TPM2_MAX_DIGEST_BUFFER,
            bufferList[i].size);

    TPM2_RC rval = hash_sequence_ex(sysContext, halg, numBuffers, bufferList, result);
    free(bufferList);
    printf("Sequence hash result size: %d\n", result->size);
    return rval == TPM2_RC_SUCCESS ? 0 : -3;
}


static bool get_key_type(TSS2_SYS_CONTEXT *sysContext, TPMI_DH_OBJECT objectHandle,
        TPMI_ALG_PUBLIC *type) {

    TSS2L_SYS_AUTH_RESPONSE sessions_data_out;

    TPM2B_PUBLIC out_public = {
            0
    };

    TPM2B_NAME name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2B_NAME qaulified_name = TPM2B_TYPE_INIT(TPM2B_NAME, name);

    TPM2_RC rval = Tss2_Sys_ReadPublic(sysContext, objectHandle, 0, &out_public, &name,
            &qaulified_name, &sessions_data_out);
    if (rval != TPM2_RC_SUCCESS) {
        printf("Sys_ReadPublic failed, error code: 0x%x", rval);
        return false;
    }
    *type = out_public.publicArea.type;
    return true;
}

static bool set_scheme(TSS2_SYS_CONTEXT *sysContext, TPMI_DH_OBJECT keyHandle,
        TPMI_ALG_HASH halg, TPMT_SIG_SCHEME *inScheme) {

    TPM2_ALG_ID type;
    bool result = get_key_type(sysContext, keyHandle, &type);
    if (!result) {
        return false;
    }

    switch (type) {
    case TPM2_ALG_RSA :
        inScheme->scheme = TPM2_ALG_RSASSA;
        inScheme->details.rsassa.hashAlg = halg;
        break;
    case TPM2_ALG_KEYEDHASH :
        inScheme->scheme = TPM2_ALG_HMAC;
        inScheme->details.hmac.hashAlg = halg;
        break;
    case TPM2_ALG_ECC :
        inScheme->scheme = TPM2_ALG_ECDSA;
        inScheme->details.ecdsa.hashAlg = halg;
        break;
    case TPM2_ALG_SYMCIPHER :
    default:
        printf("Unknown key type, got: 0x%x", type);
        return false;
    }

    return true;
}
static bool sign_and_save(tpm_sign_ctx *ctx, TPMT_SIGNATURE *sig) {
    TPM2B_DIGEST digest = TPM2B_TYPE_INIT(TPM2B_DIGEST, buffer);

    TPMT_SIG_SCHEME in_scheme;
    TSS2L_SYS_AUTH_RESPONSE sessions_data_out;

    TSS2L_SYS_AUTH_COMMAND sessions_data = { .count = 1, .auths = {{
	    .sessionHandle = TPM2_RS_PW,
		    .sessionAttributes = 0,
		    .nonce = {.size = 0},
		    .hmac = {.size = 0}}}};

    int rc = tpm_hash_compute_data(ctx->sysContext, ctx->msg, ctx->length, ctx->halg, &digest);
    if (rc) {
        printf("Compute message hash failed!");
        return false;
    }

    printf("Compute message hash digest size : %d \n", digest.size);

    bool result = set_scheme(ctx->sysContext, ctx->keyHandle, ctx->halg, &in_scheme);
    if (!result) {
        return false;
    }

    TPM2_RC rval = Tss2_Sys_Sign(ctx->sysContext, ctx->keyHandle,
                                &sessions_data, &digest, &in_scheme,
                                &ctx->validation, sig,
                                &sessions_data_out);

    if (rval != TPM2_RC_SUCCESS) {
        printf("Sys_Sign failed, error code: 0x%x", rval);
        return false;
    }
    return true;
}

int tpm2_plugin_rsa_sign(
        void  *keyHandle,
        unsigned long mechanism,
        unsigned char *msg,
        int msg_len,
        void *plugin_data_ref,
        unsigned char *sig,
        int *sig_len)
{
    TPM2_RC rval;
    common_opts_t opts = COMMON_OPTS_INITIALIZER;
    TPMT_SIGNATURE signature;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    tcti_ctx = tcti_init_from_options(&opts);
    if (tcti_ctx == NULL)
        return -1;

    TSS2_SYS_CONTEXT *sysContext = NULL;
    if (tcti_ctx) {
       sysContext = sapi_ctx_init(tcti_ctx);
       if (!sysContext) {
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
            .sysContext = sysContext
    };

    printf("rsa_sign API mechanism is %lx \n", mechanism);
    ctx.sessionData.sessionHandle = TPM2_RS_PW;
    ctx.validation.tag = TPM2_ST_HASHCHECK;
    ctx.validation.hierarchy = TPM2_RH_NULL;
    if (mechanism == 7)
        ctx.halg = TPM2_ALG_SHA256;
    else
        printf("mechanism not supported! \n");
    ctx.keyHandle = *(TPMI_DH_OBJECT *)keyHandle;

    rval = Tss2_Sys_ContextLoad(ctx.sysContext, &loaded_key_context, &ctx.keyHandle);
    if (rval != TPM2_RC_SUCCESS) {
        printf("ContextLoad Error in RSA Sign API. TPM Error:0x%x", rval);
        goto out;
    }
    ctx.length = msg_len;
    ctx.msg = msg;

    if (!sign_and_save(&ctx, &signature)){
        printf("RSA sign failed\n");
        goto out;
    }

    *sig_len = (int)signature.signature.rsassa.sig.size;
    printf("signature length:  %d \n", *sig_len);
    memcpy(sig, signature.signature.rsassa.sig.buffer, *sig_len);
    printf("signature buffer size:  %ld \n", sizeof(signature.signature.rsassa.sig.buffer));
    printf("RSA sign API successful in TPM plugin ! \n");

out:
    sapi_teardown_full(sysContext);

    return 0;

}

int tpm2_plugin_rsa_sign_update(
         void *keyHandle,
         unsigned long mechanism,
         unsigned char *msg,
         int msg_len,
         void *plugin_data_ref
        )
{
    int i, j, n;
    unsigned long hSession = (unsigned long)plugin_data_ref;
    for (i = 0; i < MAX_SESSIONS; i++){
        if (data_signupdate_session[i].session_handle == hSession){
            n = data_signupdate_session[i].data_length;
            for (j =0; j < msg_len; j++ )
                data_signupdate_session[i].data_signupdate[n + j] = msg[j];
            data_signupdate_session[i].data_length += msg_len;
            return 0;
        }
    }
    return -1;
}

int tpm2_plugin_rsa_sign_final(
         void *keyHandle,
         unsigned long mechanism,
         void *plugin_data_ref,
         unsigned char *outsig,
         int *outsiglen
        )
{
    int i, j;
    unsigned long hSession = (unsigned long)plugin_data_ref;
    unsigned char *msg;
    int msg_len;
    for (i = 0; i < MAX_SESSIONS; i++){
        if (data_signupdate_session[i].session_handle == hSession){
            msg = data_signupdate_session[i].data_signupdate;
            msg_len = data_signupdate_session[i].data_length;
            tpm2_plugin_rsa_sign(keyHandle, mechanism, msg, msg_len, plugin_data_ref, outsig, outsiglen);
            tpm2_plugin_rsa_sign_cleanup(keyHandle, mechanism, plugin_data_ref);
            return 0;
        }
    }

    return -1;
}

