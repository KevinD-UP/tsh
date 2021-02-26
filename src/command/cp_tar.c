#include "../header/cp_tar.h"

void launcher_cp(int nbArg, char ** argument, char * path);	
void printSameFileError(char * src, char * dst);
void printMissingArgumentWithR(char * src);
void cp_help();
void cp_tar(char * src, char * dst, char * path);
void cp_tar_r(char * src, char * dst, char * path);
int nbBlockFile(int headersize);
int isDirectory(char * name);
int isSon(char * father, char * son);


int main(int nbArg, char ** argument)
{
	launcher_cp(nbArg, argument, getenv("PATH"));
}

void launcher_cp(int nbArg, char ** argument, char * path)
{	
	if(nbArg == 1)
	{
		print("cp: opérande de fichier manquant\n");
		print("Saisissez « cp --help » pour plus d'informations.\n");
	}
	/* cas pour cp --help */
	else if(nbArg == 2 && strcmp(argument[1],"--help")==0)
	{
		cp_help();
	}
	/*cas pour cp -r SOURCE DESTINATAIRE */
	else if (nbArg == 4 && strcmp(argument[1],"-r")==0)
	{
		if(isDirectory(argument[2])==0)
		{
			print(argument[2]);
			print(" n'est pas un répertoire \n");
		}
		else
		{
			char *src;
  			src = name_in_tar(argument[2], path);
  			char *dst;
  			dst = name_in_tar(argument[3], path);
			cp_tar_r(src, dst, path);
		}
	}
	/* cas pour cp SOURCE DESTINATAIRE */
	else if(nbArg == 3)
	{
		if(strcmp(argument[1], "-r") == 0)
		{
			printMissingArgumentWithR(argument[2]);
		}
		else if(strcmp (argument[1], argument[2]) == 0)
    	{
    		printSameFileError(argument[1], argument[2]);
  		}
  		else if(isDirectory (argument[1])){
  			print("cp: -r non spécifié ; omission du répertoire ");
  			print(argument[1]);
  			print("\n");
  		}
  		else
  		{	
  			char *src;
  			src = name_in_tar(argument[1], path);
  			char *dst;
  			dst = name_in_tar(argument[2], path);
			cp_tar(src, dst, path);
		}
	}
	else
	{
		if(isDirectory(argument[nbArg-1]))
		{
			for(int i = 1; i < nbArg-1; i++)
			{
				char *src;
  				src = name_in_tar(argument[i], path);
  				char *dst;
  				dst = name_in_tar(argument[nbArg-1], path);
  				char * copy = malloc(100);
  				sprintf(copy, "%s%s", dst, argument[i]);
				cp_tar(src, copy, path);
				free(copy);
			}
		}else{
			print("cp: la cible ");
			print("'");
			print(argument[nbArg-1]);
			print("'");
			print(" n'est pas un répertoire \n");
		}
	}
	exit(0);
}

/* cp_help affiche la description de la commande cp dans un tar */
void cp_help()
{
	print("Utilisation : cp [OPTION]... SOURCE... DESTINATION \n");
	print("Copier la SOURCE vers DEST ou plusieurs SOURCEs vers RÉPERTOIRE.\n");
	print("-r, copier récursivement les répertoires \n");
}

void printSameFileError(char * src, char * dst)
{
	print("cp: ");
    print(src);
   	print(" et ");
   	print(dst);
   	print(" identifient le même fichier \n"); 
}

void printMissingArgumentWithR(char * src){
	print("cp -r: opérande de fichier manquant après ");
	print("'");
	print(src);
	print("'\n");
	print("Saisissez « cp --help » pour plus d'informations.\n");
}

void cp_tar(char * src, char * dst, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar
	char buffer[BLOCKSIZE]; 
	struct posix_header * p_hdr;
	unsigned int filesize;

    struct posix_header * copiedHeader = malloc (BLOCKSIZE);
    memset(copiedHeader, 0, BLOCKSIZE);
    sprintf(copiedHeader->name, "%s", dst);

    int found = 0; // found is 1 if we find the file

	do{//Boucle pour trouver le header correspondant au fichier voulu

    //Lecture des headers 

    int rdcount = read(fd,buffer,BLOCKSIZE);
    p_hdr = (struct posix_header *) buffer;

    //Erreur de lecture

    if(rdcount<0){
      print("cp :reading tar file\n");
      close(fd);
      free(copiedHeader);
      return;
    }

    //On atteint la fin du tar sans trouver le header correspondant

    if((p_hdr->name[0]=='\0')){
      print("cp: impossible d'évaluer ");
      print("'");
      print(name(src));
      print("'");
      print(" Aucun fichier ou dossier de ce type\n");
      close(fd);
      free(copiedHeader);
      return;
    }

    //Lire la taille du fichier


    sscanf(p_hdr->size, "%o",&filesize);

    //Si le header correspondant au fichier copié est trouvé on le copie et on sort de la boucle.

    if(strcmp(p_hdr->name, src) == 0){
    	found = 1; //Pour signaler que le fichier a été trouvé.
    	sprintf(copiedHeader -> mode, p_hdr->mode);
		sprintf(copiedHeader -> size, p_hdr->size);
		sprintf(copiedHeader -> mtime, "%lo", time(NULL));
		copiedHeader -> typeflag = p_hdr->typeflag;
		sprintf(copiedHeader -> magic, TMAGIC);
		sprintf(copiedHeader -> version, "00");
		set_checksum(copiedHeader);
		if(check_checksum(copiedHeader) == 0){
			perror("checksum of the header incorrect");
		}
		lseek(fd, 0, SEEK_SET); //Revenir au début du tar
		break;    
	}

    //Sinon on passe au header suivant

    lseek(fd, ((filesize+ BLOCKSIZE - 1) >> BLOCKBITS)*BLOCKSIZE, SEEK_CUR);


  }while(strcmp(p_hdr->name,src)!=0);

  if(found == 1) //Le header correspondant au fichier a été trouvé, donc le fichier existe.
  {
  	char * content;
  	unsigned int wantedfilesize;

		do{

	    //Lecture des headers 

	    int rdcount = read(fd,buffer,BLOCKSIZE);
	    p_hdr = (struct posix_header *) buffer;

	    //Erreur de lecture

	    if(rdcount<0){
	      print("cp :reading tar file\n");
	      close(fd);
	      free(copiedHeader);
	      return;
	    }

	    //Lire la taille du fichier


	    sscanf(p_hdr->size, "%o",&filesize);

	    //On copie le contenu dans 'content'

	    if(strcmp(p_hdr->name, src) == 0){
	    	content = malloc(nbBlockFile(filesize)*BLOCKSIZE);
	    	memset(content, 0, nbBlockFile(filesize)*BLOCKSIZE);
	    	read(fd, content, nbBlockFile(filesize)*BLOCKSIZE);
	    	sscanf(p_hdr->size, "%o",&wantedfilesize);
	    	lseek(fd, 0, SEEK_SET);
			break;    
		}

	    //Sinon on passe au header suivant

	    lseek(fd, ((filesize+ BLOCKSIZE - 1) >> BLOCKBITS)*BLOCKSIZE, SEEK_CUR);


	  }while(strcmp(p_hdr->name,src)!=0);

		do{

		// Lecture du header

		int rdcount = read(fd,buffer, BLOCKSIZE);
		p_hdr = (struct posix_header *)buffer;

		// Gestion d'erreur

		if(rdcount<0){
		  print("cp :reading tar file\n");
		  close(fd);
		  free(copiedHeader);
		  return;
		}

		//Lecture de la taille du fichier du header actuel
		sscanf(p_hdr->size, "%o",&filesize);

		//On est a la fin du tar, on écrit le header copié et le contenu
		if((p_hdr->name[0]=='\0'))
		{
			lseek(fd, -BLOCKSIZE, SEEK_CUR);
			write(fd, copiedHeader, BLOCKSIZE);
			for(int i = 0; i < nbBlockFile(wantedfilesize); i++)
			{
				write(fd, content, BLOCKSIZE);
			}
			memset(buffer, 0, BLOCKSIZE);
			write(fd, buffer, BLOCKSIZE);
			break;
    	}

		//Aller vers le header suivant

		lseek(fd,((filesize+ BLOCKSIZE - 1) >> BLOCKBITS)*BLOCKSIZE, SEEK_CUR);


		}while(1);
		free(content);
	}
	lseek(fd, 0, SEEK_SET);
	close(fd);
	free(copiedHeader);
}

void cp_tar_r(char * src, char * dst, char * path)
{
	cp_tar(src, dst, path);
	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar
	char buffer[BLOCKSIZE]; 
	struct posix_header * p_hdr;
	unsigned int filesize;

	do {
		int rdcount = read(fd,buffer,BLOCKSIZE);
    	p_hdr = (struct posix_header *) buffer;

    	//Gestion d'erreur

		if(rdcount<0){
		  print("cp :reading tar file\n");
		  close(fd);
		  return;
		}

		//Fin du tar
		if(p_hdr->name[0]=='\0'){
    		break;
    	}

		//Lecture de la taille du fichier correspondant au header actuel
		sscanf(p_hdr->size, "%o",&filesize);

    	if(isSon(src, p_hdr->name) == 1){
    		if(isDirectory(p_hdr->name))
    		{
    			char * copy = malloc(100);
    			sprintf(copy, "%s%s", dst, name(p_hdr->name));
    			cp_tar_r(p_hdr->name, copy, path);
    			free(copy);
    		}
    		else
    		{	
    			char * copy = malloc(100);
    			sprintf(copy, "%s%s", dst, name(p_hdr->name));
    			cp_tar(p_hdr->name, copy, path);
    			free(copy);
    		}
    	}
    	lseek(fd,((filesize+ BLOCKSIZE - 1) >> BLOCKBITS)*BLOCKSIZE, SEEK_CUR);


	}while(1);

	lseek(fd, 0, SEEK_SET);
	close(fd);
}

int nbBlockFile(int headersize)
{
	if(headersize == 0){
		return 0;
	}
	else if(headersize < 512){
		return 1;
	}else if(headersize%512 == 0){
		return headersize/512;
	}else{
		return (headersize/512)+1;
	}
}

/* isDirectory retourne 1 si le ficher name est un repertoire
 * dans le tar simplement en regardant si la chaine de caractere
 * se termine par '/' (on essaie maintenir cet invariant dans le tar) 
 * sinon retourne 0 */
int isDirectory(char * name)
{
	if(name[strlen(name)-1] == '/')
	{
		return 1;
	}
	return 0;
}

/* isSon renvoit 1 si le fichier son est dans le fichier
 * (dans l'arborescence de) father sinon 0
 * par exemple : 
 * - isSon("ROOT/FOLDER1/","ROOT/FOLDER1/") retourne 0
 * - isSon("ROOT/FOLDER1/","ROOT/FOLDER1/file2") retourne 1  
 * - isSon("ROOT/FOLDER1","ROOT/FOLDER1/") retourne 0 
 * - isSon("ROOT/FOLDER1/","ROOT/file2") retourne 0
 * - isSon("ROOT/FOLDER1/","ROOT/FOLDER1/FOLDER2/file4") retourne 1*/
int isSon(char * father, char * son)
{
	/* si la taille de son est plus petite que celle de father
	 * alors son ne peut se trouver dans l'arborescence de father */
	if(strlen(son)<strlen(father))
	{
		return 0;
	}
	/* si father et son represente le meme fichier, son ne peut se 
	 * trouver dans l'arborescence de father */
	if(strcmp(father, son) == 0)
	{
		return 0;
	}
	/* on regarde si la chaine de caractere father est presente 
	 * dans la chaine de caractere son */
	for(int i=0;i< strlen(father) ;i++)
	{
		/* s'il existe une difference, alors son ne peut se 
		 * trouver dans l'arborescence de father */
		if(father[i] != son[i])
		{
			return 0;
		}
	}

	if(strcmp(suffixe(father, son), name(son)) != 0)
	{
		return 0;
	}

	return 1;
	
}