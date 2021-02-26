#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tar.h"

int isTar(char * path);
char * file_of_tar(char * path);
char * racine_of_tar(char * path);
char * racine_of_tarBis(char * path);
int argInTarAux(char * path);
char * pathIsCorrect(char * name, char * path);
char * path_of_tar(char * path);
int argInTar(char ** argument, int nbArg);