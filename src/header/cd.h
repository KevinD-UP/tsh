#ifndef _CD_
#define _CD_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>

#include "print.h"
#include "in_tar.h"
#include "tar.h"

#define BUFSIZE	 512


int launcher_cd(int nbArg, char ** argument, char * path);

#endif