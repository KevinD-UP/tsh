#include "../header/cd.h"

int cd(char * argument, char * path);
int cd_tar(char * argument, char * path);
int vide(char * c, int debut);
void cd_home(char ** argument, char * path);
char * cleanBufPoint(char * buf, int begin);
int home(char * path);

int cd(char * argument, char * path)
{

	DIR * dirp = opendir(".");
	//chdir(".");

	if(strcmp(argument, "..") == 0)
	{
		chdir("..");

	}
	else if(strcmp(argument, ".") == 0)
	{
		chdir(".");
	}
	// in the case where the user do : cd file.tar
	else if(isTar(argument))
	{
		struct dirent *d;

		while((d = readdir(dirp)) != NULL)
		{

			if(strcmp(d->d_name,argument) == 0){
					
				path = strcat(path,"/");
				path = strcat(path,argument);
				closedir(dirp);
				return 0;							
			}
		}
		closedir(dirp);
		/* WE DON'T FIND THE FILE */
		print("tsh: cd: ");
		print(argument);
		print(": Aucun fichier ou dossier de ce type\n");	
		return -1;
	}
	else
	{		
		/* NOT IN TARBALL */
		struct dirent *d;

		while((d = readdir(dirp)) != NULL)
		{
			if(strcmp(d->d_name,argument) == 0){
					
				if(chdir(argument)!= 0)
				{
					perror("-error chdir");
				}
				getcwd(path, BUFSIZE);
				closedir(dirp);
				return 0;							
			}
		}

		/* WE DON'T FIND THE FILE */
		print("tsh: cd: ");
		print(argument);
		print(": Aucun fichier ou dossier de ce type\n");
		return -1;
	}

	getcwd(path, BUFSIZE);
	closedir(dirp);
	return 0;
}





int cd_tar(char * argument, char * path)
{
	if(strcmp(argument, "..") == 0 || strcmp(argument, "../") == 0)
	{
		for(int i=strlen(path)-1;i>0;i--)
		{
			path[i] = '\0';
			if(path[i-1] == '/')
			{
				path[i-1] = '\0';
				break;
			}
		}
		return 0;
	}
	else if(strcmp(argument, ".") == 0 || strcmp(argument, "./") == 0)
	{
		return 0;
	}
	else
	{
		/* IN TARBALL */
			
		if (isTar(path))
		{
			int fd = open(file_of_tar(path), O_RDONLY);
			int n;

			if (fd < 0)
			{
				perror("error open of tarball");
			}

			char tampon[BUFSIZE];
				
			struct posix_header * p_hdr;

			char * name = racine_of_tar(path);
			
			//si c'est null cela signifie qu'on vient tout juste
			//d'entrer dans le tar
			if(name == NULL)
			{
				name = argument;
			}
			//sinon on entrain de parcourir le tar et alors
			//on ajoute notre chemin a la racine du tar
			else
			{
				name = strcat(name, argument);
			}
		
			while((n = read(fd, tampon, BUFSIZE)) > 0){
				p_hdr = (struct posix_header*) tampon;
				//printf("%s \n", p_hdr -> name);
				
				if(strcmp(p_hdr->name, name) == 0){

					if(p_hdr -> typeflag == '5')//pour savoir si c'est un repertoire
					{
						path = strcat(path,"/");
						path = strcat(path,argument);
						path[strlen(path)-1] = '\0';
						close(fd);
						return 0;
					}
					else
					{
						print("tsh: cd: ");
						print(argument);
						print(": N'est pas un dossier\n");
						close(fd);
						return -1;
					}
						
				}

			}
			print("tsh: cd: ");
			print(argument);
			print(": Aucun fichier ou dossier de ce type\n");
			close(fd);
			return -1;
		}
	}
	
	return 0;
}

int launcher_cd(int nbArg, char ** argument, char * path)
{
	if(nbArg == 1)
	{
		cd_home(argument, path);
		return 0;
	}
	else if(nbArg == 2)
	{
		int end = 0;
		char *buf = malloc((strlen(argument[1])+1)*sizeof(char));
		if(buf == NULL)
		{
			perror("allocation error in laucher_cd in cd.c");
		}
		
		int compteur = 0;
		//printf("arg : %s : longueur %ld \n", argument[1],strlen(argument[1]));
		for(int i=0; i<strlen(argument[1]); i++)
		{
			
			buf = cleanBufPoint(buf, i-compteur);
			//printf("%s : longueur %ld ~~~~%c\n", buf,strlen(buf), argument[1][i]);
			if(argument[1][i] == '/')
			{	
				/* in Tarball */
				if(isTar(path))
				{
					buf[i-compteur] = argument[1][i];
					buf[i-compteur+1] = '\0';
					if(cd_tar(buf, path)== -1){
						return 0;
					}
				}
				/* not in tarball */
				else
				{
					if(cd(buf, path) == -1){
						return 0;
					}
				}
				if(i+1 <strlen(argument[1]))
				{
					buf = (char *)realloc(buf, (strlen(argument[1])-i)*sizeof(char));
					if(buf == NULL)
					{
						perror("reallocation error in laucher_cd in cd.c");
					}
					compteur = i+1;

				}
				else
				{
					end = 1;
					
				}
			}
			else
			{
				buf[i-compteur] = argument[1][i];
				if(i+1 == strlen(argument[1]))
				{
					buf[i-compteur+1] = '\0';
				}
			}
		}
		
		/* dans le cas ou il n'y a pas de / */
		if(vide(buf,0)==0 && end == 0){
			/* in Tarball */
			if(isTar(path))
			{	
				cd_tar(buf,path);
			}
			/* not in tarball */
			else
			{
				 cd(buf, path);
			}
		}
		//print("arrive\n");
		free(buf);
		return 0;
	}
	else
	{
		print("tsh: cd: Trop d'arguments\n");
		return 0;
	}
}

/* methode pour la commande cd sans argument ( cd home directory ) */
void cd_home(char ** argument, char * path)
{
	while(home(path) == 0)
	{
		if(isTar(path))
		{
			cd_tar("..", path);
		}
		else
		{
			cd("..", path);
		}
	}
}

char * cleanBufPoint(char * buf, int begin)
{	
	//printf("%s : longueur %ld \n", buf,strlen(buf));
	for(int i=begin; i<strlen(buf); i++)
	{
		buf[i] = '\0';
	}
	return buf;
}

//return 1 if only spaces or non-printable characters remain else 0
int vide(char * c, int debut)
{
	for(int i=debut; i<strlen(c); i++)
	{
		if(isprint(c[i]) && (isspace(c[i]) == 0))
		{
			return 0;
		}
	}
	return 1;
}

/* return 1 si on est au home directory /home/repertoire sinon 0 */
int home(char * path)
{
	int compteur = 0;
	for(int i=0; i<strlen(path); i++)
	{
		if(path[i]== '/')
		{
			compteur++;
		}
	}
	int res = (compteur == 2 && strlen(path) > 5 && path[0] == '/' && path[1] == 'h' && path[2] == 'o' && path[3] == 'm' && path[4] == 'e') ? 1 : 0;
	return res;
}
