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

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "hwpluginif.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __SSHSM_HW_INFRA_UTIL_H__
#define __SSHSM_HW_INFRA_UTIL_H__

#define MAX_KEY_DIRS_SUPPORTED 64

typedef struct hwpluginentries_s {
   char so_full_path[256+1];
   char activate_dir_full_path[256+1];
   int num_key_dirs;
   char key_dir_full_path[MAX_KEY_DIRS_SUPPORTED][256+1];
}hwpluginentries_t;

int prepareHWPlugin();
int loadHWPlugin(char *parent_dir, char *pluginsubdir);
int HwPlugin_Initiate_Activate_and_load_keys( hwpluginentries_t *);
int load_hw_plugin_and_get_function_pointers(char *,
              SSHSM_HW_FUNCTIONS_t *funcs);
int init_hw_plugin(SSHSM_HW_FUNCTIONS_t *funcs);
int activate_hw_plugin(hwpluginentries_t *entries, SSHSM_HW_FUNCTIONS_t *funcs);
int load_keys_in_hw_plugin(hwpluginentries_t *entries,
                   SSHSM_HW_FUNCTIONS_t *funcs);
int get_all_file_contents(char *dirpath,  char starting_char,
                  SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *c_buffers );

void free_buffers ( SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *c_buffers );
int program_pkcs11_info (char *dirpath, void *key_handle);
int PrepareKeyInSoftHSM(unsigned int slot_id,
               unsigned char *upin, int upin_len,
               unsigned char *key_id, int key_id_len,
               unsigned char *key_label, void *key_handle);

int HwInfraSignInit(void *keyHandle,  unsigned long mechanism,
                 void* param, int paramLen);

int HwInfraSign( void *keyHandle,  unsigned long mechanism,
                 unsigned char *msg, int msg_len,
                 unsigned char *outsig,  int *outsiglen);

#define MAX_PARENT_PATH_NAME 256

#define LOG printf
#define SSHSM_HW_PLUGIN_ERROR_BASE (10000)

#define ENVIRONMENT_TOO_LONG (01)
#define PLUGIN_PATH_TOO_LONG (02)
#define PLUGIN_PATH_OPEN_ERROR (03)
#define INCOMPLETE_PLUGIN_DIR (04)
#define ALLOCATION_ERROR (05)
#define PLUGIN_DL_OPEN_ERROR (06)
#define PLUGIN_DL_SYM_ERROR (07)
#define PLUGIN_INIT_ERROR (10)

#if defined(__cplusplus)
}
#endif


#endif /* __SSHSM_HW_INFRA_UTIL_H__ */


