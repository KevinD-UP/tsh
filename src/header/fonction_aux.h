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
#include "in_tar.h"
#include "print.h"

void touch_aux(char * argument, char * path);
void mkdir_aux(char * argument, char * path);
void rm_aux(char *argument, char * path);
char * name_in_tar(char * name, char * path);
char * analyse_name_in_tar(char * chaine);
int isOutTar(char * name);
char * nameOutTar(char * name);
char * name(char * chaine);
char * suffixe(char * name, char * son);
int file_exist(char * name, char * path);
char * path_name(char * chaine);
