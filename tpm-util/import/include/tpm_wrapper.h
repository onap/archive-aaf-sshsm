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
// Author: Arun Kumar Sekar

#ifndef __TPM_WRAPPER_H__
#define __TPM_WRAPPER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <getopt.h>

#include <tss2/tss2_sys.h>
#include <tss2/tss2-tcti-tabrmd.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_esys.h>
#include <tss2/tss2_mu.h>
#include <tss2/tss2_tcti.h>
#include <tss2/tss2_tpm2_types.h>
#include <tss2/tpm2b.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INIT_SIMPLE_TPM2B_SIZE( type ) (type).size = sizeof( type ) - 2;

TSS2_TCTI_CONTEXT* tpm2_tcti_tabrmd_init (void);

TSS2_SYS_CONTEXT* sys_ctx_init (TSS2_TCTI_CONTEXT *tcti_ctx);

TSS2_RC TeardownTctiContext( TSS2_TCTI_CONTEXT *tctiContext );

void TeardownSysContext( TSS2_SYS_CONTEXT **sysContext );

TSS2_RC swKeyTpmImport(
      /* IN */
      TSS2_SYS_CONTEXT *sysContext,
      TPM2_HANDLE parentKeyHandle,
      TPM2B_DATA* encryptionKey, TPM2B_PUBLIC* swKeyPublic, TPM2B_PRIVATE* swKeyPrivate,  TPM2B_ENCRYPTED_SECRET* encSymSeed,
      unsigned char* tpm_pwd, int tpm_pwd_len,
      /* OUT */
      TPM2B_PRIVATE *importPrivate);

#ifdef __cplusplus
}
#endif


#endif
