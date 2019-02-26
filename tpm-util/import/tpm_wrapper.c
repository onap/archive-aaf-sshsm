/*
 * Copyright 2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */
//
// Author: Arun Kumar Sekar
//

#include "tpm_wrapper.h"
#include "util.h"

#include <errno.h>
#include <tss2/tss2_sys.h>
#include <tss2/tss2-tcti-tabrmd.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_esys.h>
#include <tss2/tss2_mu.h>
#include <tss2/tss2_tcti.h>
#include <tss2/tss2_tcti_device.h>
#include <tss2/tss2_tcti_mssim.h>
#include <tss2/tss2_tpm2_types.h>
#include <tss2/tpm2b.h>
#define TSSWG_INTEROP 1
#define TSS_SAPI_FIRST_FAMILY 2
#define TSS_SAPI_FIRST_LEVEL 1
#define TSS_SAPI_FIRST_VERSION 108

TSS2_RC swKeyTpmImport(
      /* IN */
      TSS2_SYS_CONTEXT *sysContext,
      TPM2_HANDLE parentKeyHandle,
      TPM2B_DATA* encryptionKey, TPM2B_PUBLIC* swKeyPublic, TPM2B_PRIVATE* swKeyPrivate,  TPM2B_ENCRYPTED_SECRET* encSymSeed,
      unsigned char* tpm_pwd, int tpm_pwd_len, 
      /* OUT */
      TPM2B_PRIVATE *importPrivate)
{
    TPM2_RC rval = TPM2_RC_SUCCESS;
    TPM2B_NAME name = {sizeof( TPM2B_NAME ) - 2};

    TPM2_HANDLE wrapperKeyHandle;

    TSS2L_SYS_AUTH_RESPONSE npsessionsDataOut;

    if(NULL == tpm_pwd) {
        printf("TPM password pinter is NULL \n");
        return -1;
    }

TSS2L_SYS_AUTH_COMMAND npsessionsData = { .count = 1, .auths = {{
        .sessionHandle = TPM2_RS_PW,
        .sessionAttributes = 0,
        .nonce = {.size = 0},
        .hmac = {.size = 0}}}};

    npsessionsData.auths[0].hmac.size = tpm_pwd_len;
    if(tpm_pwd_len > 0)
    {
        memcpy(npsessionsData.auths[0].hmac.buffer, tpm_pwd, npsessionsData.auths[0].hmac.size);
    }

    TPMT_SYM_DEF_OBJECT symmetricAlg;

    symmetricAlg.algorithm = TPM2_ALG_AES;
    symmetricAlg.keyBits.aes = 128;
    symmetricAlg.mode.aes = TPM2_ALG_CFB;

    rval =  Tss2_Sys_Import( sysContext,
                             parentKeyHandle,
                             &npsessionsData,
                             encryptionKey,
                             swKeyPublic,
                             swKeyPrivate,
                             encSymSeed,
                             &symmetricAlg,
                             importPrivate,
                             &npsessionsDataOut
                           );
    printf("The return value of sys import: 0x%x \n", rval);

#if 0
    if(rval == TSS2_RC_SUCCESS)
    {
        rval = Tss2_Sys_Load (sysContext, parentKeyHandle, &npsessionsData, importPrivate, swKeyPublic,
                &wrapperKeyHandle, &name, &npsessionsDataOut);
        printf("\n Load Imported Handle: 0x%x | rval 0x%x \n", wrapperKeyHandle, rval);
    }
    if(rval == TSS2_RC_SUCCESS)
    {
        rval = Tss2_Sys_FlushContext( sysContext, wrapperKeyHandle);
        printf("\n Flush Loaded key Handle : 0x%8.8x| rval 0x%x \n", wrapperKeyHandle, rval);
    }
#endif

    // Nullify TPM password
    memset(tpm_pwd, 0, tpm_pwd_len);

    return rval;
}

TSS2_TCTI_CONTEXT* tpm2_tcti_tabrmd_init (void)
{
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;
    size_t size;

    rc = Tss2_Tcti_Tabrmd_Init(NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("Failed to get size for TABRMD TCTI context: 0x%x", rc);
        return NULL;
    }

    tcti_ctx = (TSS2_TCTI_CONTEXT*)calloc (1, size);
    if (tcti_ctx == NULL) {
        printf ("Allocation for TABRMD TCTI context failed: %s",
                 strerror (errno));
        return NULL;
    }
    rc = Tss2_Tcti_Tabrmd_Init (tcti_ctx, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        printf ("Failed to initialize TABRMD TCTI context: 0x%x", rc);
        free (tcti_ctx);
        return NULL;
    }

    return tcti_ctx;
}

TSS2_SYS_CONTEXT* sys_ctx_init (TSS2_TCTI_CONTEXT *tcti_ctx)
{
    TSS2_SYS_CONTEXT *sys_ctx;
    TSS2_RC rc;
    size_t size;
    TSS2_ABI_VERSION abi_version = {
        .tssCreator = TSSWG_INTEROP,
        .tssFamily  = TSS_SAPI_FIRST_FAMILY,
        .tssLevel   = TSS_SAPI_FIRST_LEVEL,
        .tssVersion = TSS_SAPI_FIRST_VERSION,
    };

    size = Tss2_Sys_GetContextSize (0);
    sys_ctx = (TSS2_SYS_CONTEXT*)calloc (1, size);
    if (sys_ctx == NULL) {
        fprintf (stderr,
                 "Failed to allocate 0x%zx bytes for the SAPI context\n",
                 size);
        return NULL;
    }
    rc = Tss2_Sys_Initialize (sys_ctx, size, tcti_ctx, &abi_version);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf (stderr, "Failed to initialize SAPI context: 0x%x\n", rc);
        free (sys_ctx);
        return NULL;
    }

    return sys_ctx;
}

TSS2_RC TeardownTctiContext( TSS2_TCTI_CONTEXT *tctiContext )
{
    //(tctiContext)->finalize( tctiContext );
    //TSS2_TCTI_FINALIZiE(tctiContext);
    Tss2_Tcti_Finalize (tctiContext);
    free (tctiContext);
    tctiContext = NULL;
    return TSS2_RC_SUCCESS;
}

void TeardownSysContext( TSS2_SYS_CONTEXT **sysContext )
{
    if( *sysContext != 0 ) {
        Tss2_Sys_Finalize(*sysContext);
        free(*sysContext);
        *sysContext = 0;
    }
}

/* helper functions */
#if 0
TSS2_RC TPM2ReadPublicPortion(TSS2_SYS_CONTEXT *sysContext, TPM_HANDLE objectHandle, TPM2B_PUBLIC *publicPortion )
{
    TPM_RC rval = TPM_RC_SUCCESS;
    TPM2B_NAME qualifiedName;
    TPM2B_NAME name;
    INIT_SIMPLE_TPM2B_SIZE( name );
    INIT_SIMPLE_TPM2B_SIZE( qualifiedName );

    if(publicPortion == NULL) {
        printf("Input reference for TPM2B_PUBLIC structure is NULL");
        return TPM_RC_FAILURE;
    }

    publicPortion->t.size = 0;
    rval = Tss2_Sys_ReadPublic( sysContext, objectHandle , 0, publicPortion, &name, &qualifiedName, 0 );
    if(rval != TPM_RC_SUCCESS)
    {
        printf("\n Tss2_Sys_ReadPublic failed: 0x%x ! for TPM handle: 0x%x \n", rval, objectHandle);
    }

    return rval;
}
#endif

