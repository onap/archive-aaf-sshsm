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
// main.c : Tool to import Openssl RSA key into TPM
// Author: Arun Kumar Sekar
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sapi/tpm20.h>

#include "tpm_wrapper.h"
#include "util.h"

void PrintHelp();
char version[] = "0.1";

void PrintHelp()
{
    printf(
            "OSSL key to tpm import tool, Version %s\nUsage:"
            "./ossl_tpm_import "
            "[-dupPub out_dupPubFile] [-dupPriv out_dupPrivFile] [-dupSymSeed out_dupSymSeedFile] "
            "[-dupEncKey out_dupEncKeyFile] [-password keyPassword] "
            "[-pub out_keyPub] [-priv out_KeyPriv] [-H primaryKeyHandle]\n"
			"\n"
			 , version);
}

int main(int argc, char* argv[])
{
    TPM_RC rval = 0;
    int count=0;
    TSS2_TCTI_CONTEXT *tcti_ctx = 0;
    TSS2_SYS_CONTEXT  *sysContext = 0;

    // SW Key Duplicate O/P variables
    char dupPub_Filename[256];
    int dupPub_flag = 0;
    char dupPriv_Filename[256];
    int dupPriv_flag = 0;
    char dupSymSeed_Filename[256];
    int dupSymSeed_flag = 0;
    char dupEncKey_Filename[256];
    int dupEncKey_flag = 0;
    char keyPassword[256];
    int keyPassword_flag = 0;
    TPM2B_DATA encryptionKey;
    TPM2B_PUBLIC swKeyPublic;
    TPM2B_PRIVATE swKeyPrivate;
    TPM2B_ENCRYPTED_SECRET encSymSeed;

    // SW Key Import O/P variables
    char pub_Filename[256];
    int pub_flag = 0;
    char priv_Filename[256];
    int priv_flag = 0;
    unsigned short file_size = 0;

    TPM_HANDLE primaryKeyHandle = 0;
    int H_flag = 0;

    TPM2B_PUBLIC parentKeyPublicPortion;
    int pubKeysize = 0;

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
            if( 0 == strcmp( argv[count], "-dupPub" ) ) {
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
            else if( 0 == strcmp( argv[count], "-password" ) ) {
                count++;
                if ( (1 != sscanf(argv[count], "%s", keyPassword )) )
                {
                    PrintHelp();
                    return 1;
                }
                keyPassword_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-pub" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", pub_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                pub_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-priv" ) ) {
                count++;
                if( (1 != sscanf( argv[count], "%s", priv_Filename )) )
                {
                    PrintHelp();
                    return 1;
                }
                priv_flag = 1;
            }
            else if( 0 == strcmp( argv[count], "-H" ) ) {
                count++;
                primaryKeyHandle = strtoul(argv[count], NULL, 16);
                printf("Primary Key handle Given: 0x%x \n", primaryKeyHandle);
                H_flag = 1;
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

    if((!H_flag)) {
        printf("Parent handle should be passed for TPM import operation \n");
        return -1;
    }

    // For TPM Import functionality, check all input params are present
    if( (!dupPub_flag) ||
        (!dupPriv_flag) ||
        (!dupSymSeed_flag) ||
        (!dupEncKey_flag) ||
        (!keyPassword_flag) ||
        (!pub_flag) ||
        (!priv_flag)
        ) {
        printf("Error: One or more Inputs for TPM import functionality is missing ! \n");
        return -1;
    }

    /* SW Key TPM Import operation started */
    if(rval == 0) {
        file_size = sizeof(TPM2B_PUBLIC);
        rval = loadDataFromFile(dupPub_Filename, (UINT8 *) &swKeyPublic, &file_size);
        if ( rval == 0 )  {
            file_size = sizeof(TPM2B_PRIVATE);
            rval = loadDataFromFile(dupPriv_Filename, (UINT8 *) &swKeyPrivate, &file_size);
        }
        if ( rval == 0 )  {
            file_size = sizeof(TPM2B_ENCRYPTED_SECRET);
            rval = loadDataFromFile(dupSymSeed_Filename, (UINT8 *) &encSymSeed, &file_size);
        }
        if ( rval == 0 )  {
            file_size = sizeof(TPM2B_DATA);
            rval = loadDataFromFile(dupEncKey_Filename, (UINT8 *) &encryptionKey, &file_size);
        }

        if ( rval == 0 ) {
            /* Initialize TCTI and sapi context */
            tcti_ctx = tpm_tcti_tabrmd_init();
            if(tcti_ctx == NULL) {
                printf("Creation of TCTI context with TABRMD failed ! \n");
                goto end;
            }

            sysContext = sys_ctx_init(tcti_ctx);
            if(sysContext == NULL) {
                printf("Creation of SAPI context with TABRMD failed ! \n");
                goto end;
            }
            printf("\nInitializing TPM context success: 0x%x ! \n", rval);
        }

        TPM2B_PRIVATE importPrivate;
        INIT_SIMPLE_TPM2B_SIZE(importPrivate);
        rval = swKeyTpmImport(sysContext, primaryKeyHandle,
                &encryptionKey, &swKeyPublic, &swKeyPrivate, &encSymSeed,
                keyPassword, strlen(keyPassword),
                &importPrivate);
        if(rval != 0) {
            printf("\nswKeyTpmImport failed: 0x%x ! \n", rval);
            goto end;
        }
        else {
            printf("\nswKeyImport success: 0x%x ! \n", rval);
            saveDataToFile(pub_Filename, (UINT8 *) &swKeyPublic, sizeof(TPM2B_PUBLIC));
            saveDataToFile(priv_Filename, (UINT8 *) &importPrivate, sizeof(TPM2B_PRIVATE));
            printf("\nOutput files are written successfully ! \n");
        }
    }

end:
    if(sysContext) {
        TeardownSysContext(&sysContext);
    }
    if(tcti_ctx) {
        TeardownTctiContext(tcti_ctx);
    }

    return rval;
}
