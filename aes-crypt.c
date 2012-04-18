/*
   AES encryption/decryption demo program
   using OpenSSL EVP apis
   
   By Andy Sayler

   With hints from work by:
   Saju Pillai (saju.pillai@gmail.com)
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aes-crypt.h"

int main(int argc, char **argv)
{
    
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    int action = 0;
    int ifarg;
    int ofarg;
    char* key_str = NULL;

    /* Check Input */
    if(argc < 2){
	fprintf(stderr, "usage: %s %s\n", argv[0],
		"<type> <opt key> <in path> <out path>");
	exit(EXIT_FAILURE);
    }

    if(!strcmp(argv[1], "-e")){
	if(argc != 5){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-e <key> <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	key_str = argv[2];
	ifarg = 3;
	ofarg = 4;
	action = 1;
    }
    else if(!strcmp(argv[1], "-d")){
	if(argc != 5){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-d <key> <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	key_str = argv[2];
	ifarg = 3;
	ofarg = 4;
	action = 0;
    }
    else if(!strcmp(argv[1], "-c")){
	if(argc != 4){
	    fprintf(stderr, "usage: %s %s\n", argv[0],
		    "-c <in path> <out path>");
	    exit(EXIT_FAILURE);
	}
	key_str = NULL;
	ifarg = 2;
	ofarg = 3;
	action = -1;
    }
    else {
	fprintf(stderr, "Unkown action\n");
	exit(EXIT_FAILURE);
    }

    /* open files */
    inFile = fopen(argv[ifarg], "rb");
    if(!inFile){
	perror("infile fopen error");
	return EXIT_FAILURE;
    }
    outFile = fopen(argv[ofarg], "wb+");
    if(!outFile){
	perror("outfile fopen error");
	return EXIT_FAILURE;
    }

    if(!do_crypt(inFile, outFile, action, key_str)){
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
