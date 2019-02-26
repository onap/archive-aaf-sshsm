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

#include "util.h"
#include <tss2/tss2_tpm2_types.h>

TPM2_RC ConcatSizedByteBuffer( TPM2B_MAX_BUFFER *result, TPM2B *addBuffer )
{
    int i;

    if( ( result->size + addBuffer->size ) > TPM2_MAX_DIGEST_BUFFER )
        return TSS2_SYS_RC_BAD_VALUE;
    else
    {
        for( i = 0; i < addBuffer->size; i++ )
            result->buffer[i + result->size] = addBuffer->buffer[i];

        result->size += addBuffer->size;

        return TPM2_RC_SUCCESS;
    }
}

int saveDataToFile(const char *fileName, UINT8 *buf, UINT16 size)
{
    FILE *f;
    UINT16 count = 1;
    if( fileName == NULL || buf == NULL || size == 0 )
        return -1;

    f = fopen(fileName, "wb+");
    if( f == NULL ) {
        printf("File(%s) open error.\n", fileName);
        return -2;
    }

    while( size > 0 && count > 0 ) {
        count = fwrite(buf, 1, size, f);
        size -= count;
        buf += count;
    }

    if( size > 0 ) {
        printf("File write error\n");
        fclose(f);
        return -3;
    }

    fclose(f);
    return 0;
}

int loadDataFromFile(const char *fileName, UINT8 *buf, UINT16 *size)
{
    UINT16 count = 1, left;
    FILE *f;
    if ( size == NULL || buf == NULL || fileName == NULL )
        return -1;

    f = fopen(fileName, "rb+");
    if( f == NULL ) {
        printf("File(%s) open error.\n", fileName);
        return -2;
    }

    left = *size;
    *size = 0;
    while( left > 0 && count > 0 ) {
        count = fread(buf, 1, left, f);
        *size += count;
        left -= count;
        buf += count;
    }

    if( *size == 0 ) {
        printf("File read error\n");
        fclose(f);
        return -3;
    }
    fclose(f);
    return 0;
}
