#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#include "plugin_register.h"
int (*plugin_init)(char* configPath);
int (*plugin_functions_mapping)(plugin_register *plugin_fp);
int (*plugin_finalize)();
