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

#include "crypto_aux.h"
#include "util.h"

UINT32 (*HmacFunctionPtr)( TPMI_ALG_HASH hashAlg, TPM2B *key,TPM2B **bufferList, TPM2B_DIGEST *result ) = OpenSslHmac;


int RSA_OAEP_Enc(TPM2B_PUBLIC_KEY_RSA *plain, // plain text to encrypt
											//Size of plain (0 <= pl <= kl - (2 * hashLen + 2)
				TPM2B_PUBLIC_KEY_RSA *cipher, 		// must be same size as key in bytes
				TPM2B_PUBLIC_KEY_RSA *key, 			// Key in big endian byte array
				TPM2B_DATA *encoding_params // Null terminated string like
												// ((unsigned char*)"DUPLICATE")
												// length of encoding parameter includes \0
												// (10 in DUPLICATE case..)
				)
{
	RSA *rsa = NULL;
	unsigned char 	encoded[256];
	int 	RC;
	BIGNUM* bne;
	BIGNUM* n;

	//Encoding
	RC = RSA_padding_add_PKCS1_OAEP_mgf1(encoded, key->size, plain->buffer, plain->size,
			encoding_params->buffer, encoding_params->size, EVP_sha256(), NULL);

	if(RC!=1)goto cleanup;

	// Creating OpenSSL structure with the supplied TPM public:
	bne = BN_new();
	RC = BN_set_word(bne,RSA_F4); // the TPM's public exponent (2^16 + 1)
	if(RC!=1)goto cleanup;

	rsa = RSA_new();
	RC = RSA_generate_key_ex(rsa, 2048, bne, NULL); // could be done in better way i guess... just for filling up fields..
	if(RC!=1)goto cleanup;

	// Over-writing the public N:
	//rsa->n = BN_bin2bn(key->b.buffer, key->b.size, rsa->n);
	n = BN_bin2bn(key->buffer, key->size, NULL);
	RSA_set0_key(rsa,n,NULL, NULL);

	//if(rsa->n == NULL) goto cleanup;
	if(n == NULL) goto cleanup;

	// Encrypting
	RC = RSA_public_encrypt(key->size, encoded, cipher->buffer, rsa, RSA_NO_PADDING);

	//if(RC<0)goto cleanup;
	cipher->size = key->size;

cleanup:
	RSA_free(rsa);
	BN_free(bne);
	return RC;
}



void AES_128_CFB_enc_dec(
		TPM2B *in,
		TPM2B *out,
		const TPM2B *const key,
		const TPM2B *const ivIn,
		TPM2B *ivOut,
		const TPMI_YES_NO enc)
{
	TPM2B_SYM_KEY ivTemp = {0};
	ivTemp.size = 16;

	if(ivOut == NULL)
		ivOut = (TPM2B *) &(ivTemp);

	memccpy(ivOut->buffer, ivIn->buffer, 0, ivIn->size);
	AES_KEY aes;
	AES_set_encrypt_key(key->buffer, 128, &aes);
	int block, j;
	for(block=0; block < (in->size) ;block+=16)
	{
		unsigned char encIV[16];
		AES_encrypt(ivOut->buffer, encIV, &aes);

		for(j=0;j<16;j++)
		{
			if(j+block >= (in->size))
				ivOut->buffer[j]=0;
			else if(enc)
				ivOut->buffer[j] = out->buffer[block+j] = encIV[j]^(in->buffer[block+j]);
			else
			{
				ivOut->buffer[j] = in->buffer[block+j];
				out->buffer[block+j] = encIV[j]^(in->buffer[block+j]);
			}
		}
	}
	out->size = in->size;

}


UINT32 ChangeEndianDword( UINT32 p )
{
    return( ((const UINT32)(((p)& 0xFF) << 24))    | \
          ((const UINT32)(((p)& 0xFF00) << 8))   | \
          ((const UINT32)(((p)& 0xFF0000) >> 8)) | \
          ((const UINT32)(((p)& 0xFF000000) >> 24)));
}


TPM2_RC KDFa( TPMI_ALG_HASH hashAlg, TPM2B *key, char *label,
    TPM2B *contextU, TPM2B *contextV, UINT16 bits, TPM2B_MAX_BUFFER  *resultKey )
{

    TPM2B_DIGEST tmpResult;
    TPM2B_DIGEST tpm2bLabel, tpm2bBits, tpm2b_i_2;
    UINT8 *tpm2bBitsPtr = &tpm2bBits.buffer[0];
    UINT8 *tpm2b_i_2Ptr = &tpm2b_i_2.buffer[0];
    TPM2B_DIGEST *bufferList[8];
    UINT32 bitsSwizzled, i_Swizzled;
    TPM2_RC rval;
    int i, j;
    UINT16 bytes = bits / 8;

#ifdef DEBUG
    DebugPrintf( 0, "KDFA, hashAlg = %4.4x\n", hashAlg );
    DebugPrintf( 0, "\n\nKDFA, key = \n" );
    PrintSizedBuffer( key );
#endif

    resultKey->size = 0;

    tpm2b_i_2.size = 4;

    tpm2bBits.size = 4;
    bitsSwizzled = ChangeEndianDword( bits );
    *(UINT32 *)tpm2bBitsPtr = bitsSwizzled;

    for(i = 0; label[i] != 0 ;i++ );

    tpm2bLabel.size = i+1;
    for( i = 0; i < tpm2bLabel.size; i++ )
    {
        tpm2bLabel.buffer[i] = label[i];
    }

#ifdef DEBUG
    DebugPrintf( 0, "\n\nKDFA, tpm2bLabel = \n" );
    PrintSizedBuffer( (TPM2B *)&tpm2bLabel );

    DebugPrintf( 0, "\n\nKDFA, contextU = \n" );
    PrintSizedBuffer( contextU );

    DebugPrintf( 0, "\n\nKDFA, contextV = \n" );
    PrintSizedBuffer( contextV );
#endif

    resultKey->size = 0;

    i = 1;

    while( resultKey->size < bytes )
    {
        // Inner loop

        i_Swizzled = ChangeEndianDword( i );
        *(UINT32 *)tpm2b_i_2Ptr = i_Swizzled;

        j = 0;
        bufferList[j++] = (TPM2B_DIGEST *)&(tpm2b_i_2);
        bufferList[j++] = (TPM2B_DIGEST *)&(tpm2bLabel);
        bufferList[j++] = (TPM2B_DIGEST *)contextU;
        bufferList[j++] = (TPM2B_DIGEST *)contextV;
        bufferList[j++] = (TPM2B_DIGEST *)&(tpm2bBits);
        bufferList[j++] = (TPM2B_DIGEST *)0;
#ifdef DEBUG
        for( j = 0; bufferList[j] != 0; j++ )
        {
            DebugPrintf( 0, "\n\nbufferlist[%d]:\n", j );
            PrintSizedBuffer( &( bufferList[j]->b ) );
        }
#endif
        rval = (*HmacFunctionPtr )( hashAlg, key, (TPM2B **)&( bufferList[0] ), &tmpResult );
        if( rval != TPM2_RC_SUCCESS )
        {
            return( rval );
        }

        ConcatSizedByteBuffer( resultKey, (TPM2B *) &(tmpResult) );
    }

    // Truncate the result to the desired size.
    resultKey->size = bytes;

#ifdef DEBUG
    DebugPrintf( 0, "\n\nKDFA, resultKey = \n" );
    PrintSizedBuffer( &( resultKey->b ) );
#endif

    return TPM2_RC_SUCCESS;
}


UINT32 OpenSslHmac( TPMI_ALG_HASH hashAlg, TPM2B *key,TPM2B **bufferList, TPM2B_DIGEST *result )
{
	if(hashAlg != TPM2_ALG_SHA256)return -1;

	UINT32 RC = 0;
	HMAC_CTX *hmac = HMAC_CTX_new();
	UINT32 resLen=0;

	int i=0;

	HMAC_Init_ex(hmac, key->buffer, key->size, EVP_sha256(), NULL);

	for(i=0;bufferList[i];i++) 
	{
		HMAC_Update(hmac, bufferList[i]->buffer, bufferList[i]->size);
	}

	HMAC_Final(hmac, result->buffer, &resLen);
	result->size = resLen;

	HMAC_CTX_free(hmac);

	return RC;
}

void print_buff(char * data, int len, const PBYTE buff)
{
    printf("%s \n",data);
    int i = 0;
    for(;i<len;i++)
        printf("0x%02X, ", buff[i]);
    printf("\n");

}

