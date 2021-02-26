#include "../header/rmdir_tar.h"

int launcher_rmdir(int nbArg, char ** argument, char * path);
void rmdir_general( char *argument, char * path);
void rmdir_tar( char *argument, char * path);
void rmdir_help();
void rmdirOutTar(char * src);

int main(int nbArg, char ** argument)
{
	launcher_rmdir(nbArg, argument, getenv("PATH"));
}

int launcher_rmdir(int nbArg, char ** argument, char * path)
{
	/* si il n'y a pas d'argument apres rmdir*/
	if(nbArg == 1)
	{
		print("tsh: rmdir: opérande manquant \n");
	}
	/* rm --help */
	else if(nbArg == 2 && strcmp(argument[1], "--help") == 0)
	{
		rmdir_help();
	}
	else
	{	
		/* si on est dans le tar */
		if(isTar(path)){
			char * src;
			for(int i = 1; i < nbArg; i++)
			{
				src = name_in_tar(argument[i],path);
				/* le rep src doit etre en dehors du tar */
				if(isOutTar(src)){
					rmdirOutTar(nameOutTar(src));
				}
				/* le rep src doit etre dans le tar */
				else{
					rmdir_tar(src, path);
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
						print("tsh: rmdir: ");
						print(argument[i]);
						print(" n'existe pas \n");
						break;
					}
					/* sinon */
					else{
						char * src = analyse_name_in_tar(racine_of_tarBis(argument[i]));
						rmdir_tar(src,analyse_name_in_tar(newPath));
					}
				}
				/* l'argument i doit etre en dehors du tar */
				else{
					rmdirOutTar(argument[i]);
				}
			}
		}
	}
	return 0;
}

void rmdir_help(){
	print("Utilisation : rmdir RÉPERTOIRE... \n");
	print("Supprimer le ou les RÉPERTOIRE, s'ils sont vides. \n");
	print("--help     afficher l'aide et quitter \n");
}

void rmdir_general(char * argument, char * path)
{
	
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}

	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int size = 0; //size of directory give by his header
	int found = 0; // found is 1 if we find the directory and we can delete it else 0
	char ecriture[BLOCKSIZE]; // to write '\0' at the end of the directory if found == 1
	int compteur = -1; // to count how BLOCKSIZE do the directory that we want to remove
	int retour = 0;// to count how BLOCKSIZE we do skip before writing in the .tar
		
	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		sscanf(p_hdr->size, "%o", &size); //Update the size
		
		/* if we find the directory and skip ( header + directory ) we begin writing */ 
		if(found == 1 && compteur == 0)
		{
			/* on se decale à retour * BLOCKSIZE en arriere pour commencer l'ecriture 
			   on reecrit par dessus le fichier qu'on souhaite supprimer, on va decaler
			   tous le reste */
			lseek(fd, -(retour * BLOCKSIZE), SEEK_CUR);
			if(write(fd, tampon, BLOCKSIZE) < 0)
			{
				perror("erreur write in rmdir_tar.c in rmdirGen ");
			}
			/* on revient à notre position initiale */
			lseek(fd, (retour-1)*BLOCKSIZE, SEEK_CUR);
		}
		/* if we find the directory but we are in the directory */
		if(found == 1 && compteur != 0)
		{
			compteur --;
		}
		/* if we find the directory ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0 && found == 0){

			found = 1; // we find the directory so found = 1
			if(size ==  0)
			{
				compteur = 0;
			}
			else
			{
				if(size % BLOCKSIZE == 0){
					compteur = (size/BLOCKSIZE); // how BLOCKSIZE do the directory
				}
				else{
					compteur = (size/BLOCKSIZE) + 1; // how BLOCKSIZE do the directory
				}
				
			}
			retour = 2 + compteur; // how BLOCKSIZE we do skip ( + 2 because : 1 for the header and 1 for the header afer the directory)		
		}
	}

	/* nettoyage de la fin du fichier */

	/* comme la fin du fichier n'est reecrit ( car il n'y a rien a ecrire dessus)
	   on la nettoie a 0 vu qu'elle a deja ete ecrite juste avant */
	lseek(fd, -(retour * BLOCKSIZE), SEEK_CUR);
	for(int i=0; i<retour ; i++)
	{
		memset(ecriture, 0, BLOCKSIZE);
		if(write(fd, tampon, BLOCKSIZE) < 0)
		{
			perror("erreur write 2 in rm_tar.c in rm");
		}
	}
	
	// we close the file.tar
	close(fd);

}

void rmdir_tar(char *argument, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
		exit(fd);
	}

	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int found = 0;

	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;

		/* if we find the directory ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0){

			found = 1;

			/* si c'est un fichier */
			if(p_hdr -> typeflag == '0')
			{
				print("rmdir: impossible de supprimer '");
				print(argument);
				print("': N'est pas un dossier \n");
				break;
			}
			/* si c'est un répertoire mais qu'elle est non vide */
			else if(p_hdr -> typeflag == '5' && !strcmp(p_hdr->size,"000000000000"))
			{
				print("tsh: rmdir: impossible de supprimer '");
				print(argument);
				print("': Le dossier n'est pas vide \n");
				break;
			}
			/* les autres types de fichiers */
			else
			{
				/* appel a rmdir_general pour supprimer le header et le dossier du tar */
				rmdir_general(p_hdr->name, path);
				/* on se redeplace d'un BLOCKSIZE avant car on a doit relire la ou on est */
				lseek(fd,-BLOCKSIZE, SEEK_CUR);
			}			
		}
	}
	// if we didn't find the directory
	if(found == 0)
	{
		print("tsh: rmdir: impossible de supprimer '");
		print(argument);
		print("': Aucun fichier ou dossier de ce type \n");
	}

	// we close the file.tar
	close(fd);
}

void rmdirOutTar(char * src){
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/rmdir","/bin/rmdir",src, (char*) NULL )  <0){
				perror("exec");
				print("rmdir");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}
