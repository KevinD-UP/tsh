
#include "../header/rm_tar.h"

void launcher_rm(int nbArg, char ** argument, char * path);
void rmGeneral( char *argument, char * path);
void rm( char *argument, char * path);
void rm_r(char * argument, char * path);
void rm_help();
void rm_rOutTar(char * src);
void rmOutTar(char * src);
int isSon(char * father, char * son);
void recopie(char * dst, char * src);

int main(int nbArg, char ** argument)
{
	launcher_rm(nbArg, argument, getenv("PATH"));
}

void launcher_rm(int nbArg, char ** argument, char * path)
{
	int posRm = 1;
	int posRm_r = 2;

	/* si il n'y a pas d'argument apres rm ou rm -r */
	if(nbArg == 1 || (nbArg == 2 && strcmp(argument[1], "-r") == 0))
	{
		print("tsh: rm: opérande manquant \n");
		print("Saisissez « rm --help » pour plus d'informations. \n");
	}
	/* rm --help */
	else if(nbArg == 2 && strcmp(argument[1], "--help") == 0)
	{
		rm_help();
	}
	/* rm file .... or rm -r file ...*/
	else
	{
		/* si on est dans le tar */
		if(isTar(path)){
			char * src;	
			/* rm -r */ 
			if(strcmp(argument[1], "-r") == 0)
			{
				while(posRm_r < nbArg)
				{
					src = name_in_tar(argument[posRm_r],path);
					if(isOutTar(src)){
						rm_rOutTar(nameOutTar(src));
					}
					else{
						rm_r(src, path);
					}
					posRm_r ++;
				}
			}
			/* rm */
			else
			{
				while(posRm < nbArg)
				{
					src = name_in_tar(argument[posRm],path);
					if(isOutTar(src)){
						rmOutTar(nameOutTar(src));
					}
					else{
						rm(src, path);
					}
					posRm ++;
				}
			}
			free(src);
		}
		/* on est pas dans le tar */
		else{
			char * newPath;	
			/* rm -r */ 
			if(strcmp(argument[1], "-r") == 0)
			{
				while(posRm_r < nbArg)
				{
					/* l'argument posRm_r doit etre dans le tar */
					if(argInTarAux(argument[posRm_r])){
						newPath = pathIsCorrect(argument[posRm_r], path);
						/* le chemin fourni est incorrecte, on sort de la boucle */
						if( newPath == NULL){
							print("tsh: rm: ");
							print(argument[posRm_r]);
							print(" n'existe pas \n");
							break;
						}
						/* sinon */
						else{
							char * src = analyse_name_in_tar(racine_of_tarBis(argument[posRm_r]));
							rm_r(src,analyse_name_in_tar(newPath));
						}
					}
					/* l'argument posRm_r est en dehors du tar */
					else{
						rm_rOutTar(argument[posRm_r]);
					}
					posRm_r ++;
				}
			}
			/* rm */
			else{
				while(posRm < nbArg){
					/* l'argument posRm_r doit etre dans le tar */
					if(argInTarAux(argument[posRm])){
						newPath = pathIsCorrect(argument[posRm], path);
						/* le chemin fourni est incorrecte, on sort de la boucle */
						if( newPath == NULL){
							print("tsh: rm: ");
							print(argument[posRm]);
							print(" n'existe pas \n");
							break;
						}
						/* sinon */
						else{
							char * src = analyse_name_in_tar(racine_of_tarBis(argument[posRm]));
							rm(src,analyse_name_in_tar(newPath));
						}
					}
					/* l'argument posRm_r est en dehors du tar */
					else{
						rmOutTar(argument[posRm]);
					}
					posRm ++;
				}
			}
		}
	}
	exit(0);
}

/* rm --help */
void rm_help()
{
	print("Utilisation : rm [OPTION]... [FICHIER]... \n");
	print("Supprimer (retirer le lien) le ou les FICHIERs. \n");
	print("-r    remove directories and their contents recursively \n");
	print("--help     afficher l'aide et quitter \n");
	print("Par défaut, rm ne supprime pas les répertoires. Utilisez l'option -r pour supprimer les répertoires, ainsi que l'intégralité de leur contenu. \n");
}

void rmGeneral(char *argument, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}

	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int size = 0; //size of file give by his header
	int found = 0; // found is 1 if we find the file and we can delete it else 0
	char ecriture[BLOCKSIZE]; // to write '\0' at the end of the file if found == 1
	int compteur = -1; // to count how BLOCKSIZE do the file that we want to remove
	int retour = 0;// to count how BLOCKSIZE we do skip before writing in the .tar
		
	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		sscanf(p_hdr->size, "%o", &size); //Update size
		
		/* if we find the file and skip ( header + file ) we begin writing */ 
		if(found == 1 && compteur == 0)
		{
			/* on se decale à retour * BLOCKSIZE en arriere pour commencer l'ecriture 
			   on reecrit par dessus le fichier qu'on souhaite supprimer, on va decaler
			   tous le reste */
			lseek(fd, -(retour * BLOCKSIZE), SEEK_CUR);
			if(write(fd, tampon, BLOCKSIZE) < 0)
			{
				perror("erreur write in rm_tar.c in rm ");
			}
			/* on revient à notre position initiale */
			lseek(fd, (retour-1)*BLOCKSIZE, SEEK_CUR);
		}
		/* if we find the file but we are in the file */
		if(found == 1 && compteur != 0)
		{
			compteur --;
		}
		/* if we find the file ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0 && found == 0){

			found = 1; // we find the file so found = 1
			if(size ==  0)
			{
				compteur = 0;
			}
			else
			{
				compteur = (size/BLOCKSIZE) + 1; // how BLOCKSIZE do the file
			}
			retour = 2 + compteur; // how BLOCKSIZE we do skip ( + 2 because : 1 for the header and 1 for the header afer the file)		
		}
	}

	/* nettoyage de la fin du fichier */

	/* comme la fin du fichier n'est pas reecrit ( car il n'y a rien a ecrire dessus)
	   on la nettoie a 0 vu qu'elle a deja ete ecrite juste avant */
	lseek(fd, -((retour-1) * BLOCKSIZE), SEEK_CUR);
	for(int i=0; i<retour ; i++)
	{
		memset(ecriture, 0, BLOCKSIZE);
		if(write(fd, tampon, BLOCKSIZE) < 0)
		{
			perror("erreur write 2 in rm_tar.c in rm");
		}
	}
	
	struct stat st;
				
	if(stat(file_of_tar(path), &st) == -1){
		perror("stat error in rm_tar in ecriture");
		exit(0);
	}
	if(truncate(file_of_tar(path), st.st_size -  ((retour-1) * BLOCKSIZE)) == -1){
		perror("truncate error in rm_tar in ecriture");
	}
	
	// we close the file.tar
	close(fd);
}

/* rm without option */
void rm(char *argument, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}

	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int found = 0;

	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;

		/* if we find the file ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0){

			found = 1;

			/* si c'est un repertoire */
			if(p_hdr -> typeflag == '5')
			{
				print("tsh: rm: impossible de supprimer '");
				print(argument);
				print("': est un dossier \n");
				break;
			}
			/* les autres types de fichiers */
			else
			{
				/* appel a rmGeneral pour supprimer le header et le fichier du tar */
				rmGeneral(p_hdr->name, path);
				/* on se redeplace d'un BLOCKSIZE avant car on a doit relire la ou on est */
				lseek(fd,-BLOCKSIZE, SEEK_CUR);
			}			
		}
	}
	// if we didn't find the file 
	if(found == 0)
	{
		print("tsh: rm: impossible de supprimer '");
		print(argument);
		print("': Aucun fichier ou dossier de ce type \n");
	}

	// we close the file.tar
	close(fd);
}

/* rm -r */
void rm_r(char * argument, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}

	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int found = 0; // found is 1 if we find the file and we can delete it else 0
	char rep[100]; //pour garder le nom du repertoire qu'on doit supprimer
	memset(rep, 0, 100);

	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		
		/* if we find the file ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0 || isSon(argument, p_hdr -> name) )
		{
			
			/* si c'est un repertoire et c'est celui sur lequel on est appele */
			if(p_hdr -> typeflag == '5' && strcmp(p_hdr->name, argument) == 0)
			{
				found = 1; // on a trouver le repertoire ou un fichier lui appartenant
				recopie(rep,p_hdr -> name);	//on stocke le nom du repertoire qu'on souhaite supprimer
			}
			/* si c'est un repertoire fils -> appel recursif */
			else if(p_hdr -> typeflag == '5')
			{
				/* on met a jour le path */
				path = strcat(path,"/");
				path = strcat(path,argument);
				/* on se rappelle recursivement */
				rm_r(p_hdr->name, path);
				/* on remet a jour le path */
				for(int i=strlen(path)-1;i>0;i--)
				{
					path[i] = '\0';
					if(path[i-1] == '/')
					{
						path[i-1] = '\0';
						break;
					}
				}
				/* on se repositionne a 1 BLOCKSIZE en arriere car vient d'ecrire sur le 
					BLOCKSIZE ou on se trouve */
				lseek(fd,-BLOCKSIZE, SEEK_CUR);
			}
			/* sinon : ce n'est pas un repertoire, on va le supprimer directement */
			else
			{
				/* on appelle rmGeneral */
				rmGeneral(p_hdr->name,path);
				/* on se repositionne a 1 BLOCKSIZE en arriere car vient d'ecrire sur le 
					BLOCKSIZE ou on se trouve */
				lseek(fd,-BLOCKSIZE, SEEK_CUR);
			}			
		}
	}
	// if we didn't find the file 
	if(found == 0)
	{
		print("tsh: rm: impossible de supprimer '");
		print(argument);
		print("': Aucun fichier ou dossier de ce type \n");
	}
	else
	{
		/* et la on appelle rmGeneral sur rep qui est le nom du 
			repertoire qu'on souhaitais supprimer ( car on avait seulement
			supprimer le contenu du repertoire) */
		rmGeneral(rep,path);
	}
	// we close the file.tar
	close(fd);
}

void rm_rOutTar(char * src){
	
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/rm","/bin/rm","-r", src, (char*) NULL )  <0){
				perror("exec");
				print("rm");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}


void rmOutTar(char * src){
	
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/rm","/bin/rm", src, (char*) NULL )  <0){
				perror("exec");
				print("rm");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}

/* return 1 si son appartient au repertoire father, sinon 0 */
int isSon(char * father, char * son)
{
	for(int i=0;i< strlen(father) ;i++)
	{
		if(father[i] != son[i])
		{
			return 0;
		}
	}
	return 1;
}


/* methode qui copie la chaine de caractere src dans dst */
void recopie(char * dst, char * src)
{
	for(int i=0;i< strlen(src); i++)
	{
		dst[i] = src[i];
	}
}
