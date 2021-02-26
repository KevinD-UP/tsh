#include "../header/touch_tar.h"

int launcher_touch_tar(int nbArg, char ** argument, char * path);
void touch_help();
void touch_tar(char * argument, char * path);
void touchOutTar(char * src);

int main(int nbArg, char ** argument)
{
	launcher_touch_tar(nbArg, argument, getenv("PATH"));
}

int launcher_touch_tar(int nbArg, char ** argument, char * path)
{
	if(nbArg == 1)
	{
		print("tsh: touch: opérande manquant \n");
		print("Saisissez « touch --help » pour plus d'informations. \n");
	}
	/* rm --help */
	else if(nbArg == 2 && strcmp(argument[1], "--help") == 0)
	{
		touch_help();
	}
	else{
		/* si on est dans le tar */
		if(isTar(path)){
			char * src;
			for(int i = 1; i < nbArg; i++)
			{
				src = name_in_tar(argument[i],path);
				/* le fichier src doit etre en dehors du tar */
				if(isOutTar(src)){
					touchOutTar(nameOutTar(src));
				}
				/* le fichier src doit etre dans le tar */
				else{
					touch_tar(src, path);
				}
				
			}
			free(src);
		}
		/* on n'est pas dans le tar */
		else{
			char * newPath;
			for(int i = 1; i < nbArg; i++)
			{
				/* l'argument i doit etre dans le tar */
				if(argInTarAux(argument[i])){
					newPath = pathIsCorrect(argument[i], path);
					/* le chemin fourni est incorrecte, on sort de la boucle */
					if( newPath == NULL){
						print("tsh: touch: ");
						print(argument[i]);
						print(" n'existe pas \n");
						break;
					}
					/* sinon */
					else{
						/* on enleve le '/' a ma fin (fourni par racine_of_tar) */
						char * src = analyse_name_in_tar(racine_of_tarBis(argument[i]));
						touch_tar(src,analyse_name_in_tar(newPath));
						//touch_tar(src,analyse_name_in_tar(newPath));
					}
				}
				/* l'argument i doit etre en dehors du tar */
				else{
					touchOutTar(argument[i]);
				}
			}
		}
	}

	return 0;

}

void touch_help()
{
	print("Utilisation : mkdir FICHIER...\n");
	print("Créer le ou les FICHIERs s'ils n'existent pas. \n");
	print("--help     afficher l'aide et quitter \n");
}

void touch_tar(char * argument, char * path){
	
	if(path_name(argument) != NULL && file_exist(path_name(argument),path)==0){
		print("tsh: touch: le repertoire '");
		print(path_name(argument));
		print("' n'existe pas\n");
		exit(1);
	}
	
	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar 
	if (fd < 0){
		perror("error open of tarball");
	}
	int n;
	
	/* on enleve '/' au nom car c'est un ficher regulier */
	if(argument[strlen(argument)-1] == '/'){
		argument[strlen(argument)-1] = '\0';
	}
	
	char buffer[BLOCKSIZE]; //Reader of the header

	struct posix_header * p_hdr; //Pointeur of the header 

	//Init the new Header
	struct posix_header * newFile = malloc(BLOCKSIZE);
	memset(newFile, 0, BLOCKSIZE);
	sprintf(newFile -> name, "%s",argument);
	sprintf(newFile -> mode, "0000700");
	sprintf(newFile -> size, "%011o", 0);
	sprintf(newFile -> mtime, "%lo", time(NULL));
	newFile -> typeflag = '0'; //Typeflag for a simple file
	sprintf(newFile -> magic, TMAGIC);
	sprintf(newFile -> version, "00");
	set_checksum(newFile);
	if(check_checksum(newFile) == 0){
		perror("checksum of the header incorrect");
	};
	
	/* upgrade size oh the tar + 512 octets for the header */
	struct stat st;
				
	if(stat(file_of_tar(path), &st) == -1){
		perror("stat error in mv_tar in ecriture");
		exit(0);
	}
	if(truncate(file_of_tar(path), st.st_size +  BLOCKSIZE) == -1){
		perror("truncate error in mv_tar in ecriture");
	}

    //Reading of each header
	while((n = read(fd, buffer, BLOCKSIZE)) > 0)
	{
		p_hdr = (struct posix_header*) buffer; //Update the current header
		if(p_hdr-> name[0] == '\0')//We reach the end of the tar
		{
			lseek(fd, -BLOCKSIZE, SEEK_CUR);//Move backwards by 512 bytes
			write(fd, newFile, BLOCKSIZE); //Write the header
			break; //Exit the loop
		}
	}
	lseek(fd, 0, SEEK_SET);
	close(fd);
	free(newFile);
}

/* touchOutTar permet d'executer touch avec un argument qui est 
 * en dehors du tar */
void touchOutTar(char * src){
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/touch","/bin/touch",src, (char*) NULL )  <0){
				perror("exec");
				print("touch");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}
