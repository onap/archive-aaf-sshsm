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
// main.c : Tool to import Openssl RSA key into TPM. Generates TPM duplication data
// Author: Arun Kumar Sekar
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/pem.h>
#include <pthread.h>
#include <tss2/tss2-tcti-tabrmd.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_tpm2_types.h>
#include <tss2/tpm2b.h>
#include "tpm_duplication_aux.h"
#include "util.h"


void PrintHelp();
char version[] = "0.1";

void PrintHelp()
{
    printf(
            "OSSL key to tpm import tool, Version %s\nUsage:"
            "./ossl_tpm_duplicate [-pemfile InputPemFile] [-pempwd inputPemPwd (optional)] [-parentPub inputParentPubFile]"
            "[-dupPub out_dupPubFile] [-dupPriv out_dupPrivFile] [-dupSymSeed out_dupSymSeedFile] [-dupEncKey out_dupEncKeyFile] \n" 
			"\n"
			 , version);
}

static TPM2_RC convert_PEM_To_EVP(EVP_PKEY **evpPkey,                /* freed by caller */
                              const char *pem_Filename,
                              const char *pem_pwd)
{
    TPM2_RC      rc = 0;
    FILE        *fp_pemfile = NULL;

    if (rc == 0)
    {
        fp_pemfile = fopen(pem_Filename, "rb");  /* closed @2 */
        if(fp_pemfile == NULL) {
            rc = EXIT_FAILURE;
        }
    }

    if (rc == 0) 
    {
        *evpPkey = PEM_read_PrivateKey(fp_pemfile, NULL, NULL, (void *)pem_pwd);
        if (*evpPkey == NULL) 
        {
            printf("convert_PEM_To_EVP: Error reading key file %s\n", pem_Filename);
            rc = EXIT_FAILURE;
        }
        printf("PEM_read_PrivateKey success for file: %s \n", pem_Filename);
    }

end:
   if (fp_pemfile != NULL)
    {
        fclose(fp_pemfile);
    }

    return rc;
}

static TPM2_RC convert_EVP_to_RSA(RSA **rsaKey,              /* freed by caller */
                              EVP_PKEY *evpPkey)
{
    TPM2_RC      rc = 0;

    if (rc == 0)
    {
        *rsaKey = EVP_PKEY_get1_RSA(evpPkey);
        if (*rsaKey == NULL) 
        {
            printf("convert_EVP_to_RSA: EVP_PKEY_get1_RSA failed\n");
            rc = EXIT_FAILURE;
        }
        printf("convert_EVP_to_RSA success! \n");
    }

    return rc;
}


int main(int argc, char* argv[])
{
    TPM2_RC rval = 0;
    int count=0;

    char pem_Filename[256];
    int pemfile_flag = 0;
    const char *pem_pwd = "";   /* default empty password */
    int pempwd_flag = 0;

    // SW Key Duplicate I/P variables
    char parent_pub_Filename[256];
    int parent_pub_flag = 0;

    // SW Key Duplicate O/P variables
    char dupPub_Filename[256];
    int dupPub_flag = 0;
    char dupPriv_Filename[256];
    int dupPriv_flag = 0;
    char dupSymSeed_Filename[256];
    int dupSymSeed_flag = 0;
    char dupEncKey_Filename[256];
    int dupEncKey_flag = 0;
    TPM2B encryptionKey;
    TPM2B_PUBLIC swKeyPublic;
    TPM2B_PRIVATE swKeyPrivate;
    TPM2B_ENCRYPTED_SECRET encSymSeed;
    unsigned short file_size = 0;
    UINT8 policyDigest[32] = {0};
    UINT32 digestSize = 0;

    TPM2B_PUBLIC parentKeyPublicPortion;
    int pubKeysize = 0;

    // RSA key structures
    EVP_PKEY    *evpPkey = NULL;
    RSA         *rsaKey = NULL;

    setbuf(stdout, NULL);
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);
    if( (argc < 2) )
    {
        printf("Arguments count does not match \n");
        PrintHelp();
        return 1;
    }
    else
    {
        /* Get the argument values and evaluate it */
        for( count = 1; count < argc; count++ )
        {
            if( 0 == strcmp( argv[count], "-pemfile" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", pem_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                pemfile_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-pempwd" ) ) {
                count++;
                pem_pwd = argv[count];
                pempwd_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-parentPub" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", parent_pub_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                parent_pub_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-dupPub" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", dupPub_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                dupPub_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-dupPriv" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", dupPriv_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                dupPriv_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-dupSymSeed" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", dupSymSeed_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                dupSymSeed_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-dupEncKey" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", dupEncKey_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                dupEncKey_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "--help" ) ) {
                PrintHelp();
                exit(1);
            }
            else {
                PrintHelp();
                exit(1);
            }
        }
    }

    // For Duplicate functionality, check all input params are present
    if( (!pemfile_flag) ||
                (!parent_pub_flag) ||
                (!dupPub_flag) ||
                (!dupPriv_flag) ||
                (!dupSymSeed_flag) ||
                (!dupEncKey_flag)
            )
    {
        printf("Error: One or more Inputs for Duplicate are not passed as input \n");
        return -1;
    }

    printf("Input PEM file name: %s \n", pem_Filename);

    if (rval == 0) {
        rval = convert_PEM_To_EVP(&evpPkey, pem_Filename, pem_pwd);
    }
    if (rval == 0) {
        rval = convert_EVP_to_RSA(&rsaKey, evpPkey);
    }

    if ( rval == 0 )  {
        file_size = sizeof(TPM2B_PUBLIC);
        rval = loadDataFromFile(parent_pub_Filename, (UINT8 *) &parentKeyPublicPortion, &file_size);
    }

    /* SW key duplicate operation started */
    if ( rval == 0 )  {
        rval = swKeyDuplicate(rsaKey, &parentKeyPublicPortion, policyDigest, digestSize,
                &encryptionKey, &swKeyPublic, &swKeyPrivate, &encSymSeed);
        if(rval != 0) {
            printf("\nswKeyDuplicate failed: 0x%x ! \n", rval);
            goto end;
        }
        else {
            printf("\nswKeyDuplicate success: 0x%x ! \n", rval);
            rval = saveDataToFile(dupPub_Filename, (UINT8 *) &swKeyPublic, sizeof(TPM2B_PUBLIC));
            rval = saveDataToFile(dupPriv_Filename, (UINT8 *) &swKeyPrivate, sizeof(TPM2B_PRIVATE));
            rval = saveDataToFile(dupSymSeed_Filename, (UINT8 *) &encSymSeed, sizeof(TPM2B_ENCRYPTED_SECRET));
            rval = saveDataToFile(dupEncKey_Filename, (UINT8 *) &encryptionKey, sizeof(TPM2B_DATA));
            printf("\nOutput files are written successfully ! \n");
        }
    }

end:
    if (rsaKey != NULL) {
        RSA_free(rsaKey);
    }
    if (evpPkey != NULL) {
        EVP_PKEY_free(evpPkey);
    }

    return rval;
}

