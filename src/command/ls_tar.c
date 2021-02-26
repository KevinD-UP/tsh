#include "../header/ls_tar.h"

char *longPrint(char* toDisplay,struct posix_header *header);
char *get_typeflag(char* toDisplay,struct posix_header *header);
char *get_mode(char* toDisplay,struct posix_header *header);
char *get_nlinks(char* toDisplay,struct posix_header *header);
char *get_mtime(char* toDisplay,struct posix_header *header);
char *get_size(char* toDisplay,struct posix_header *header);
char* coloration(char typeflag);

int main(int nbArg, char ** argument)
{
    ls_tar(argument,nbArg, getenv("PATH")); //we customed the path variable so that it keeps track
                                            //of the changes of pwd in the tar
}

//main function, used to analyse inputs and context to gather informations
int ls_tar(char **argv, int argc, char * path){

    //  VARIABLES
    int lf = 0;             //(bool) long-format
    int nb_args = 0;        //number of targets in args
    int nb_display = 0;     //will be used in future version

    //  #1  What is the name of the tar I've been called in ?
    char* tar_name = file_of_tar(path);
    //  #2 What is my path inside of the tar ?
    //  ps : racine_of_tar returns NULL if I'm in depth0 of the tar, so I must check
    char* pwd_tar = racine_of_tar(path);
    //  #3 What do we expect of me ? What are my arguments ?
    //      3.1 Is "-l" present in argv ?
    int nb_l = 0;
    for (size_t i = 0; i < argc; i++){
        if(strcmp(argv[i],"-l")==0){
            lf=1;
            nb_l++;
        }
    }
    if(nb_l>1){
        print("ls: you can't enter \"-l\" option multiple times (why would you do that anyway ?)\n");
        return -1;
    }
    //      3.2 How many args do I have ?
    nb_args = argc - lf -1;
    nb_display = (nb_args == 0) ? 1 : nb_args;  //it's actually the number of arguments + 1, if no args then you still need 1 display
    char** arguments; //we want an array with k args, without "-l" if it was given
    arguments = (nb_args==0)?NULL: malloc(nb_args * sizeof(char*));
    if(arguments!=NULL){
        int index_arg = 0;
        for (size_t i = 1; i < argc; i++){
            if(strcmp(argv[i],"-l")!=0){
                arguments[index_arg] = malloc(sizeof(char)*(strlen(argv[i])+1));
                memset(arguments[index_arg],'\0',sizeof(char)*(strlen(argv[i])+1));
                sprintf(arguments[index_arg++],"%s",argv[i]);
            }
        }
    }
    //      3.3 We must display multiple arguments : let's make an array of disp_bloc
    disp_bloc display_list[nb_display];
    init_display(display_list,arguments,nb_args);

    //  #4 launching tar parsing
    return parse_tar(tar_name, arguments, lf, nb_args, pwd_tar, display_list);
}

//this function is used to initialise the display_list with the arguments provided by the user
void init_display(disp_bloc* display_list, char** arguments, int nb_args){

    for (size_t i = 0; i < nb_args+(nb_args==0)?1:0 ; i++){
        display_list[i].arg_name = NULL;
        if(nb_args!=0){
            display_list[i].arg_name = malloc(sizeof(char) * (strlen(arguments[i])+1));
            memset(display_list[i].arg_name,'\0',strlen(arguments[i])+1);
            sprintf(display_list[i].arg_name,"%s",arguments[i]);
        }                                                                                   //\>O</
        display_list[i].display = NULL;
    }
}

//this is the function that parses once the tar, and sends to other functions crutial intel
int parse_tar(char* tar_name,char** arguments, int lf, int nb_args, char* pwd_tar, disp_bloc* display_list){
    //  #1  We open the tar in read mode
    int fd = open(tar_name,O_RDONLY);
    if(fd < 0){
        perror("opening tar file");     //we check if it went well
        return -1;
    }
    //  #2 setting up the buffer for posix headers
    char buffer[512];
    struct posix_header *header = (struct posix_header *)buffer;

    //  #3 Main parsing loop : going from header to header looking for a matching file
    do{
        //  3.1 we read a header
        int nb_read = read(fd, header, BLOCKSIZE);
        if (nb_read != BLOCKSIZE){
            print("Error, tar malformé");
            goto exit_failure;
        }
        //  3.2 we check if we reached the end of it
        if ((header->name[0] == '\0')){
            goto exit_success;
        }

        //  3.3 we analyse the current header's name to know if we have to add it to the display
        elem_scan(header,pwd_tar,arguments, nb_args,lf,display_list);

        //  last.last we then jump to the next header
        unsigned int size;
        sscanf(header->size, "%o", &size);
        lseek(fd, ((size + BLOCKSIZE - 1) >> BLOCKBITS) * BLOCKSIZE, SEEK_CUR);

    } while (1);
    
    //those go-to are used to make sure we close and free everything and to lighten code
    exit_failure:
        //print("EXIT FAILURE\n");
        close(fd);
        free_display(display_list,nb_args+(arguments==NULL?1:0));
        for (size_t i = 0; i < nb_args; i++){
            free(arguments[i]);
        }
        return -1;
    exit_success:
        //print("EXIT SUCCESS\n");
        print_display(display_list, nb_args+(arguments==NULL?1:0));
        if(!lf)print("\n");
        close(fd);
        free_display(display_list,nb_args+(arguments==NULL?1:0));
        for (size_t i = 0; i < nb_args; i++){
            free(arguments[i]);
        }
        return 0;

}

void free_display(disp_bloc* display_list, int nb_display){
    for (size_t i = 0; i < nb_display; i++){
        free(display_list[i].arg_name);
        free(display_list[i].display);
    }
}

//this is an essential function, where we decide to print or not the current header's file
int elem_scan(struct posix_header* header, char* pwd_tar, char** arguments, int nb_args, int lf, disp_bloc* display_list){

    char name[100];
    strcpy(name,header->name);
    char* local_pwd_tar = (pwd_tar == NULL) ? "" : pwd_tar;

    if (nb_args == 0){
        /*print("\nname = "); print(name); print("\n");
        print("pwd_tar = "); print(local_pwd_tar); print("\n");*/
        if(isPrefix(local_pwd_tar,name)){
            if(strcmp(local_pwd_tar,name)==0){
                //print("Special Case \"Name == Pwd\" = SUCCESS\n");
                add_display(display_list,0,"");
            }
            else{
                if(isInFolder(name,local_pwd_tar)){
                    //print("MUST PRINT\n");
                    elem_print(header,display_list,nb_args,pwd_tar,NULL,lf,0);
                }
            }

        }
    }
    else {
        for (int i = 0; i < nb_args; i++){
            char* toFind = malloc((strlen(local_pwd_tar)+strlen(arguments[i]))*sizeof(char)+1);
            memset(toFind,'\0',(strlen(local_pwd_tar)+strlen(arguments[i]))*sizeof(char)+1);
            sprintf(toFind,"%s%s",local_pwd_tar,arguments[i]);

            /*print("\nname = "); print(name); print("\n");
            print("toFind = "); print(toFind); print("\n");*/

            if(isPrefix(toFind,name)){
                if(strcmp(toFind,name)==0){
                    if(header->typeflag != '5'){
                        //print("Special Case \"Name == Pwd && it's not a folder\" = SUCCESS\n");
                        //print("MUST PRINT\n");
                        add_display(display_list,i,name);
                    }
                    else{
                        //print("Special Case \"Name == toFind\" = SUCCESS\n");
                        add_display(display_list,i,"");
                    }
                }
                else{
                    if(isInFolder(name,toFind)){
                        //print("MUST PRINT\n");
                        elem_print(header,display_list,nb_args,pwd_tar,arguments[i],lf,i);
                    }
                }
            }
        } 
    }    
    return 0;
}

//utilitary function : returns true if s1 is included at the begining of s2
//for instance, isPrefix("ROOT/FOLDER2/","ROOT/FOLDER2/file") returns 1
int isPrefix(char* s1, char* s2){

    char* pwd = malloc(strlen(s1)*sizeof(char)+1);
    memset(pwd,'\0',strlen(s1)*sizeof(char)+1);
    strcpy(pwd,s1);
    char* header = malloc(strlen(s2)*sizeof(char)+1);
    memset(header,'\0',strlen(s2)*sizeof(char)+1);
    strcpy(header,s2);

    char* save_pwd;
    char* save_header;

    char* pwd_token = strtok_r(pwd,"/",&save_pwd);
    char* header_token = strtok_r(header,"/",&save_header);

    while(pwd_token != NULL && header_token != NULL){
        if(strcmp(pwd_token,header_token)!=0){
            //print("Prefix Test : FAILED (mismatch)\n");
            return 0;
        }else{
            pwd_token = strtok_r(NULL,"/",&save_pwd);
            header_token = strtok_r(NULL,"/",&save_header);
        }
    }

    if(pwd_token == NULL && header_token == NULL){ 
        //print("Prefix Test : SUCCESS\n");
        return 1;
    }else if(header_token != NULL && pwd_token == NULL){
        //print("Prefix Test : SUCCESS\n");
        return 1;
    }else{
        //print("Prefix Test : FAILED\n");
        return 0;
    }

    free(pwd);
    free(header);
}

//utilitary function : returns true if s1 is in s2
//for instance : inFolder("ROOT/FOLDER1/","ROOT/") returns 1
int isInFolder(char* s1, char* s2){
    /*printf("s1 = %s, size %ld\n",s1,strlen(s1));
    printf("s2 = %s, size %ld\n",s2,strlen(s2));
    printf("depth(s1) = %d and depth(s2) = %d\n",elem_depth(s1),elem_depth(s2));*/
    if(elem_depth(s1)-1==elem_depth(s2)){
        //print("Depth Test : SUCCESS\n");
        return 1;
    }
    else{
        //print("Depth Test : FAILED\n");
        return 0;
    }

}

//this function is used to know the depth of a file inside the tar
int elem_depth(char* name){
    int depth = 0;
    char* name_bis = malloc(strlen(name)*sizeof(char)+1);
    memset(name_bis,'\0',strlen(name)*sizeof(char)+1);
    strcpy(name_bis,name);

    char* tmp = strtok(name_bis,"/");

    while(tmp != NULL){
        depth++;
        tmp = strtok(NULL,"/");
    }

    free(name_bis);
    return depth;
}

//here, we are finally sure we have to print this file's name, so we do it (and nicely)
int elem_print(struct posix_header* header, disp_bloc* display_list, int nb_args, char* pwd_tar, char*argument, int lf, int index){
    //we must check which arg belongs the header to : 0 if display_list[0].arg_name == NULL
    int arg_index = (nb_args ==0)?0:index;
    char* to_add = malloc(sizeof(char)*100);
    memset(to_add,'\0',sizeof(char)*100);
    if(lf)to_add = longPrint(to_add,header);
    to_add = strcat(to_add,coloration(header->typeflag));
    to_add = strcat(to_add,header->name+
    (pwd_tar==NULL?0:strlen(pwd_tar)) +(argument==NULL?0:strlen(argument)));
    to_add = strcat(to_add,"\033[0m");
    to_add = strcat(to_add, (lf)?"\n":"    ");
    add_display(display_list, arg_index, to_add);
    return 0;
}

char* coloration(char typeflag){
    switch (typeflag)
    {
    case '0': //regular file
        return "\033[0;37m\0";
        break;
    case '\0': //AREGTYPE ? what is that
        return "\033[0;33m\0";
        break;
    case '5': //folder
        return "\033[0;34m\0";
        break;
    case '1': //link
        return "\033[0;36m\0";
        break;
    case '3': //special character file
        return "\033[0;35m\0";
        break;
    case '4': //block special file
        return "\033[0;31m\0";
        break;
    case '6': //FIFO file
        return "\033[0;33m\0";
        break;
    default:
        return "\033[0;37m\0";;
        break;
    }
}

void add_display(disp_bloc* display_list, int index, char* input){
    if(display_list[index].display!=NULL){
        display_list[index].display = strcat(display_list[index].display,input);
    }else{
        display_list[index].display = malloc(sizeof(char)*strlen(input)+1);
        memset(display_list[index].display,'\0',sizeof(char)*strlen(input)+1);
        display_list[index].display = strcat(display_list[index].display,input);
    }
}

//this function prints the final display for the user
void print_display(disp_bloc* display_list, int nb_display){
    //for each disp_bloc
    for (size_t i = 0; i < nb_display; i++){
        if(display_list[i].arg_name!=NULL && nb_display > 1 && (display_list[i].arg_name[strlen(display_list[i].arg_name)-1]=='/')){
            print(display_list[i].arg_name);
            print(":\n");
        }
        if(display_list[i].display==NULL){
            print("ls: cannot acces '");print(display_list[i].arg_name);print("':No such file or directory\n");
        }
        else if(strlen(display_list[i].display)!=0)
            print(display_list[i].display);
        if(nb_display>1)print("\n");
    }
}

char* longPrint(char* toDisplay, struct posix_header *header)
{
    toDisplay = get_typeflag(toDisplay,header);
    toDisplay = get_mode(toDisplay,header);
    toDisplay = get_nlinks(toDisplay,header);
    //printing UID
    toDisplay = strcat(toDisplay, header->uname);
    toDisplay = strcat(toDisplay, "\t");
    //printing GID
    toDisplay = strcat(toDisplay, header->gname);
    toDisplay = strcat(toDisplay, "\t");
    toDisplay = get_size(toDisplay,header);
    toDisplay = get_mtime(toDisplay,header);
    return toDisplay;
}

char *get_typeflag(char* toDisplay, struct posix_header *header)
{
    switch (header->typeflag){
    case '0': //regular file
        toDisplay = strcat(toDisplay,"-");
        break;
    case '\0': //AREGTYPE ? what is that
        toDisplay = strcat(toDisplay,"-");
        break;
    case '5': //folder
        toDisplay = strcat(toDisplay,"d");
        break;
    case '1': //link
        toDisplay = strcat(toDisplay,"l");
        break;
    case '3': //special character file
        toDisplay = strcat(toDisplay,"c");
        break;
    case '4': //block special file
        toDisplay = strcat(toDisplay,"b");
        break;
    case '6': //FIFO file
        toDisplay = strcat(toDisplay,"p");
        break;
    default:
        toDisplay = strcat(toDisplay,"?");
        break;
    }
    return toDisplay;
}

char *get_mode(char* toDisplay,struct posix_header *header)
{
    char str_mode[3];
    strcpy(str_mode, (header->mode) + 4); //fetching the last 3 digit of header->mode field

    int i = 0;
    while (i < 3){
        switch (str_mode[i]){
        case '0':
            toDisplay = strcat(toDisplay, "---");
            break;
        case '1':
            toDisplay = strcat(toDisplay, "--x");
            break;
        case '2':
            toDisplay = strcat(toDisplay, "-w-");
            break;
        case '3':
            toDisplay = strcat(toDisplay, "-wx");
            break;
        case '4':
            toDisplay = strcat(toDisplay, "r--");
            break;
        case '5':
            toDisplay = strcat(toDisplay, "r-x");
            break;
        case '6':
            toDisplay = strcat(toDisplay, "rw-"); //for instance, 6 in binary is 110, so r = 1, w = 1, x = 0
            break;
        case '7':
            toDisplay = strcat(toDisplay, "rwx");
            break;
        default:
            toDisplay = strcat(toDisplay, "???");
            break;
        }
        i++;
    }

    toDisplay = strcat(toDisplay, "\t");
    return toDisplay;
}

char *get_nlinks(char* toDisplay,struct posix_header *header)
{
    toDisplay = strcat(toDisplay,"?\t");
    return toDisplay;
}

char *get_mtime(char* toDisplay,struct posix_header *header)
{
    //printing last date
    time_t time = 0;
    //printf("header->mtime = %s\n",header->mtime);
    sscanf(header->mtime, "%lo", &time);
    //printf("time = %ld\n",time);
    struct tm *time_struct = gmtime(&time);
    char *res = asctime(time_struct);
    res[strlen(res) - 1] = '\0';
    //printf("res = %s\n",res);
    toDisplay = strcat(toDisplay, res);

    //toDisplay = strcat(toDisplay,str_time);
    toDisplay = strcat(toDisplay, "\t");
    return toDisplay;
}

char *get_size(char* toDisplay,struct posix_header *header)
{
    //printing size
    unsigned int size = 0;
    sscanf(header->size, "%o", &size);
    char str_size[4];
    sprintf(str_size, "%d", size);
    toDisplay = strcat(toDisplay, str_size);
    toDisplay = strcat(toDisplay, "\t");
    return toDisplay;
}

//cayocuapi
/*
int isPrefix(char* s1, char* s2){

    print("s1 = ");print(s1);print("\n");
    print("s2 = ");print(s2);print("\n");

    char* s1_bis = malloc(strlen(s1)*sizeof(char)+1);
    memset(s1_bis,'\0',strlen(s1)*sizeof(char)+1);
    strcpy(s1_bis,s1);
    char* s2_bis = malloc(strlen(s2)*sizeof(char)+1);
    memset(s2_bis,'\0',strlen(s2)*sizeof(char)+1);
    strcpy(s2_bis,s2);

    char* tmp1 = strtok(s1_bis,"/");
    char* tmp2 = strtok(s2_bis,"/");

    while(tmp1 != NULL && tmp2 != NULL){
        if(strcmp(tmp1,tmp2)!=0){
            print("Prefix Test : FAILED\n");
            return 0;
        }else{
            tmp1 = strtok(NULL,"/");
            tmp2 = strtok(NULL,"/");
        }
    }
    print("after while parsing :\n");
    print("tmp1 was "); print(tmp1==NULL?"null\n":"not null\n");
    print("tmp2 was "); print(tmp2==NULL?"null\n":"not null\n");

    if(tmp1 == NULL && tmp2 == NULL){avant 
        print("Prefix Test : SUCCESS\n");
        return 1;
    }else if(tmp2 == NULL && tmp1 != NULL){
        print("Prefix Test : SUCCESS\n");
        return 1;
    }else{
        print("Prefix Test : FAILED\n");
        return 0;
    }


    free(s1_bis);
    free(s2_bis);
}*/
