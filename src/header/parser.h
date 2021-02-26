#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE	 512

typedef struct
{
    int nbArg;
    char * args[];
}argument;


argument * parser(char * buffer);