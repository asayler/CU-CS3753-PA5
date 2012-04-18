/**
   AES encryption/decryption demo program using OpenSSL EVP apis
   gcc -Wall openssl_aes.c -lcrypto

   this is public domain code. 

   Saju Pillai (saju.pillai@gmail.com)
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "openssl_aes.h"

#define BLOCKSIZE 512

int main(int argc, char **argv)
{
    
    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
       status of enc/dec operations */
    EVP_CIPHER_CTX en;

    /* 8 bytes to salt the key_data during key generation. This is an example of
       compiled in salt. We just read the bit pattern created by these two 4 byte 
       integers on the stack as 64 bits of contigous salt material - 
       ofcourse this only works if sizeof(int) >= 4 */
    unsigned int salt[] = {12345, 54321};
    unsigned char* key_data;
    int key_data_len;
    int nrounds = 5;
    unsigned char key[32];
    unsigned char iv[32];    
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    int i;
    unsigned char buf[BLOCKSIZE];
    unsigned char cbuf[BLOCKSIZE + AES_BLOCK_SIZE];
    int clen = 0;
    int flen = 0;
    size_t readSize;
    size_t writeSize;

    /* Check Input */
    if(argc < 4){
	fprintf(stderr, "usage: %s %s\n", argv[0], "<key> <in path> <out path>");
	exit(EXIT_FAILURE);
    }

    /* the key_data is read from the argument list */
    key_data = (unsigned char *)argv[1];
    key_data_len = strlen(argv[1]);
  
    /*
     * Gen key & IV for AES 256 CBC mode.
     * A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material.
     * More rounds are more secure but slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), (unsigned char*)&salt,
		       key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
	fprintf(stderr, "Key size is %d bits - should be 256 bits\n", i * 8);
	return EXIT_FAILURE;
    }

    /* Setup Cipher */
    EVP_CIPHER_CTX_init(&en);
    if(!EVP_EncryptInit_ex(&en, EVP_aes_256_cbc(), NULL, key, iv)){
	fprintf(stderr, "EVP_EncryptInit_ex Error\n");
	return EXIT_FAILURE;
    }

    /* open files */
    inFile = fopen(argv[2], "r");
    if(!inFile){
	perror("infile fopen error");
	return EXIT_FAILURE;
    }
    outFile = fopen(argv[3], "wb+");
    if(!outFile){
	perror("outfile fopen error");
	return EXIT_FAILURE;
    }

    /* Loop Through Input */
    while(!feof(inFile)){
	/* Read Input */
	readSize = fread(buf, sizeof(*buf), BLOCKSIZE, inFile);
	if((readSize != BLOCKSIZE) && !feof(inFile)){
	    perror("inFile fread error");
	    return EXIT_FAILURE;
	}

	/* Encrypt Input */	
	/* update ciphertext, c_len is filled with the length of ciphertext generated,
	 * len is the size of plaintext in bytes */
	if(!EVP_EncryptUpdate(&en, cbuf, &clen, buf, readSize)){
	    fprintf(stderr, "EVP_EncryptUpdate Error\n");
	    return EXIT_FAILURE;
	}
	
	/* Write Output */
	writeSize = fwrite(cbuf, sizeof(*cbuf), clen, outFile);
	if((int)writeSize != clen){
	    perror("outFile fwrite error");
	    return EXIT_FAILURE;
	}
    }
    
    /* Write Padding */
    /* update ciphertext with the final remaining bytes */
    if(!EVP_EncryptFinal_ex(&en, cbuf, &flen)){
	fprintf(stderr, "EVP_EncryptFinal Error\n");
	return EXIT_FAILURE;
    }
    /* Write Output */
    writeSize = fwrite(cbuf, sizeof(*cbuf), clen, outFile);
    if((int)writeSize != clen){
	perror("outFile fwrite error");
	return EXIT_FAILURE;
    }

    /* Cleanup */
    if(fclose(outFile)){
        perror("outFile fclose error\n");
    }

    if(fclose(inFile)){
	perror("inFile fclose error\n");
    }
    
    if(!EVP_CIPHER_CTX_cleanup(&en)){
	fprintf(stderr, "EVP_CIPHER_CTX Error\n");
    }

    return 0;
}
