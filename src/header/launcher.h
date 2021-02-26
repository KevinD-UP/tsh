#ifndef _LAUNCHER_
#define _LAUNCHER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cd.h"
#include "exit.h"
#include "print.h"
#include "ls_tar.h"
#include "mkdir_tar.h"
#include "touch_tar.h"
#include "pwd_tar.h"
#include "rm_tar.h"
#include "analyse_lexicale.h"

//int launcher(char* args[], int in_stream, int out_stream, int err_stream);
int launcher(char * args[], int nbArg);
int isRedir(char* s);
int checkRedir(int seen0, int seen1, int seen2, int seen3, int seen4);
int manageRedir(char** cmdBlocks, int nb_blocks);
int managePipes(char** pipeBlocks, int nb_blocks);

#endif
