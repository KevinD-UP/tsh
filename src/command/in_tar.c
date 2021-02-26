#include "../header/in_tar.h"

//return 1 si on se situe dans un repertoire .tar (un tarball), 0 sinon
int isTar(char * path)
{
	for(int i=0; i< (strlen(path)-3); i++)
	{
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r')
		{
			return 1;
		}
	}
	return 0;
}

//return le nom du fichier.tar
char * file_of_tar(char * path)
{
	int length = strlen(path);
	int fin;
	int debut;

	for(int i=0; i< (length-3); i++)
	{
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r')
		{
			fin=i+3;
		}
	}

	for(int i=fin-3; i>0; i--)
	{
		if(path[i-1]=='/')
		{
			debut = i;
			break;
		}
	}

	char * file = malloc((fin - debut + 2)*sizeof(char));

	for(int i=debut;i<=fin;i++)
	{
		file[i-debut] = path[i];
	}
	file[fin-debut+1] = '\0';
	return file;
}

/* return la racine du tar, c'est a dire dans notre tar tous les noms de fichier sont de la 
 * forme ROOT/ ROOT/FOLDER1/ .... 
 * Par exemple : 
 * - racine_of_tar("doc/root.tar/file") -> "file/"*/
char * racine_of_tar(char * path)
{
	int debut;

	for(int i=0; i< strlen(path)-3; i++)
	{
		// i+4 == strlen(path) pour savoir si on arrive a la fin du path
		//  path[i+4]=='/' comme on est pas a la fin du path, on cherche le debut de la racine qui commence
		//apres file.tar/.... RACINE .....
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r' && (i+4 == strlen(path) || path[i+4]=='/')) 
		{
			debut = i + 4;
		}
	}

	//cela signifie qu'il n'y a plus rien apres le .tar 
	//dans le path, donc on vient de rentrer dans le tar
	//la racine est donc null
	if(debut == strlen(path))
	{
		return NULL;
	}

	char * racine = calloc((strlen(path)-debut+1),sizeof(char));
	if(racine == NULL)
	{
		perror("allocation error in racine_of_tar in in_tar");
	}
	//on commence a debut+1 car path[debut] = / et on ne souhaite pas 
	//recuperer ce slash
	for(int i=debut+1;i<strlen(path);i++)
	{
		racine[i-debut-1] = path[i];
	}
	//on oublie pas d'ajouter ce dernier slash
	//printf("racine1 : %s \n", racine);
	racine[strlen(path)-debut] = '\0';
	//printf("racine2 : %s \n", racine);
	if(racine[strlen(racine)-1] != '/')
	{
		racine = strcat(racine, "/");
	}
	//printf("racine3 : %s \n", racine);
	return racine;
}

/* return la racine du tar, c'est a dire dans notre tar tous les noms de fichier sont de la 
 * forme ROOT/ ROOT/FOLDER1/ .... sans rien rajouter a la fin, contrairement a 
 * racine_of_tar qui rajoutait un '/' 
 * Par exemple : 
 * - racine_of_tar("doc/root.tar/file") -> "file"*/
char * racine_of_tarBis(char * path)
{
	int debut;

	for(int i=0; i< strlen(path)-3; i++)
	{
		// i+4 == strlen(path) pour savoir si on arrive a la fin du path
		//  path[i+4]=='/' comme on est pas a la fin du path, on cherche le debut de la racine qui commence
		//apres file.tar/.... RACINE .....
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r' && (i+4 == strlen(path) || path[i+4]=='/')) 
		{
			debut = i + 4;
		}
	}

	//cela signifie qu'il n'y a plus rien apres le .tar 
	//dans le path, donc on vient de rentrer dans le tar
	//la racine est donc null
	if(debut == strlen(path))
	{
		return NULL;
	}

	char * racine = calloc((strlen(path)-debut+1),sizeof(char));
	if(racine == NULL)
	{
		perror("allocation error in racine_of_tar in in_tar");
	}
	//on commence a debut+1 car path[debut] = / et on ne souhaite pas 
	//recuperer ce slash
	for(int i=debut+1;i<strlen(path);i++)
	{
		racine[i-debut-1] = path[i];
	}
	//on oublie pas d'ajouter ce dernier slash
	//printf("racine1 : %s \n", racine);
	racine[strlen(path)-debut] = '\0';
	//printf("racine2 : %s \n", racine);
	//printf("racine3 : %s \n", racine);
	return racine;
}

/* argInTarAux retourne 1 si la chaine de caractere fourni contient 
 * un tarball sinon 0 
 * Par exemple : 
 * argInTarAux("command/ROOT.tar/file") -> 1
 * argInTarAux("command/doc/file") -> 0 */
int argInTarAux(char * path)
{
	if(strlen(path) < 4){
		return 0;
	}
	for(int i=0; i< (strlen(path)-4); i++)
	{
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r' && path[i+4] == '/' )
		{
			return 1;
		}
	}
	return 0;
}

/* argInTar : parcourt un tableau de chaine de caractere et verifie 
 * pour chaque chaine de caractere si elle possede le tarball et renvoit
 * 1 dans ce cas sinon 0 */
int argInTar(char ** argument, int nbArg){
	for(int i=0;i<nbArg; i++){
		if(argInTarAux(argument[i])){
			return 1;
		}
	}
	return 0;
}

/* pathIsCorrect retourne une chaine de caractere representant le chemin 
 * pour allez au tarball depuis notre repertoire courant a partir d'un chemin
 * fourni(name) si celui fourni est correct, sinon on retourne NULL 
 * Par exemple : 
 * - pathIsCorrect("doc/ROOT.tar/file", "home") -> "home/doc/ROOT.tar"*/
char * pathIsCorrect(char * name, char * path){
	
	struct stat st;
				
	if(stat(path_of_tar(name), &st) == -1){
		perror("stat error in mv_tar in ecriture");
		return NULL;
	}
	path = strcat(path,"/");
	path = strcat(path,path_of_tar(name));
	return path;
}

/* path_of_tar retourne une chaine de caractere qui represente 
 * le chemin pour allez jusqu'au tarball a partir d'une chaine de 
 * caractere fourni (path) 
 * Par exemple : 
 * - path_of_tar("home/command/ROOT.tar/ROOT/") -> "home/command/ROOT.tar"*/
char * path_of_tar(char * path)
{
	int length = strlen(path);
	int fin;

	for(int i=0; i< (length-3); i++)
	{
		if(path[i] =='.' && path[i+1] == 't' && path[i+2] == 'a' && path[i+3] == 'r')
		{
			fin=i+3;
		}
	}


	char * file = malloc((fin + 2)*sizeof(char));
	memset(file,0,fin+2);
	for(int i=0;i<=fin;i++)
	{
		file[i] = path[i];
	}
	return file;
}
