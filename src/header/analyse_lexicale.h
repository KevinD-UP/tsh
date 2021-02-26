#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE	 512

char * analyse(char *c);
char ** decoupage(char * c, char separateur, int taille);
int taille(char * c, char separateur);
char ** analysePipe(int debut, int fin, char ** arg);