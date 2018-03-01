#include <stdio.h>
#include "tpm2_plugin_api.h"

void main(void)
{
    unsigned long mechanish =1;
    void *param = NULL;
    size_t len = 100;
    void *ctx = NULL;

    printf("test app calling tpm2_rsa_sign_init in tpm2_plugin \n");

    tpm2_rsa_sign_init(mechanish, param, len, ctx);

    printf("tpm2_rsa_sign_init was returned from tpm2_plugin, test app will exit...  \n");

}
