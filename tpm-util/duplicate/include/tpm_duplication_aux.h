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

#ifndef     TPM_DUP_AUX
#define     TPM_DUP_AUX

#include <openssl/rand.h>
#include <openssl/hmac.h>
#include "crypto_aux.h"

#define BYTES_TO_BITS(val) (val * 8) 

#define INIT_SIMPLE_TPM2B_SIZE( type ) (type).size = sizeof( type ) - 2;

TPM2B_NAME * GetName(TPMI_ALG_HASH hashAlg, TPM2B_PUBLIC *obj);

TSS2_RC swKeyDuplicate(
	  RSA* rsaKey, TPM2B_PUBLIC* parentKeyPublicPortion, UINT8* policyDigest, int digestSize,
      /* OUT */ 
      TPM2B* encryptionKey, TPM2B_PUBLIC *swKeyPublic, TPM2B_PRIVATE *swKeyPrivate,  TPM2B_ENCRYPTED_SECRET *encSymSeed);


void CreateDuplicationBlob(
		//IN
		TPM2B_PUBLIC_KEY_RSA *protector,
		TPMT_PUBLIC * publicPortion,
		TPMT_SENSITIVE *sens,
		TPM2B *plainSymSeed, TPMI_YES_NO generateInSymSeed,
		TPM2B encryptionKey, TPMI_YES_NO generateEncryptionKey,

		//OUT
		TPM2B_PRIVATE *outDuplicate,
		TPM2B_ENCRYPTED_SECRET *encSymSeed);


void CreateSwDataObject(
		BYTE* auth, UINT16 authSize,
		RSA * rsakey,
		BYTE * dataToSeal, UINT16 dataSize,
		BYTE * policyDigest, UINT16 policyDigestSize,
		TPMT_PUBLIC * outPublic, 
        TPMT_SENSITIVE *outSens);

void CreateDuplicationBlob2B(
	//IN
		TPM2B_PUBLIC_KEY_RSA *protector,
		TPM2B_PUBLIC * public2B,
		TPM2B_SENSITIVE *sens2B,
		TPM2B *plainSymSeed, TPMI_YES_NO generateInSymSeed,
		TPM2B encryptionKey, TPMI_YES_NO generateEncryptionKey,

		//OUT
		TPM2B_PRIVATE *outDuplicate,
		TPM2B_ENCRYPTED_SECRET *encSymSeed);

void CreateSwDataObject2B(
		//IN
		BYTE* auth, UINT16 authSize,
		RSA * rsaKey,
		BYTE * policyDigest, UINT16 policyDigestSize,
		//OUT
		TPM2B_PUBLIC * outPublic, 
        TPM2B_SENSITIVE *outSens);


#endif //TPM_DUP_AUX

