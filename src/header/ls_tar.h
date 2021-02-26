#ifndef _LSTAR_
#define _LSTAR_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

typedef struct disp_bloc{
    char* arg_name;
    char* display;
}disp_bloc;

int isPrefix(char* s1, char* s2);
int isInFolder(char* s1, char* s2);
int ls_tar(char **argv, int argc, char * path);
int parse_tar(char* tar_name,char** arguments, int lf, int nb_args, char* pwd_tar, disp_bloc* display_list);
int ls_tar_test(char **argv, int argc, char * path);
int elem_depth(char* name);
int elem_scan(struct posix_header* header,char* pwd_tar, char** arguments, int nb_args, int lf, disp_bloc* display_list);
int elem_print(struct posix_header* header, disp_bloc* display_list, int nb_args, char* pwd_tar, char*argument ,int lf, int index);
void add_display(disp_bloc* display_list, int index, char* input);
void init_display(disp_bloc* display_list, char** arguments, int nb_args);
void print_display(disp_bloc* display_list, int nb_args);
void free_display(disp_bloc* display_list, int nb_display);

int main(int nbArg, char ** argument);

#endif