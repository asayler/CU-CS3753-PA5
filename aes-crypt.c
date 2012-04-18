/**
   AES encryption/decryption demo program using OpenSSL EVP apis
   gcc -Wall openssl_aes.c -lcrypto

   this is public domain code. 

   Saju Pillai (saju.pillai@gmail.com)
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aes-crypt.h"

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
