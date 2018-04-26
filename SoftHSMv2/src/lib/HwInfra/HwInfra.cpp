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
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "HwInfra.h"
#include "hwpluginif.h"
#include "OSAttributes.h"
#include "cryptoki.h"

char hw_plugins_parent_dir[MAX_PARENT_PATH_NAME+1] = "";
char *default_hw_plugin_parent_dir = "/tmp/hwparent/";
void *g_dl_handle;
SSHSM_HW_FUNCTIONS_t g_pluginfuncs;

/**
  Function name : prepareHWPlugin
  Description:  This function is expected to be called by C_Initialize
  of softHSM.  This function does following
  -- Reads the parent directory entries
  -- If the subdirectory entry starts with 'S', then it calls loadHWPlugin
  -- if the loadHWPlugin returns failure, then it finds the next subdirectory
     that start with 'S' and calls loadHWPlugin.
**/

int prepareHWPlugin()
{
  DIR *dirhandle;
  struct dirent *entry;
  int len;
  char *env;
  int ret_val = 0;

  LOG("%s() called \n", __func__);
  /** check if there is any envrionment variable defined to represent
   ** hw plugin parent directory.
   **/
  env = getenv("SSHSM_HW_PLUGINS_PARENT_DIR");
  if (env != NULL)
  {
     len = strlen(env);
     if (len > MAX_PARENT_PATH_NAME)
     {
        LOG("SSHSM_HW_PLUGINS_PARENT_DIR environment is too long %d \n", len);
        return(SSHSM_HW_PLUGIN_ERROR_BASE + ENVIRONMENT_TOO_LONG);
     }
     strcpy(hw_plugins_parent_dir, env);
  }
  else
  {
     strcpy(hw_plugins_parent_dir, default_hw_plugin_parent_dir);
  }

  /**Read parent directory entries **/
  ret_val = -1;
  dirhandle = opendir (hw_plugins_parent_dir);
  if (dirhandle != NULL)
    {
      int count = 0;
      while (NULL != (entry = readdir (dirhandle)) )
      {
         count++;
         /**Check if it is directory **/
         if (entry->d_type == DT_DIR)
         {
             /** See if it starts with 'S' **/
             if ((entry->d_name[0] == 'S') ||
                 (entry->d_name[0] == 's') )
             {
                /** Load plugin.so file if it exists in the subdirectory
                    load it and check whether the HW is present by calling
                    init function  **/
                ret_val = loadHWPlugin( hw_plugins_parent_dir,
                                entry->d_name);
                if(ret_val == 0)
                {
                  break;
                }
             }
         }
      }
    }
  else
  {
    LOG ("Couldn't open the directory \n");
    return ret_val;
  }

  closedir(dirhandle);
  return ret_val;
}

/**
  Function name : loadHWPlugin
  Description:  It first checks whether there is plugin.so file, activate
  directory and at least one key directory. if any of them not present, then
  it returns error.  If all three are present, then it calls
  of softHSM.  It calls HwPlugin_Initiate_Activate_and_load_key() function.
**/

int loadHWPlugin(char *parent_dir, char *pluginsubdir)
{
   char fullpath[256+1];
   DIR *dirhandle;
   struct dirent *entry;
   char so_present, activate_dir_present, key_dir_present;
   hwpluginentries_t *entries;
   int ret_val = -1;

   if (strlen(parent_dir) + strlen(pluginsubdir) > 256 )
   {
     LOG("hwpluing path is too long  \n");
     return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
   }

   strcpy(fullpath, parent_dir);
   strcat(fullpath, pluginsubdir);

   dirhandle = opendir(fullpath);

   entries = (hwpluginentries_t*)malloc(sizeof(hwpluginentries_t));
   if (entries == NULL )
   {
     LOG("Could not allocate entries  \n");
     closedir(dirhandle);
     return(SSHSM_HW_PLUGIN_ERROR_BASE + ALLOCATION_ERROR);
   }
   memset(entries, 0, sizeof(hwpluginentries_t));

   if (dirhandle != NULL)
   {
      so_present = 0;
      activate_dir_present = 0;
      key_dir_present = 0;
      while (NULL != (entry = readdir (dirhandle)) )
      {
         /** Ensure that the directory has plugin.so file, activate directory,
          ** at least one key directory
          **/

          if ((entry->d_type == DT_REG) &&
              (strcmp(entry->d_name, "plugin.so") == 0))
          {
             so_present = 1;
             if (strlen(fullpath) + strlen("/")+ strlen(entry->d_name) > 256)
             {
                LOG("plugin so path is too long  \n");
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                break;
             }
             strcpy(entries->so_full_path, fullpath);
             strcat(entries->so_full_path, "/");
             strcat(entries->so_full_path, entry->d_name);
          }

          if ((entry->d_type == DT_DIR) &&
              (strcmp(entry->d_name, "activate") == 0 ))
          {
             activate_dir_present = 1;
             if (strlen(fullpath) + 2*strlen("/")+ strlen(entry->d_name) > 256)
             {
                LOG("activate path is too long  \n");
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                break;
             }
             strcpy(entries->activate_dir_full_path, fullpath);
             strcat(entries->activate_dir_full_path, "/");
             strcat(entries->activate_dir_full_path, entry->d_name);
             strcat(entries->activate_dir_full_path, "/");
          }

          if ((entry->d_type == DT_DIR) &&
              (strncmp(entry->d_name, "key", 3) == 0 ))
          {
             key_dir_present = 1;
             if (strlen(fullpath) + 2*strlen("/")+ strlen(entry->d_name) > 256)
             {
                LOG("activate path is too long  \n");
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                break;
             }
             strcpy(entries->key_dir_full_path[entries->num_key_dirs],
                             fullpath);
             strcat(entries->key_dir_full_path[entries->num_key_dirs], "/");
             strcat(entries->key_dir_full_path[entries->num_key_dirs],
                              entry->d_name);
             strcat(entries->key_dir_full_path[entries->num_key_dirs], "/");
             entries->num_key_dirs++;
          }

          if (so_present && activate_dir_present && key_dir_present)
          {
              printf("so dir path: %s \n", entries->so_full_path);
              printf("activate dir path: %s \n", entries->activate_dir_full_path);
              ret_val = HwPlugin_Initiate_Activate_and_load_keys(entries);
              break;
          }
      }

      if (!so_present || !activate_dir_present || !key_dir_present)
      {
          LOG("Minimum set of entries not present hwplugin dir plugindir %s so_present %d activate present %d key present %d \n", fullpath,  so_present, activate_dir_present, key_dir_present);
          return(SSHSM_HW_PLUGIN_ERROR_BASE + INCOMPLETE_PLUGIN_DIR);
      }
   }
   else
   {
     LOG("Could not open hwplugin directory %s \n", fullpath);
     return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_OPEN_ERROR);
   }
   free(entries);
   closedir(dirhandle);
   return(ret_val);
}


/**
** Function name: HWPlugin_Initiate_Activate_and_load_keys
** Description:  This function loads plugin, gets the function pointers,
** activates the plugin and then finally loads the keys
**/
int HwPlugin_Initiate_Activate_and_load_keys(hwpluginentries_t *entries)
{
     int ret_val;

     ret_val = load_hw_plugin_and_get_function_pointers(entries->so_full_path,
                   &g_pluginfuncs);
     if(ret_val != 0)
        return(ret_val);

     ret_val = init_hw_plugin(&g_pluginfuncs);
     if(ret_val != 0 )
        return(ret_val);

     ret_val = activate_hw_plugin(entries, &g_pluginfuncs);
     if (ret_val != 0 )
       return(ret_val);

     ret_val = load_keys_in_hw_plugin(entries, &g_pluginfuncs);
     if (ret_val != 0 )
       return(ret_val);

     return(0);
}


/**
  Function name : load_hw_plugin_and_get_function_pointers
**/

int load_hw_plugin_and_get_function_pointers(char *so_path,
              SSHSM_HW_FUNCTIONS_t *funcs)
{
   int (*functogetpluginfuncs)(SSHSM_HW_FUNCTIONS_t *fs);
   int ret_val;

   g_dl_handle = dlopen(so_path, RTLD_NOW);
   if(g_dl_handle == NULL )
   {
       LOG("dlopen on %s failed: %s \n", so_path, dlerror());
       return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_DL_OPEN_ERROR);
   }

   functogetpluginfuncs = NULL;
   functogetpluginfuncs = (int (*)(SSHSM_HW_FUNCTIONS_t *)) dlsym(g_dl_handle,
             "sshsm_hw_plugin_get_plugin_functions");

   if (functogetpluginfuncs == NULL)
   {
       LOG("dlsym of sshsm_hw_plugin_get_plugin_functions : %s \n", dlerror() );
       return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_DL_SYM_ERROR);
   }

   ret_val = functogetpluginfuncs(funcs);

    return ret_val;
}

int init_hw_plugin(SSHSM_HW_FUNCTIONS_t *funcs)
{
    int ret_val;

    ret_val = (funcs->xxx_init)();

    if(ret_val != 0 )
    {
       LOG("HWPlugin init failed \n" );
       return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);

    }
    return(ret_val);
}


int activate_hw_plugin(hwpluginentries_t *entries, SSHSM_HW_FUNCTIONS_t *funcs)
{
   int ret_val;

   if( (entries == NULL) || (funcs == NULL) )
   {
     ret_val = -1;
     LOG("activate_hw_plugin: Input values are NULL \n");
     return ret_val;
   }
   /** Read all files starting with 'A' and pass the information to
    ** plugin
    **/

   SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t comp_buffers;

   memset(&comp_buffers, 0, sizeof(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t));

   ret_val = get_all_file_contents(entries->activate_dir_full_path, 'A',
                       &comp_buffers);

   if (ret_val == 0 )
   {
     ret_val = (funcs->xxx_activate)(&comp_buffers);
     //free_buffers(&comp_buffers);
   }

   return(ret_val);
}


int load_keys_in_hw_plugin(hwpluginentries_t *entries,
                   SSHSM_HW_FUNCTIONS_t *funcs)
{

    int ret_val;
    void *key_handle;
    int ii;
    //unsigned long hwkeyhandle=987654321;
    //key_handle = (void *) &hwkeyhandle;

    SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t comp_buffers;

    /**
     Travese through all key directories and load the key in plugin
    **/

    ret_val = -1;
    for(ii = 0; ii < entries->num_key_dirs; ii++)
    {
       memset(&comp_buffers, 0,
          sizeof(SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t));

       ret_val = get_all_file_contents(entries->key_dir_full_path[ii], 'K',
                        &comp_buffers);

       if(ret_val == 0)
       {
           ret_val = (funcs->xxx_load_key)(&comp_buffers, &key_handle);
           //free_buffers(&comp_buffers);
           if(ret_val == 0)
           {
               /** Get PKCS11 information **/
               /** Call SoftHSM functions to create private key object */
               ret_val = program_pkcs11_info(entries->key_dir_full_path[ii], &key_handle);
           }
       }

    }

    return(ret_val);
}

int get_all_file_contents(char *dirpath, char starting_char,
                  SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *c_buffers )
{
   DIR *dirhandle;
   struct dirent *entry;

   buffer_info_t *buffer;
   char *token;

   struct stat st;
   int fd;

   int ret_val = 0;


   char fullpath[256+1];

   dirhandle = opendir(dirpath);
   if (dirhandle != NULL)
   {
      while (NULL != (entry = readdir (dirhandle)))
      {
         if ((entry->d_type == DT_REG) &&
              (entry->d_name[0] == starting_char))
         {
             buffer = (buffer_info_t*) malloc(sizeof(buffer_info_t));
             if (buffer == NULL )
             {
               LOG("Could not allocate entries  \n");
               ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + ALLOCATION_ERROR);
               break;
             }
             token = strchr(entry->d_name, '.');
             strcpy(buffer->id, token+1);

             /** get full path of the file **/
             if ((strlen(dirpath) + strlen(entry->d_name)) > 256)
             {
                LOG("file  path is too long  \n");
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                free(buffer);
                break;
             }
             strcpy(fullpath,dirpath);
             strcat(fullpath, entry->d_name);
             stat(fullpath, &st);
             buffer->buffer = (unsigned char*) malloc(st.st_size);
             if(buffer->buffer == NULL)
             {
               LOG("Could not allocate entries  \n");
               ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + ALLOCATION_ERROR);
               free(buffer);
               break;
             }
             buffer->length_of_buffer = st.st_size;
             fd = open(fullpath, O_RDONLY);
             if (fd == -1 )
             {
               LOG("Could not open file %s  \n", fullpath);
               ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + ALLOCATION_ERROR);
               free(buffer->buffer);
               free(buffer);
               break;
             }

             if(read(fd, buffer->buffer, st.st_size) < 0)
             {
                LOG("Reading from file %s failed \n", fullpath);
                continue;
             }

             close(fd);

             /** Now write this buffer in c_buffers **/
             c_buffers->buffer_info[c_buffers->num_buffers] = buffer;
             c_buffers->num_buffers++;

         }
      }
   }
   else
   {
     LOG("Could not open hwplugin directory %s \n", dirpath);
     return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_OPEN_ERROR);
   }

   closedir(dirhandle);
   //if (ret_val != 0 )
     //free_buffers(c_buffers);

   return(ret_val);
}

void free_buffers ( SSHSM_HW_PLUGIN_ACTIVATE_LOAD_IN_INFO_t *c_buffers )
{
   int ii;

   for(ii = 0; ii < c_buffers->num_buffers; ii++)
   {
      free(c_buffers->buffer_info[ii]->buffer);
      free(c_buffers->buffer_info[ii]);
   }
}

int program_pkcs11_info (char *dirpath, void *key_handle)
{
   DIR *dirhandle;
   struct dirent *entry;

   char fullpath[256+1];
   int ret_val = 0;

   FILE *fp;
   char buffer[80+1];

   unsigned int  slot_id = 0;
   unsigned char upin[64+1];
   int upin_len = 0;
   unsigned char keyid[64+1];
   int key_id_len = 0;
   unsigned char key_label[64+1] = "";
   char *valuep;
   char *endvalue;


   dirhandle = opendir(dirpath);
   if (dirhandle != NULL)
   {
      while (NULL != (entry = readdir (dirhandle)))
      {
           if (strcmp(entry->d_name, "pkcs11.cfg") == 0 )
           {
              /** get full path of the file **/
               if ((strlen(dirpath) + strlen(entry->d_name)) > 256)
             {
                LOG("file  path is too long  \n");
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                break;
             }
             strcpy(fullpath,dirpath);
             strcat(fullpath, entry->d_name);

             fp = fopen(fullpath, "r");
             if(fp == NULL )
             {
                ret_val = (SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_PATH_TOO_LONG);
                break;
             }
             while (fgets(buffer, 80, fp) != NULL)
             {
                 valuep = strchr(buffer, ':');
                 if(valuep == NULL)
                    continue;
                 valuep[0] = '\0';

                 /** Looks like \n is part of buffer that is read via fgets
                  ** Replacce tha with 0 **/
                 endvalue = strchr(valuep+1, '\n');
                 if(endvalue != NULL)
                    endvalue[0] =  '\0';
                 if (strcmp(buffer, "slot") == 0)
                 {
                    slot_id = strtoul(valuep+1, NULL, 10);
                    continue;
                 }
                 if(strcmp(buffer, "key_id") == 0 )
                 {
                    strcpy((char*)keyid, valuep+1);
                    key_id_len = strlen((char*)keyid);
                    continue;
                 }
                 if(strcmp(buffer, "key_label") == 0 )
                 {
                    strcpy((char*)key_label, valuep+1);
                    continue;
                 }
                 if(strcmp(buffer, "upin") == 0 )
                 {
                    strcpy((char*) upin, valuep+1);
                    upin_len = strlen((char *) upin);
                    continue;
                 }
             }
             fclose(fp);

             /** Program key in SoftHSM **/
             ret_val = PrepareKeyInSoftHSM(slot_id, upin, upin_len, keyid,
                           key_id_len, key_label, key_handle);

              break;
           }

      }
   }

   return ret_val;
}


void long_to_byte_string(const unsigned long longValue, unsigned char *out, size_t *outlen)
{
        unsigned long setValue = longValue;
        unsigned char byteStrIn[8];
        size_t i;

        for (i = 0; i < 8; i++)
        {
            byteStrIn[7-i] = (unsigned char) (setValue & 0xFF);
            setValue >>= 8;
        }
        for (i = 0; i < 8; i++)
        {
            if (byteStrIn[i])
               break;
        }
        memcpy(out, &byteStrIn[i], 8-i);
        *outlen = 8-i;
}



/*** PrepareKeyInSoftHSM
** Description:  It creates the object in softhsm with given key id and
** key label and also stores the keyhandle that was returned by hardware plugin
** Inputs:
**   -  Slot ID
**   -  Key ID
**   -  Key Label
**   -  upin
**   -  pluginkeyHandle
** Output:
**    - None
** Renturs
**     - SUCCESS or FAILURE
*****/
int PrepareKeyInSoftHSM(unsigned int slot_id,
               unsigned char *upin, int upin_len,
               unsigned char *key_id, int key_id_len,
               unsigned char *key_label, void *key_handle)
{
    CK_SESSION_HANDLE hSession;
    CK_RV ret_val;
    int ii;
    CK_OBJECT_HANDLE hKey;
    unsigned char key_handle_str[32] = {0};

    printf ("slot %ul upin %s key_id %s key_label %s \n", slot_id, upin, key_id,
             key_label);
    if(!key_handle)
    {
        printf("Input Key handle is NULL ! \n");
        return (SSHSM_HW_PLUGIN_ERROR_BASE + INVALID_KEY_ERROR);
    }

    /** For creating the key object, first the session needs to be opened
        C_OpenSession is used to open the session
     **/
    ret_val = C_OpenSession(slot_id, CKF_SERIAL_SESSION | CKF_RW_SESSION,
                            NULL_PTR, NULL_PTR, &hSession);

    if (ret_val != CKR_OK)
    {
        printf("OpenSession failed for slot %x \n", slot_id);
        return(ret_val);
    }

    /** Next step is login
     ** C_Login is used to login to the session
     **/
    ret_val = C_Login(hSession, CKU_USER,  upin, upin_len);
    if (ret_val != CKR_OK)
    {
        printf("Login failed: 0x%lx | for slot %x upin below \n", ret_val, slot_id);
        for (ii = 0; ii < upin_len; ii++ )
           printf("%2x  %c \n",  upin[ii], upin[ii]);
        return(ret_val);
    }

    CK_OBJECT_CLASS privClass = CKO_PRIVATE_KEY;
    CK_KEY_TYPE keyType = CKK_RSA;
    CK_BBOOL ckTrue = CK_TRUE, ckFalse = CK_FALSE ;

    unsigned long int key_id_int = atol( (const char*) key_id );
    unsigned char byte_str[8];
    size_t outlen;
    long_to_byte_string(key_id_int, byte_str, &outlen);

    //Add a Dummy key for now
    CK_BYTE pN[] = { 0xC6, 0x47, 0xDD, 0x74, 0x3B, 0xCB, 0xDC, 0x6F, 0xCE, 0xA7,
                         0xF0, 0x5F, 0x29, 0x4B, 0x27, 0x00, 0xCC, 0x92, 0xE9, 0x20,
                         0x8A, 0x2C, 0x87, 0x36, 0x47, 0x24, 0xB0, 0xD5, 0x7D, 0xB0,
                         0x92, 0x01, 0xA0, 0xA3, 0x55, 0x2E, 0x3F, 0xFE, 0xA7, 0x4C,
                         0x4B, 0x3F, 0x9D, 0x4E, 0xCB, 0x78, 0x12, 0xA9, 0x42, 0xAD,
                         0x51, 0x1F, 0x3B, 0xBD, 0x3D, 0x6A, 0xE5, 0x38, 0xB7, 0x45,
                         0x65, 0x50, 0x30, 0x35 };
    CK_BYTE pD[] = { 0x6D, 0x94, 0x6B, 0xEB, 0xFF, 0xDC, 0x03, 0x80, 0x7B, 0x0A,
                         0x4F, 0x0A, 0x98, 0x6C, 0xA3, 0x2A, 0x8A, 0xE4, 0xAA, 0x18,
                         0x44, 0xA4, 0xA5, 0x39, 0x37, 0x0A, 0x2C, 0xFC, 0x5F, 0xD1,
                         0x44, 0x6E, 0xCE, 0x25, 0x9B, 0xE5, 0xD1, 0x51, 0xAF, 0xA8,
                         0x30, 0xD1, 0x4D, 0x3C, 0x60, 0x33, 0xB5, 0xED, 0x4C, 0x39,
                         0xDA, 0x68, 0x78, 0xF9, 0x6B, 0x4F, 0x47, 0x55, 0xB2, 0x02,
                         0x00, 0x7E, 0x9C, 0x05 };


    CK_ATTRIBUTE keyTemplate[] = {
        { CKA_CLASS,            &privClass,         sizeof(privClass) },
        { CKA_KEY_TYPE,         &keyType,           sizeof(keyType) },
        { CKA_LABEL,            key_label,          strlen((char *) key_label) },
        { CKA_ID,               byte_str,           outlen },
        { CKA_SIGN,             &ckTrue,            sizeof(ckTrue) },
        { CKA_DECRYPT,          &ckTrue,            sizeof(ckTrue) },
        { CKA_UNWRAP,           &ckFalse,           sizeof(ckFalse) },
        { CKA_TOKEN,            &ckTrue,            sizeof(ckTrue) },
        { CKA_PRIVATE,          &ckTrue,            sizeof(ckTrue) },
        { CKA_EXTRACTABLE,      &ckFalse,           sizeof(ckFalse) },
        { CKA_SENSITIVE,        &ckFalse,           sizeof(ckFalse) },
        { CKA_PUBLIC_EXPONENT,  0,                  0},
        { CKA_MODULUS,          pN,                 sizeof(pN) },
        { CKA_PRIVATE_EXPONENT, pD,                 sizeof(pD) },
        { CKA_PRIME_2,          0,                  0},
        { CKA_EXPONENT_1,       0,                  0},
        { CKA_EXPONENT_2,       0,                  0},
        { CKA_COEFFICIENT,      0,                  0},
        { CKA_OS_PRIVATE_HANDLE,       (CK_VOID_PTR ) *((CK_ULONG*)key_handle),      sizeof(CK_ULONG) }
    };


    CK_OBJECT_HANDLE hObject;
    CK_ULONG ulObjectCount;
    CK_RV rv;

    rv = C_FindObjectsInit(hSession, keyTemplate, 0);
    if(rv != CKR_OK) {
        LOG ("C_FindObjectsInit rv %ld\n", rv);
    }
    rv = C_FindObjects(hSession, &hObject, 16, &ulObjectCount);
    printf("PrepareKeyInSoftHSM: ulObjectCount %ld\n", ulObjectCount);
    if(rv != CKR_OK || ulObjectCount == 0) {
        ret_val =  C_CreateObject(hSession, keyTemplate,
                 sizeof(keyTemplate)/sizeof(CK_ATTRIBUTE),&hKey);
        if (ret_val != CKR_OK)
        {
            printf("CreateObject failed: 0x%lx | for slot %x | keylabel %s | keyid below \n",
                ret_val, slot_id, key_label);
            for (ii = 0; ii < key_id_len; ii++ )
               printf("%2x  %c \n",  key_id[ii], key_id[ii]);
        //return(ret_val);
        }
    }
    else {
        printf("PrepareKeyInSoftHSM: Object already exists\n");
    }

    rv = C_FindObjectsFinal(hSession);
    if(rv != CKR_OK) {
        LOG ("C_FindObjectsFinal rv %ld\n", rv);
    }

    ret_val = C_Logout(hSession);
    if (ret_val != CKR_OK)
    {
        printf("Logout failed 0x%lx | for slot %x \n", ret_val, slot_id);
        return(ret_val);
    }

    ret_val = C_CloseSession(hSession);
    if (ret_val != CKR_OK)
    {
        printf("C_CloseSession failed for slot %x \n", slot_id);
        return(ret_val);
    }

    return ret_val;
}

int HwInfraSignInit(void *keyHandle,  unsigned long mechanism,
                 void* param, int paramLen, void **hwCryptoOpaque)
{
    if (g_pluginfuncs.xxx_rsa_sign_init == NULL)
        return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);
    
    return (g_pluginfuncs.xxx_rsa_sign_init(keyHandle, mechanism, param, 
                                                 paramLen, hwCryptoOpaque)) ;

}

int HwInfraSign(void *keyHandle,  unsigned long mechanism,
                 unsigned char *msg, int msg_len, void *hwCryptoOpaque,
                 unsigned char *outsig,  int *outsiglen)
{
    if (g_pluginfuncs.xxx_rsa_sign == NULL)
        return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);

    return ( g_pluginfuncs.xxx_rsa_sign(keyHandle, mechanism, msg, msg_len,
                hwCryptoOpaque, outsig, outsiglen) );
}

int HwInfraSignUpdate(void *keyHandle, 
                 void *hwCryptoOpaque, unsigned char *param, int paramLen)
{
    if (g_pluginfuncs.xxx_rsa_sign_update == NULL)
        return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);

    return ( g_pluginfuncs.xxx_rsa_sign_update(keyHandle, param, 
                                               paramLen, hwCryptoOpaque) );
}

int HwInfraSignFinal(void *keyHandle,  
                 unsigned char *msg, int msg_len, void *hwCryptoOpaque,
                 unsigned char *outsig,  int *outsiglen)
{
    if (g_pluginfuncs.xxx_rsa_sign_final == NULL)
        return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);

    return ( g_pluginfuncs.xxx_rsa_sign_final(keyHandle, 
                                     hwCryptoOpaque, outsig, outsiglen) );
}

int HwInfraSignCleanup(void *keyHandle,
                       void *hwCryptoOpaque)
{
    if (g_pluginfuncs.xxx_rsa_sign_cleanup == NULL)
        return(SSHSM_HW_PLUGIN_ERROR_BASE + PLUGIN_INIT_ERROR);

    return ( g_pluginfuncs.xxx_rsa_sign_cleanup(keyHandle,  
                                                hwCryptoOpaque) );
}

