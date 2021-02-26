#include "../header/launcher.h"

int isRedir(char* s);
int pipe_rec(char** pipeBlocks, int nb_blocks, int pos);
int notCommandForTarball(char *s);

int isRedir(char* s){
	return (strcmp(s,"<")==0) ||
	(strcmp(s,">")==0) || (strcmp(s,">>")==0) ||
	(strcmp(s,"2>")==0) || (strcmp(s,"2>>")==0);
}

int checkRedir(int seen0, int seen1, int seen2, int seen3, int seen4){
	if(seen0 > 1 || seen1 > 1 || seen2 > 1 || seen3 > 1 || seen4 > 1){
		print("Multiple redirections aren't implemented.\n");
		return -1;
	}
	if((seen0 && seen1) || (seen2 && seen3)){
		print("You can't use 2 redirections for the same stream in two different mods.\n");
		return -1;
	}
	return 0;
}

int manageRedir(char** cmdBlocks, int nb_blocks){

	//first we must check the user's input

	int seen0 = 0;	// >
	int seen1 = 0;	// >>
	int seen2 = 0;	// 2>
	int seen3 = 0;	// 2>>
	int seen4 = 0;  // <

	//print("input = \n");
	for (size_t i = 0; i < nb_blocks; i++)
	{
		//print("cmdBlocks[i] = ");print(cmdBlocks[i]);print("\n");
		if(strcmp(cmdBlocks[i],">")==0)seen0++;
		if(strcmp(cmdBlocks[i],">>")==0)seen1++;
		if(strcmp(cmdBlocks[i],"2>")==0)seen2++;
		if(strcmp(cmdBlocks[i],"2>>")==0)seen3++;
		if(strcmp(cmdBlocks[i],"<")==0)seen4++;
	}

	if(checkRedir(seen0,seen1,seen2,seen3,seen4)<0){
		return 0;
	}
	
	char* cmd_and_args = malloc(strlen(cmdBlocks[0])*sizeof(char)+1);
	memset(cmd_and_args,'\0',strlen(cmdBlocks[0])*sizeof(char)+1);
	sprintf(cmd_and_args,"%s",cmdBlocks[0]);
	//print("1. cmd_and_args = "); print(cmd_and_args); print("\n");

	//saving the cmd's args
	if(nb_blocks > 1){
		for (size_t i = 1; i < nb_blocks; i++)
		{
			if(!(isRedir(cmdBlocks[i])==1)){
				cmd_and_args = realloc(cmd_and_args, ((strlen(cmd_and_args)+1+strlen(cmdBlocks[i]))*sizeof(char)+1));
				cmd_and_args = strcat(cmd_and_args," ");
				cmd_and_args = strcat(cmd_and_args,cmdBlocks[i]);
			}else{
				cmd_and_args[strlen(cmd_and_args)] = '\0';
				break;
			}
		}
	}	

	//print("2. cmd_and_args = "); print(cmd_and_args); print("\n");

	for (size_t i = 0; i < nb_blocks; i++)
	{

		if(strcmp(cmdBlocks[i],">")==0){
			if(i==nb_blocks-1 || i == 0){
				print("Wrong usage of redirections !\n");
				return 0;
			}
			else{
				/*print("must redirect towards ");
				print(cmdBlocks[i+1]); print(" using '>'\n");*/
				int fd = open(cmdBlocks[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if(fd < 0){
					print("error opening ");print(cmdBlocks[i+1]);print("\n");
					return 0;
				}
				dup2(fd, 1);
				close(fd);
			}
		}
		if(strcmp(cmdBlocks[i],"<")==0){
			if(i==nb_blocks-1 || i == 0){
				print("Wrong usage of redirections !\n");
				return 0;
			}
			else{
				/*print("must redirect from ");
				print(cmdBlocks[i+1]); print(" using '<'\n");*/
				int fd = open(cmdBlocks[i+1], O_RDONLY, 0644);
				if(fd < 0){
					print("error opening ");print(cmdBlocks[i+1]);print("\n");
					return 0;
				}
				dup2(fd,0);
				close(fd);
			}
		}
		if(strcmp(cmdBlocks[i],"2>")==0){
			if(i==nb_blocks-1 || i == 0){
				print("Wrong usage of redirections !\n");
				return 0;
			}
			else{
				int fd = open(cmdBlocks[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if(fd < 0){
					print("error opening ");print(cmdBlocks[i+1]);print("\n");
					return 0;
				}
				dup2(fd,0);
				close(fd);
				/*print("must redirect standar error ");
				print(cmdBlocks[i+1]); print(" using '2>'\n");*/
			}
		}
		if(strcmp(cmdBlocks[i],"2>>")==0){
			if(i==nb_blocks-1 || i == 0){
				print("Wrong usage of redirections !\n");
				return 0;
			}
			else{
				int fd = open(cmdBlocks[i+1], O_APPEND|O_CREAT, 0644);
				if(fd < 0){
					print("error opening ");print(cmdBlocks[i+1]);print("\n");
					return 0;
				}
				dup2(fd,1);
				close(fd);
				/*print("must redirect standar error in append mode ");
				print(cmdBlocks[i+1]); print(" using '2>>'\n");*/
			}
		}
		if(strcmp(cmdBlocks[i],">>")==0){
			if(i==nb_blocks-1 || i == 0){
				print("Wrong usage of redirections !\n");
				return 0;
			}
			else{
				int fd = open(cmdBlocks[i+1], O_APPEND|O_CREAT, 0644);
				if(fd < 0){
					print("error opening ");print(cmdBlocks[i+1]);print("\n");
					return 0;
				}
				dup2(fd,1);
				close(fd);
				//print("must redirect standar output in append mode ");
				/*print(cmdBlocks[i+1]); print(" using '>>'\n");*/
			}
		}
		else{
			/*print(cmdBlocks[i]);print(" must be either the cmd, a cmd argumet or a redirection argument\n");*/
		}
	}
	int size = taille(cmd_and_args,' ');
	char** args = decoupage(cmd_and_args,' ',size);
	return launcher(args,size);
}

int pipe_rec(char** pipeBlocks, int nb_blocks, int pos){
	int size;
	char** args;
	int fd[2];
	if(pipe(fd)==-1){
		perror("pipe");
		exit(1);
	}
	int tmp_pid = fork();
	//after the fork, we have :	father | son
	switch (tmp_pid)
	{
		case -1:
			perror("fork");
			exit(1);
			break;
		//if we're in the son, needs to read
		case 0:
			if(nb_blocks == pos + 1){
				close(fd[1]);//close ecriture
				dup2(fd[0],STDIN_FILENO);
				close(fd[0]); //close lecture
				size = taille(pipeBlocks[pos],' ');
				args = decoupage(pipeBlocks[pos],' ',size);
				exit(manageRedir(args,size));
			}
			else{
				close(fd[1]);//close ecriture
				dup2(fd[0],STDIN_FILENO);
				close(fd[0]); //close lecture
				pipe_rec(pipeBlocks, nb_blocks, pos+1);
			}
						
			break;
		//if we're in the father, needs to write
		default:
			close(fd[0]);// close lecture
			dup2(fd[1],STDOUT_FILENO);
			close(fd[1]); //close ecriture
			size = taille(pipeBlocks[pos-1],' ');
			args = decoupage(pipeBlocks[pos-1],' ',size);
			exit(manageRedir(args,size));		
			
	}
	return 0;
}

int managePipes(char** pipeBlocks, int nb_blocks){
	
	int nbArg = taille(pipeBlocks[0],' ');
	char ** argument = decoupage(pipeBlocks[0],' ',nbArg);
	
	if(nb_blocks < 2 && strcmp(argument[0], "cd") == 0){
		return launcher_cd(nbArg, argument, getenv("PATH"));	
	}
	else{
		int size;
		char** args;
		pid_t n;
		int status;

		switch(n = fork()){
			case -1:
				perror("fork");
				exit(1);
				break;
			case 0: //son
				if(nb_blocks >= 2){

					int fd[2];
					if(pipe(fd)==-1){
						perror("pipe");
						exit(1);
					}
					int tmp_pid = fork();
					//after the fork, we have :	father | son
					switch (tmp_pid)
					{
						case -1:
							perror("fork");
							exit(1);
							break;
						//if we're in the son, needs to read
						case 0:
							if(nb_blocks == 2){
								close(fd[1]);//close ecriture
								dup2(fd[0],STDIN_FILENO);
								close(fd[0]); //close lecture
								size = taille(pipeBlocks[1],' ');
								args = decoupage(pipeBlocks[1],' ',size);
								exit(manageRedir(args,size));
							}
							else{
								close(fd[1]);//close ecriture
								dup2(fd[0],STDIN_FILENO);
								close(fd[0]); //close lecture
								pipe_rec(pipeBlocks, nb_blocks, 2);
							}
							
							break;
						//if we're in the father, needs to write
						default:
							close(fd[0]);// close lecture
							dup2(fd[1],STDOUT_FILENO);
							close(fd[1]); //close ecriture
							size = taille(pipeBlocks[0],' ');
							args = decoupage(pipeBlocks[0],' ',size);
							exit(manageRedir(args,size));
					}
					
				}else{
					size = taille(pipeBlocks[0],' ');
					args = decoupage(pipeBlocks[0],' ',size);
					exit(manageRedir(args,size));
				}

			default : //father
				waitpid(n,&status, 0);
				if(WIFEXITED(status)){
					return WEXITSTATUS(status);
				}
				break;
		}
	}
	return 0;
}

//used as a precise launcher
int launch(char**args, int nbArgs){
	
	char * buf = calloc(BLOCKSIZE+1, sizeof(char));
	sprintf(buf, "/bin/%s", args[0]);

	if(execv(buf, args)<0)
	{
		print(args[0]);
		print(" : command not found \n");
	}

	free(buf);
	return 0;
}

//int launcher(char* args[], int in_stream, int out_stream, int err_stream);
int launcher(char * args[], int nbArg)
{
	/* EMPTY LINE */
	if(strcmp(args[0], "") == 0)
	{	
		return 0;
	}
	/* EXIT */
	else if(strcmp(args[0], "exit") == 0)
	{
		return exitTerminal(nbArg, args);
	}
	/* CD */
	else if(strcmp(args[0], "cd") == 0)
	{	
		return launcher_cd(nbArg, args, getenv("PATH"));	
	}
	/* USUAL COMMAND FOR SHELL WITH TARBALL */
	else if(isTar(getenv("PATH")) || argInTar(args, nbArg))
	{	
		/*COMMAND THAT WE IMPLEMENTED WITH TARBALL*/
		if(notCommandForTarball(args[0]) == 0){
			if(execv(args[0], args)<0){
				print(args[0]);
				print(" : command not found \n");
			}
		/*OTHER COMMAND THAT DOESN'T DEPEND OF THE TARBALL WHILE THE CURRENT DIRECTORY IS IN TARBALL*/
		}else if(notCommandForTarball(args[0]) == 1){
			char * buf = calloc(BLOCKSIZE+1, sizeof(char));
			sprintf(buf, "/bin/%s", args[0]);

			switch(fork()){
			case -1: perror("fork"); exit(1); break;
			case 0: //son
				if(execv(buf, args)<0){
					print(args[0]);
					print(" : command not found \n");
				}
				break;
			default:
				wait(NULL);

			}
		free(buf);
		}
	}

	/* USUAL COMMAND FOR SHELL WITHOUT TARBALL */
	else 
	{	
		char * buf = calloc(BLOCKSIZE+1, sizeof(char));
		sprintf(buf, "/bin/%s", args[0]);

		switch(fork()){
			case -1: perror("fork"); exit(1); break;
			case 0: //son
				if(execv(buf, args)<0){
					print(args[0]);
					print(" : command not found \n");
				}
				break;
			default:
				wait(NULL);

		}
		free(buf);
	}
	return 0;
}

/* Check if the command has been asked by the professor in tarball
Return 0 if true else 1*/
int notCommandForTarball(char * s){
	if( strcmp(s,"cd")    == 0   || strcmp(s,"exit") == 0  ||
		strcmp(s,"pwd")   == 0   || strcmp(s,"mkdir") == 0 ||
		strcmp(s,"rmdir") == 0   || strcmp(s,"mv") == 0    ||
		strcmp(s,"cp")    == 0   || strcmp(s,"rm") == 0    ||
		strcmp(s,"ls")    == 0   || strcmp(s,"cat") == 0   ||
		strcmp(s,"cp")    == 0   || strcmp(s,"rm") == 0)
	{
		return 0;
	}else{
		return 1;
	}

}
