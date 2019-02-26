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

#ifndef     CRYPTO_AUX
#define     CRYPTO_AUX

#include <string.h>
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
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

typedef BYTE* PBYTE;

int RSA_OAEP_Enc(TPM2B_PUBLIC_KEY_RSA *plain, // plain text to encrypt
											//Size of plain (0 <= pl <= kl - (2 * hashLen + 2)
				TPM2B_PUBLIC_KEY_RSA *cipher, 		// must be same size as key in bytes
				TPM2B_PUBLIC_KEY_RSA *key, 			// Key in big endian byte array
				TPM2B_DATA *encoding_params // Null terminated string like
												// ((unsigned char*)"DUPLICATE")
												// length of encoding parameter includes \0
												// (10 in DUPLICATE case..)
				);

void AES_128_CFB_enc_dec(
		TPM2B *in,
		TPM2B *out,
		const TPM2B *const key,
		const TPM2B *const ivIn,
		TPM2B *ivOut,
		const TPMI_YES_NO enc);



TPM2_RC KDFa( TPMI_ALG_HASH hashAlg, TPM2B *key, char *label,
    TPM2B *contextU, TPM2B *contextV, UINT16 bits, TPM2B_MAX_BUFFER  *resultKey );

UINT32 OpenSslHmac( TPMI_ALG_HASH hashAlg, TPM2B *key,TPM2B **bufferList, TPM2B_DIGEST *result );

void print_buff(char * data, int len, const PBYTE buff);

#endif

