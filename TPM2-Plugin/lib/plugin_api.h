
#ifndef __PLUGIN_API_H__
#define __PLUGIN_API_H__

#ifdef __cplusplus
extern "C" {
#endif

int plugin_configure(char *configPath);

void plugin_assign_hw_instance();

#ifdef __cplusplus
}
#endif

#endif

