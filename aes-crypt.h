
#ifndef AES_CRYPT_H
#define AES_CRYPT_H

#include <stdio.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/aes.h>

int do_crypt(FILE *in, FILE *out, int do_encrypt)
{
    /* Allow enough space in output buffer for additional block */
    unsigned char inbuf[1024];
    unsigned char outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen;
    int outlen;
    EVP_CIPHER_CTX ctx;
    
    /* Bogus key and IV: we'd normally set these from
     * another source.
     */
    unsigned char key[32];
    unsigned char iv[32];
    int i;
    for(i = 0; i < 32; i++){
	key[i] = (unsigned char)i;
	iv[i] = (unsigned char)(100-i);
    }

    if(do_encrypt >= 0){
	EVP_CIPHER_CTX_init(&ctx);
	EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv, do_encrypt);
    }    

    for(;;)
	{
	    inlen = fread(inbuf, 1, 1024, in);
	    if(inlen <= 0) break;

	    if(do_encrypt >= 0){
		if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
		    {
			/* Error */
			EVP_CIPHER_CTX_cleanup(&ctx);
			return 0;
		    }
	    }
	    else{
		memcpy(outbuf, inbuf, inlen);
		outlen = inlen;
	    }

	    fwrite(outbuf, 1, outlen, out);
	}
    
    if(do_encrypt >= 0){
	if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
	    {
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	    }
	fwrite(outbuf, 1, outlen, out);
	EVP_CIPHER_CTX_cleanup(&ctx);
    }
    
    return 1;
}

#endif
