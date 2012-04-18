/**
   AES encryption/decryption demo program using OpenSSL EVP apis
   gcc -Wall openssl_aes.c -lcrypto

   this is public domain code. 

   Saju Pillai (saju.pillai@gmail.com)
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/aes.h>

int do_crypt(FILE *in, FILE *out, int do_encrypt)
{
    /* Allow enough space in output buffer for additional block */
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;
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

    EVP_CIPHER_CTX_init(&ctx);
    EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv, do_encrypt);
    
    for(;;)
	{
	    inlen = fread(inbuf, 1, 1024, in);
	    if(inlen <= 0) break;
	    if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
		{
		    /* Error */
		    EVP_CIPHER_CTX_cleanup(&ctx);
		    return 0;
		}
	    fwrite(outbuf, 1, outlen, out);
	}
    if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
	{
	    /* Error */
	    EVP_CIPHER_CTX_cleanup(&ctx);
	    return 0;
	}
    fwrite(outbuf, 1, outlen, out);
    EVP_CIPHER_CTX_cleanup(&ctx);
    return 1;
}

int main(int argc, char **argv)
{
    
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    int action = 0;

    /* Check Input */
    if(argc < 4){
	fprintf(stderr, "usage: %s %s\n", argv[0], "<type> <in path> <out path>");
	exit(EXIT_FAILURE);
    }

    if(!strcmp(argv[1], "-e")){
	action = 1;
    }
    else if(!strcmp(argv[1], "-d")){
	action = 0;
    }
    else {
	fprintf(stderr, "Unkown action\n");
	exit(EXIT_FAILURE);
    }

    /* open files */
    inFile = fopen(argv[2], "rb");
    if(!inFile){
	perror("infile fopen error");
	return EXIT_FAILURE;
    }
    outFile = fopen(argv[3], "wb+");
    if(!outFile){
	perror("outfile fopen error");
	return EXIT_FAILURE;
    }

    if(!do_crypt(inFile, outFile, action)){
	fprintf(stderr, "do_crypt failed\n");
    }

    /* Cleanup */
    if(fclose(outFile)){
        perror("outFile fclose error\n");
    }

    if(fclose(inFile)){
	perror("inFile fclose error\n");
    }

    return 0;
}
