#include "../header/mkdir_tar.h"

int launcher_mkdir_tar(int nbArg, char ** argument, char * path);
void mkdir_help();
void mkdir_tar(char *argument, char * path);
void mkdirOutTar(char * src);

int main(int nbArg, char ** argument)
{
	launcher_mkdir_tar(nbArg, argument, getenv("PATH"));
}

int launcher_mkdir_tar(int nbArg, char ** argument, char * path)
{
	/*S'il n'y a pas d'argument après mkdir*/
	if(nbArg == 1)
	{
		print("tsh: mkdir: opérande manquant \n");
		print("Saisissez « mkdir --help » pour plus d'informations. \n");
	}
	/* rm --help */
	else if(nbArg == 2 && strcmp(argument[1], "--help") == 0)
	{
		mkdir_help();
	}
	else{
		/* si on est dans le tar */
		if(isTar(path)){
			char * src;
			for(int i = 1; i < nbArg; i++)
			{
				src = name_in_tar(argument[i],path);
				/* le rep src doit etre en dehors du tar */
				if(isOutTar(src)){
					mkdirOutTar(nameOutTar(src));
				}
				/* le rep src doit etre dans le tar */
				else{
					mkdir_tar(src, path);
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
						print("tsh: mkdir:  ");
						print(argument[i]);
						print(" n'existe pas \n");
						break;
					}
					/* sinon */
					else{
						char * src = analyse_name_in_tar(racine_of_tarBis(argument[i]));
						mkdir_tar(src,analyse_name_in_tar(newPath));
					}
				}
				/* l'argument i doit etre en dehors du tar */
				else{
					mkdirOutTar(argument[i]);
				}
			}
		}
	}

	return 0;

}

void mkdir_help()
{
	print("Utilisation : mkdir RÉPERTOIRE...\n");
	print("Créer le ou les RÉPERTOIREs s'ils n'existent pas. \n");
	print("--help     afficher l'aide et quitter \n");
}

void mkdir_tar(char *argument, char * path)
{
	if(path_name(argument) != NULL && file_exist(path_name(argument),path)==0){
		print("tsh: mkdir: le repertoire '");
		print(path_name(argument));
		print("' n'existe pas\n");
		exit(1);
	}
	
	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar 

	int n;
	
	/* on ajoute '/' au nom car c'est un repertoire */
	if(argument[strlen(argument)-1] != '/'){
		argument = strcat(argument,"/");
	}
	
	char buffer[BLOCKSIZE]; //Reader of the header

	struct posix_header * p_hdr; //Pointeur of the header 

	//Init the new Header
	struct posix_header * newDir = malloc(BLOCKSIZE);
	memset(newDir, 0, BLOCKSIZE);
	sprintf(newDir -> name, "%s",argument);
	sprintf(newDir -> mode, "000000700");
	sprintf(newDir -> size, "%011o", 0);	
	sprintf(newDir -> mtime, "%lo", time(NULL));
	newDir -> typeflag = '5'; //Typeflag for a directory
	sprintf(newDir -> magic, TMAGIC);
	sprintf(newDir -> version, "00");
	set_checksum(newDir);
	if(check_checksum(newDir) == 0){
		perror("checksum of the header incorrect");
	}
	
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
			write(fd, newDir, BLOCKSIZE); //Write the header
			break; //Exit the loop
		}
	}
	memset(buffer, 0, BLOCKSIZE);
	write(fd, buffer, BLOCKSIZE);
	lseek(fd, 0, SEEK_SET);
	close(fd);
	free(newDir);
}

void mkdirOutTar(char * src){
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/mkdir","/bin/mkdir",src, (char*) NULL )  <0){
				perror("exec");
				print("mkdir");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}
