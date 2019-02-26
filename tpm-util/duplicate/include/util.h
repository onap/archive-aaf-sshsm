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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <tss2/tss2_sys.h>
#include <tss2/tss2_common.h>
#include <tss2/tss2_esys.h>
#include <tss2/tss2_tpm2_types.h>
#include <tss2/tpm2b.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <getopt.h>


void hex_log(UINT8 *pData, UINT32 numBytes, const char* caption);

TPM2_RC ConcatSizedByteBuffer( TPM2B_MAX_BUFFER *result, TPM2B *addBuffer );

int saveDataToFile(const char *fileName, UINT8 *buf, UINT16 size);

int loadDataFromFile(const char *fileName, UINT8 *buf, UINT16 *size);

#endif
