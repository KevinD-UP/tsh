#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//for readline

#include <readline/readline.h>
#include <readline/history.h>

#include "../header/print.h"
#include "../header/launcher.h"
#include "../header/analyse_lexicale.h"
#include "../header/launcher.h"

#define BUFSIZE	512

void print_prompt(char* path);
int nb_pipes(char* s);

int main(int argc, char * argv[])
{

	int end = 0;

	char *path = malloc((BUFSIZE+1) * sizeof(char));
	if(path == NULL)
	{
		perror("allocation error of path in loop");
	}
	getcwd(path, BUFSIZE);

	setenv("PATH", path, 1);

	if(argc > 1){
		if(argc != 2){
			print("You can only launch tsh on one demo.txt at the time\nUsage : ./tsh target\n1 command = 1 line\n");
			return 0;
		}

		int demo = open(argv[1],O_RDONLY);
		if(demo < 0){
			perror("error opening file\n");
			return 0;
		}
		dup2(demo,0);
		close(demo);

		char *my_line = malloc(sizeof(char) * 1000);

		while(1){

			print("loading");
			
			int x = 0;
			while(x++<2){
				print(".");
			}
			print("\n");

			print_prompt(getenv("PATH"));

			int j = 0;	
			memset(my_line, '\0', 1000);
			char buffer[1];
			int byte_read = 0;
			do {
				byte_read = read(STDIN_FILENO, buffer, 1);
				if(buffer[0]!='\n')my_line[j++]=buffer[0];
			}while(byte_read != 0 && buffer[0] != '\n' && buffer[0] != EOF);

			if(strlen(my_line)==0){
				print("\nDEMO COMPLETED\n");
				exit(0);
			}

			print("TEST ENTERED = [");print(my_line);print("]\n");

			print("OUTPUT = \n");
			//step3 : spliting user_entry into blocs of commands separated by pipes
			int nb_pipe = nb_pipes(my_line);
			if(nb_pipe < 0)continue;
			//printf("nb_pipes = %d\n",nb_pipe);
			int nb_blocks = nb_pipe+1;
			//printf("nb blocks = %d\n",nb_blocks);

			char** pipeBlocks = malloc((sizeof(char)*nb_blocks)+1);
			int i = 0;
			char* pipe_token = strtok(my_line,"|");
			while (pipe_token != NULL)
			{
				pipeBlocks[i] = malloc(sizeof(char)*strlen(pipe_token)+1);
				memset(pipeBlocks[i],'\0',sizeof(char)*strlen(pipe_token)+1);
				sprintf(pipeBlocks[i++],"%s",analyse(pipe_token));
				pipe_token = strtok(NULL,"|");
			}
			/*for (size_t j = 0; j < nb_blocks; j++){
				print("pipeBlocks[j] = ");print(pipeBlocks[j]);print("\n");
			}*/
			
			//step4 : we now send everything to launcher for further analyse and processing
			end = managePipes(pipeBlocks,nb_blocks);

			for (size_t i = 0; i < nb_blocks; i++)
			{
				free(pipeBlocks[i]);
			}
			free(pipeBlocks);	

			if(byte_read==0)break;
		}
		if(my_line)free(my_line);
		
		exit(EXIT_SUCCESS);
		
	}
	else{
		/* LOOP */
		while(end == 0)
		{	
			print_prompt(getenv("PATH"));

			//step1 : getting user input		
			char * buffer = malloc((BUFSIZE+1) * sizeof(char));
			if(buffer == NULL)
			{
				perror("allocation error in loop");
			}
			memset(buffer,0,BUFSIZE);
			int n = read(STDOUT_FILENO, buffer, BUFSIZE);
			if(n<0)
			{
				perror("error open in loop");
			}
			
			//step2 : cleaning user input by removing extra spaces
			char * user_entry = analyse(buffer);
			if(strcmp(user_entry,"")==0){	//if no input, loop back
				continue;
			}
			//print("user_entry = "); print(user_entry); print("\n");

			//step3 : spliting user_entry into blocs of commands separated by pipes
			int nb_pipe = nb_pipes(user_entry);
			if(nb_pipe < 0)continue;
			//printf("nb_pipes = %d\n",nb_pipe);
			int nb_blocks = nb_pipe+1;
			//printf("nb blocks = %d\n",nb_blocks);

			char** pipeBlocks = malloc((sizeof(char)*nb_blocks)+1);
			int i = 0;
			char* pipe_token = strtok(user_entry,"|");
			while (pipe_token != NULL)
			{
				pipeBlocks[i] = malloc(sizeof(char)*strlen(pipe_token)+1);
				memset(pipeBlocks[i],'\0',sizeof(char)*strlen(pipe_token)+1);
				sprintf(pipeBlocks[i++],"%s",analyse(pipe_token));
				pipe_token = strtok(NULL,"|");
			}
			/*for (size_t j = 0; j < nb_blocks; j++){
				print("pipeBlocks[j] = ");print(pipeBlocks[j]);print("\n");
			}*/
			
			//step4 : we now send everything to launcher for further analyse and processing
			end = managePipes(pipeBlocks,nb_blocks);

			for (size_t i = 0; i < nb_blocks; i++)
			{
				free(pipeBlocks[i]);
			}
			free(pipeBlocks);		
			free(buffer);
		}
		return 0;
	}
 	return 0;
}

int nb_pipes(char* s){
	int nb = 0;
	for (size_t i = 0; i < strlen(s); i++)
	{
		if(s[i]=='|'){
			if((s[i-1]!=' ') || (s[i+1]!=' ')){
				print("tsh: malformed entry, you must add 1 space before and after a pipe\n");
				return -1;
			}else{
				nb++;
			}
		}
	}
	return nb;	
}

void print_prompt(char* path){
		
		char hostname[HOST_NAME_MAX];
  		//char username[LOGIN_NAME_MAX];
  		int result;
  		
  		result = gethostname(hostname, HOST_NAME_MAX);		//login and hostname found here : https://stackoverflow.com/questions/27914311/get-computer-name-and-logged-user-name/44152790
  		
  		if (result)
    	{
      		perror("gethostname");
    	}
    	
  		//result = getlogin_r(username, LOGIN_NAME_MAX);
  		
  		if (result)
    	{
      		perror("getlogin_r");
    	}
		
		print("\033[1;32m");			
		print( "laptop");		
		print("@");
		print( hostname);
		print("@tsh:");
		print("\033[1;34m");
		print( path);
		print( "\033[0;m");
		print("$ ");
		/*
		
		char * prompt = malloc(BLOCKSIZE * sizeof(char));
		
		if(prompt == NULL){
			perror("allocation error prompt loop");
		}
		sprintf(prompt, "\033[1;32m%s@%s@tsh\033[1;34m%s\033[0;m$ ", getlogin(), hostname, path);
		return prompt;
		*/
}
