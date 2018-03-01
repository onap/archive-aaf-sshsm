#ifndef __PLUGIN_REGISTER_H__
#define __PLUGIN_REGISTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Callback function definitions
 */

typedef int (*fp_crypto_rsa_decrypt_init) (
    /* IN */
    unsigned long mechanism,    /* PKCS#11 Mechanism */
    void *param,                /* PKCS#11 Paramter */
    unsigned long param_len,    /* PKCS#11 Parameter len */
    /* OUT */
    void *cb                   /* Address of pointer to store context block */ 
    );

typedef int (*fp_crypto_rsa_decrypt) (
    /* IN */
    void* cb,                /* Pointer Crypto Block which is created during decrypt_init */
    unsigned char* cipher,   /* Input Cipher data */
    int cipher_length,       /* Ciphet data length */
    /* OUT */
    unsigned char* out_data, /* Decrypted output data */
    int* out_data_len        /* output length */
    );

typedef int (*fp_crypto_rsa_sign_init) (
    /* IN */
    unsigned long mechanism,    /* PKCS#11 Mechanism */
    void *param,                /* PKCS#11 Paramter */
    unsigned long param_len,    /* PKCS#11 Parameter len */
    /* OUT */
    void *cb                   /* Address of pointer to store context block */ 
    );

typedef int (*fp_crypto_rsa_sign_update) (
    /* IN */
    void *cb,                   /* Previously created context block (during sign_init) passed */
    void *pPart,                /* pPart */
    unsigned long ulPartLen     /* ulPartLen */
    );

typedef int (*fp_crypto_rsa_sign_final) (
    /* IN */
    void *cb,                   /* Previously passed context block */
    /* OUT */
    unsigned char *sig,         /* Output Signature buffer */
    int *sigLen                 /* Pointer to hold signature buffer length */
    );

typedef int (*fp_crypto_rsa_sign) (
    /* IN */
    void *cb,                   /* Previously created context block (during sign_init) passed */
    unsigned char* msg,         /* Data to be signed */
    int msg_len,                /* Input data length */
    /* OUT */
    unsigned char *sig,         /* Output Signature buffer */
    int *sig_len                /* Pointer to hold signature buffer length */
    );

typedef int (*fp_crypto_ecdsa_sign) (
    /* IN */
    void *cb,                   /* Previously created context block (during sign_init) passed */
    unsigned char* data,        /* Data to be signed */
    int data_len,               /* Input data length */
    /* OUT */
    unsigned char *sig,         /* Output Signature buffer */
    int *sig_len                /* Pointer to hold signature buffer length */
    );

typedef int (*fp_crypto_ecdsa_verify) (
    /* IN */
    unsigned long appHandle,    /* Application handle needed for QAT KPT mode */
    //DhsmWPKECDSAFormat *wpk,    /* Wrapped Private Key strcuture for ECDSA */
    void *wpk,    /* Wrapped Private Key strcuture for ECDSA */
    unsigned char* swk,         /* Symmetric Wrapping Key (SWK) value */
    int swk_len,                /* SWK length */
    unsigned char* iv,          /* IV value used during Application Key encryption */
    int iv_len,                 /* IV length */
    int tag_len,                /* AES-GCM tag length */
    unsigned char* data,        /* Data which is used for signing */
    int data_len,               /* Input data length */
    unsigned char *sig,         /* Signature value */ 
    int sig_len,                /* Signature length */
    /* OUT */
    int* verifyResult           /* Pointer to hold the verification result */
    );

typedef int (*fp_crypto_del_apphandle) (unsigned long skmKeyHandle);

// SWK related operations
typedef int (*fp_crypto_swk_getParentKey) (unsigned char** tlvbuffer, int* buflen);
typedef int (*fp_crypto_swk_import) (
    unsigned long appHandle, 
    unsigned char* tlvbuffer, 
    int buflen, 
    unsigned char* iv, 
    int iv_len, 
    unsigned char* tpm_pwd, 
    int tpm_pwd_len);

typedef int (*fp_crypto_rsa_create_object) (
    unsigned long appHandle,    /* Application handle needed for QAT KPT mode */
    //DhsmWPKRSAFormat *wpk,      /* Wrapped Private Key structure for RSA */
    void *wpk,      /* Wrapped Private Key structure for RSA */
    unsigned char* swk,         /* Symmetric Wrapping Key (SWK) value */
    int swk_len,                /* SWK length */
    unsigned char* iv,          /* IV value used during Application Key encryption */
    int iv_len,                 /* IV length */
    int tag_len,                /* AES-GCM tag length */
    void **cb_object            /* Pointer to store context block */
    );

typedef int (*fp_crypto_rsa_delete_object) (
    void *cb_object             /* Pointer Crypto Block which is created during decrypt_create_object */
    );

typedef int (*fp_crypto_ecdsa_create_object) (
    unsigned long appHandle,    /* Application handle needed for QAT KPT mode */
    //DhsmWPKECDSAFormat *wpk,    /* Wrapped Private Key structure for RSA */
    void *wpk,    /* Wrapped Private Key structure for RSA */
    unsigned char* swk,         /* Symmetric Wrapping Key (SWK) value */
    int swk_len,                /* SWK length */
    unsigned char* iv,          /* IV value used during Application Key encryption */
    int iv_len,                 /* IV length */
    int tag_len,                /* AES-GCM tag length */
    void **cb_object            /* Pointer to store context block */
    );

typedef int (*fp_crypto_ecdsa_delete_object) (
    void *cb_object             /* Pointer Crypto Block which is created during decrypt_create_object */
    );


typedef struct 
{
    fp_crypto_rsa_decrypt_init     cb_crypto_rsa_decrypt_init;
    fp_crypto_rsa_decrypt          cb_crypto_rsa_decrypt;	
    fp_crypto_rsa_sign_init	   cb_crypto_rsa_sign_init;
    fp_crypto_rsa_sign_update 	   cb_crypto_rsa_sign_update;
    fp_crypto_rsa_sign_final	   cb_crypto_rsa_sign_final;
    fp_crypto_rsa_sign		   cb_crypto_rsa_sign;
    fp_crypto_ecdsa_sign	   cb_crypto_ecdsa_sign;
    fp_crypto_ecdsa_verify	   cb_crypto_ecdsa_verify;
    fp_crypto_del_apphandle	   cb_crypto_del_apphandle;
    fp_crypto_swk_getParentKey 	   cb_crypto_swk_getParentKey;
    fp_crypto_swk_import 	   cb_crypto_swk_import;
    fp_crypto_rsa_create_object    cb_crypto_rsa_create_object;
    fp_crypto_rsa_delete_object    cb_crypto_rsa_delete_object;
    fp_crypto_ecdsa_create_object  cb_crypto_ecdsa_create_object;
    fp_crypto_ecdsa_delete_object  cb_crypto_ecdsa_delete_object;

} plugin_register;


#ifdef __cplusplus
}
#endif

#endif

