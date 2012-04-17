#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>
#include <linux/xattr.h>
#include <sys/types.h>

#define CMDLS "-l"
#define CMDSET "-s"
#define CMDGET "-g"
#define CMDREM "-r"
#define USAGE_GENERAL "<command> <opt args> <path>"
#define USAGE_LIST CMDLS " <path>"
#define USAGE_SET CMDSET " <Attr Name> <Attr Value> <path>"
#define USAGE_GET CMDGET " <Attr Name> <path>"
#define USAGE_REM CMDREM " <Attr Name> <path>"

#ifdef linux
/* Linux is missing ENOATTR error, using ENODATA instead */
#define ENOATTR ENODATA
#endif

void printUsageGeneral(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_GENERAL);    
}

void printUsageList(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_LIST);    
}

void printUsageSet(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_SET);    
}

void printUsageGet(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_GET);    
}

void printUsageRem(char* pgmName){
    fprintf(stderr, "Usage: %s %s\n",
	    pgmName, USAGE_REM);    
}

int main(int argc, char* argv[]){

    char* lst = NULL;
    char* chr = NULL;
    char* start = NULL;
    char* tmpstr = NULL;
    char* tmpval = NULL;
    ssize_t cnt = 0;
    ssize_t lstsize = 0;
    ssize_t valsize = 0;

    /* Check base input */
    if(argc < 2){
	printUsageGeneral(argv[0]);
	exit(EXIT_FAILURE);
    }
    
    /* Parse Command */
    if(!strcmp(argv[1], CMDLS)){
	/* List Case */
	/* Check proper input */
	if(argc != 3){
	    printUsageList(argv[0]);
	    exit(EXIT_FAILURE);
	}
	lstsize = listxattr(argv[2], NULL, 0);
	if(lstsize < 0){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", argv[2]);
	    exit(EXIT_FAILURE);
	}
	lst = malloc(sizeof(*lst)*lstsize);
	if(!lst){
	    perror("malloc of 'lst' error");
	    exit(EXIT_FAILURE);
	}
	lstsize = listxattr(argv[2], lst, lstsize);
	if(lstsize < 0 || !lst){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", argv[2]);
	    exit(EXIT_FAILURE);
	}
	/* Tokenize on \0 */
	chr = start = lst;
	cnt = 0;
	while(cnt < lstsize){
	    fprintf(stdout, "%s\n", start);
	    while(*chr != '\0'){
		chr++;
		cnt++;
	    }
	    start = ++chr;
	    cnt++;
	}
    }
    else if(!strcmp(argv[1], CMDSET)){
	/* Set Case */
	/* Check proper input */
	if(argc != 5){
	    printUsageSet(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	if(setxattr(argv[4], tmpstr, argv[3], strlen(argv[3]), 0)){
	    perror("setxattr error");
	    fprintf(stderr, "path  = %s\n", argv[4]);
	    fprintf(stderr, "name  = %s\n", tmpstr);
	    fprintf(stderr, "value = %s\n", argv[3]);
	    fprintf(stderr, "size  = %d\n", strlen(argv[3]));
	    exit(EXIT_FAILURE);
	}
	free(tmpstr);
    }
    else if(!strcmp(argv[1], CMDGET)){
	/* Get Case */
	/* Check proper input */
	if(argc != 4){
	    printUsageGet(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	/* Get valsize */
	valsize = getxattr(argv[3], tmpstr, NULL, 0);
	if(valsize < 0){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		exit(EXIT_FAILURE);
	    }
	    else{
		perror("getxattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		fprintf(stderr, "value = %s\n", "NULL");
		fprintf(stderr, "size  = %d\n", valsize);
		exit(EXIT_FAILURE);
	    }
	}
	/* Malloc Value Space */
	tmpval = malloc(sizeof(*tmpval)*(valsize+1));
	if(!tmpval){
	    perror("malloc of 'tmpval' error");
	    exit(EXIT_FAILURE);
	}
	/* Get Value */
	valsize = getxattr(argv[3], tmpstr, tmpval, valsize);
	if(valsize < 0){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		exit(EXIT_FAILURE);
	    }
	    else{
		perror("getxattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		fprintf(stderr, "value = %s\n", tmpval);
		fprintf(stderr, "size  = %d\n", valsize);
		exit(EXIT_FAILURE);
	    }
	}
	
	/* Print Value */
	tmpval[valsize] = '\0';
	fprintf(stdout, "%s = %s\n", tmpstr, tmpval);
	
	/* Clean Up */
	free(tmpval);
	free(tmpstr);
    }
    else if(!strcmp(argv[1], CMDREM)){
	/* Get Case */
	/* Check proper input */
	if(argc != 4){
	    printUsageRem(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* Append necessary 'user.' prefix to beginning of name string */
	tmpstr = malloc(strlen(argv[2]) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr){
	    perror("malloc of 'tmpstr' error");
	    exit(EXIT_FAILURE);
	}
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, argv[2]);
	/* Remove Attr */
	if(removexattr(argv[3], tmpstr)){
	    if(errno == ENOATTR){
		fprintf(stdout, "No %s attribute set on %s\n", tmpstr, argv[3]);
		exit(EXIT_FAILURE);
	    }
	    else{
		perror("removexattr error");
		fprintf(stderr, "path  = %s\n", argv[3]);
		fprintf(stderr, "name  = %s\n", tmpstr);
		exit(EXIT_FAILURE);
	    }
	}
	free(tmpstr);
    }
    else{
	fprintf(stderr, "Unrecognized option\n");
	exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
