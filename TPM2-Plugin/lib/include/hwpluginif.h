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

#ifndef __SSHSM_HW_PLUGIN_IF_H__
#define __SSHSM_HW_PLUGIN_IF_H__


#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_ID_LENGTH (32)

typedef struct buffer_info_s{
       char id[MAX_ID_LENGTH+1];
       int length_of_buffer;
       unsigned char *buffer;
    }buffer_info_t;

/***
 * Init Callback
 * Description:
 * This function is called by HWPluginInfra as part of C_Initialize to figure
 * out whether there is any correspnding HW is present to use this plugin.
 * In case of TPM2.0 Plugin,
 *  it is expected that this function checks
 *  whether the TPM2.0 is present or not, by checking the capabilities
 *  using Tss2_Sys_GetCapability with TPM_CAP_TPM_PROPERTIES and
 *  TPM_PT_MANUFACTURER property. If this function returns SUCCESS,
 *  TPM plguin can assume that TPM2.0 is presenta nd return success
 * In case of SGX Plugin: <To be filled>
 * Parameters:
 *    Inputs: None
 *    OUtputs; None
 *    Returns :  SUCCESS (if HW is present), FAILURE if HW is not present
 *
 ***/
typedef int (*sshsm_hw_plugin_init)();

/***
 * UnInit Callback
 * Description: This function is called by HWPluginInfra during C_Finalize().
 * This functin is gives chance for any cleanup by plugins.
 ***/
typedef int (*sshsm_hw_plugin_uninit)();

/***
 * Activate Callback
 * Description: This callback function is called by HWPluginInfra
 * (as part of C_Intialize)  to activate the
 * HW via HW plugin. SofHSM HWPluginInfra reads set of files required for
 * activation (from
 * activation directory) and passes them as buffers.
 * HWPluginInfra reads the file in 'activate directory'
 * as part of C_Initialize and passes the file content as is
 * to the activate callback function.
 * If there are two files, then num_buffers in in_info would be 2.
 * 'id' is name of the file (May not be used by TPM plugin)
 * 'length_of_buffer' is the valid length of the buffer.
 * 'buffer' contains the file content.
 * HWPluginInfra in SoftHSM allocates memory for this structure and internal
 * buffers and it frees them up after this function returns. Hence,
 * the plugin should not expect that these buffers are valid after the call
 * is returned.
 *
 * In case of TPM Plugin:
 *    It is expected that activate directory has a file with SRK Handle
 *    saved in it. Note that SRK is saved in TPM memory (persistence)
 *    Actiate function of TPM plugin is called with SRK handle.
 *
 ***/

#define MAX_BUFFER_SEGMENTS 8
typedef struct sshsm_hw_plugin_activate_in_info_s {
    int num_buffers;
    buffer_info_t *buffer_info[MAX_BUFFER_SEGMENTS];
}SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t;

typedef struct sshsm_hw_plugin_import_public_key_info_s {
    unsigned long modulus_size;
    unsigned char *modulus;
    unsigned long exponent_size;
    //unsigned char *exponent;
    unsigned int *exponent;
}SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t;

typedef int (*sshsm_hw_plugin_activate)(
           SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *activate_in_info
        );

/***
 * Load Key  Callback
 * Description: This callback function is called by SoftHSM HWPluginInfra
 * to load private keys into the HW using HW plugin.
 * Each HW plugin expects the keys to be specific to its HW.
 * Since SoftHSM HWPluginInfra is expected to be generic, the design
 * chosen is that HWPluginInfra reads key content from files and pass
 * that information to HW Plugins via this function pointer.
 * Yet times, Key information for HW Plugins is exposed as multiple files.
 * Hence, HWPluginInfra reads multiple files for each key.  Since, there
 * could be multiple keys, each set of files that correspond to one key
 * is expected to have same file name, but with different extensions. Since
 * the directory holding these file may also need to have other files
 * related to key, but for PKCS11, it is expected that all HWPlugin related
 * files should have its name start with HW.
 *
 * HWPluginInfra calls this callback function as many timne as number of
 * distinct keys.  For each distinct key, it reads the HW tagged files, loads
 * them into the buffer pointers and calls the HW Plugin -loadkey- function.
 * HWPluginInfra also stores the any returned buffers into the SoftHSM key
 * object.
 *
 * In case of TPM Plugin, it does following:
 *
 * -- Gets the buffers in in_info structure.
 *    --- Typically, there are two buffers in TPM understandable way
 *    - public & private key portion
 *    --- From global variables, it knows SRKHandle, SAPI context.
 *    --- Using Tss2_Sys_Load(), it loads the key.
 *
 * -- In both cases, it also expected to return KeyHandle, which is
 *    keyObjectHandle in case of TPM.
 *
 *
 ***/

typedef int (*sshsm_hw_plugin_load_key)(
           SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *loadkey_in_info,
           void **keyHandle,
           SSHSM_HW_PLUGIN_IMPORT_PUBLIC_KEY_INFO_t *importkey_info
        );

typedef int (*sshsm_hw_plugin_unload_key)(
           void **keyHandle
        );

/***
 * Callback:  RSA Sign Init
 * Description: This is called by HWPluginInfra as part of C_SignInit function
 * for RSA keys
 */

typedef int (*sshsm_hw_plugin_rsa_sign_init)(
         void *keyHandle,
         unsigned long mechanism,
         void *param,
         int len
        );

/***
 * Callback:  RSA Sign Init
 * Description: This is called by HWPluginInfra as part of C_Sign function
 * for RSA keys. HWPluginInfra get the keyHandle from the key object.
 *
 * In case of TPM plugin, it does following:
 * -- TSS2_Sys_Sing function is called.
 *
 *
 */

typedef int (*sshsm_hw_plugin_rsa_sign)(
         void *keyHandle,
         unsigned long mechanism,
         unsigned char *msg,
         int msg_len,
         unsigned char *outsig,
         int *outsiglen
        );

/***
 * Function Name: sshsm_hw_plugin_get_plugin_functions
 * Descrpiton:  Every HW plugin is expected to define this function.
 * This function is expected to return its function as pointers to the
 * caller.
 * SoftHSM calls this function after loading the hw plugin .SO file.
 * SoftHSM calls this function as part of C_initialize.
 * Arugments:
 *  Outputs: funcs
 *  Inputs: None
 *  Return value:  SUCCESS or FAILURE
 *
 ***/

typedef struct sshsm_hw_functions_s
{
    sshsm_hw_plugin_init  xxx_init;
    sshsm_hw_plugin_uninit  xxx_uninit;
    sshsm_hw_plugin_activate xxx_activate;
    sshsm_hw_plugin_load_key xxx_load_key;
    sshsm_hw_plugin_unload_key xxx_unload_key;
    sshsm_hw_plugin_rsa_sign_init  xxx_rsa_sign_init;
    sshsm_hw_plugin_rsa_sign xxx_rsa_sign;
}SSHSM_HW_FUNCTIONS_t;

int sshsm_hw_plugin_get_plugin_functions(SSHSM_HW_FUNCTIONS_t *funcs);

#if defined(__cplusplus)
}
#endif

#endif

