#ifndef _CATTAR_
#define _CATTAR_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//thoses includes can be found in the "man 2 open" page
//they're required for open syscalls
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//this one is in "man read"
#include <unistd.h>

//tp's file
#include "tar.h"
#include "in_tar.h"

//maxime
#include "print.h"
#include "fonction_aux.h"

int main(int nbArg, char ** argument);
int cat_tar(char** argument, int argc, char* path);

#endif