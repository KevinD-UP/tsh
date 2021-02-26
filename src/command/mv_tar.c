#include "../header/mv_tar.h"

void launcher_mv(int nbArg, char ** argument, char * path);
void mv_help();
void rename_tar(char * src, char * dst, char * path);
int isSon(char * father, char * son);
int rename_is_possible(char * src, char * dst, char * path);
int isFile(char * name);
int isDirectory(char * name);
void move_tar(char * src, char * dst, char * path);
int createFileInDirInTar(char * src, char * dst, char * path);
int ecriture(char * src, char * dst, char * path);
void writing(char * dst, char * path, char * buffer, int count);
int createDirInDirInTar(char * src, char * dst, char * path);
int longueurBlock(char * src);

int main(int nbArg, char ** argument)
{
	launcher_mv(nbArg, argument, getenv("PATH"));
}

void launcher_mv(int nbArg, char ** argument, char * path)
{	
	/* cas pour mv --help */
	if(nbArg == 2 && strcmp(argument[1],"--help")==0)
	{
		mv_help();
	}
	/* cas pour mv SOURCE DESTINATAIRE */
	else if(nbArg == 3)
	{
		char * src = name_in_tar(argument[1],path);
		char * dst = name_in_tar(argument[2],path);
		
		/* on appelle mv avec des arguments qui se situe en dehors du tar 
		 * on va alors appeler mv (du shell) avec exec */
		if(isOutTar(src) && isOutTar(dst)){
			if(execl("/bin/mv","/bin/mv", nameOutTar(src), nameOutTar(dst), (char*) NULL )  <0){
				perror("exec");
				print("mv");
				print(" : command not found \n");
			}
		}
		/* on appelle mv avec l'argument src qui se trouve en dehors du tar et
		 * dst qui peut etre un nom (pour renommer) ou un fichier pour deplacer */
		else if(isOutTar(src)){
			/* le fichier dst existe dans le tar -> deplacement */
			if(file_exist (dst, path)){
				char * name_src = nameOutTar(src);
				struct stat st;
				
				if(stat(name_src, &st) == -1){
					perror("stat error in mv_tar");
					print("bash: mv: ");
					print(name_src);
					print(": Aucun fichier ou dossier de ce type\n");
					exit(0);
				}
				/* si dst est un repertoire */
				if(isDirectory(dst)){
					printf("repertoire aa \n");
					/* si src est un fichier regulier */
					if(S_ISREG(st.st_mode)){
						/*
						if(createFileInDirInTar(name_src, dst, path)){
							if(remove(name_src)!=0 ){
								perror("fichier pas supprimer");
							}
						}
						else{
							print("bash: mv: ");
							print(name_src);
							print(": l'operation a echouée \n");
						}
						*/
						print("tsh : mv : cette fonctionnalité n'est pas implemntee\n");
					}
					/* si src est un repertoire */	
					if(S_ISDIR(st.st_mode)){
						/*
						if(createDirInDirInTar(name_src, dst, path)){
							if(rmdir(name_src)!=0 ){
								perror("fichier pas supprimer");
							}
						}
						else{
							print("bash: mv: ");
							print(name_src);
							print(": l'operation a echouée \n");
						}
						*/
						print("tsh : mv : cette fonctionnalité n'est pas implemntee\n");
					}					
				}
				/* dst n'est pas un repertoire */
				else{
					
					if(isFile(src)){
						//on supprime dst
						//on cree src a l'emplacement dst avec le nom de dst
						//on ecrit dans le src ( a partir du tar )
						//et on supprime src du tar
						print("tsh : mv : cette fonctionnalité n'est pas implemntee\n");
						
					}
					else{
						print("tsh: mv : impossible d'écraser le non-répertoire '");
						print(argument[2]);
						print("' par le repertoire '");
						print(argument[1]);
						print("'\n");
					}
				}
			}
			/* il n'existe pas -> on renomme */
			else{
				if(execl("/bin/mv","/bin/mv", nameOutTar(src), argument[2], (char*) NULL )  <0){
					perror("exec");
					print("mv");
					print(" : command not found \n");
				}
			}
		}
		/* on appelle mv avec l'argument dst qui se trouve en dehors du tar
		 * on verifie que src existe bien dans le tar, et que dst existe, 
		 * voir si c'est un repertoire ou pas */
		else if(isOutTar(dst)){
			
			/* le fichier src existe dans le tar */
			if(file_exist (src, path)){
				char * name_dst = nameOutTar(dst);
				struct stat st;
				
				if(stat(name_dst, &st) == -1){
					perror("stat error in mv_tar");
					print("bash: mv: ");
					print(name_dst);
					print(": Aucun fichier ou dossier de ce type\n");
					exit(0);
				}
				/* si dst est un repertoire */
				if(st.st_mode == S_IFDIR){
					print("tsh : mv : cette fonctionnalité n'est pas implemntee\n");
					//on cree src dans dst
					//on supprime src du tar
					

				}
				/* dst n'est pas un repertoire */
				else{
					
					if(isFile(src)){
						print("tsh : mv : cette fonctionnalité n'est pas implemntee\n");
						//on supprime dst
						//on cree src a l'emplacement dst avec le nom de dst
						//on ecrit dans le src ( a partir du tar )
						//et on supprime src du tar
						
					}
					else{
						print("tsh: mv : impossible d'écraser le non-répertoire '");
						print(argument[2]);
						print("' par le repertoire '");
						print(argument[1]);
						print("'\n");
					}
				}				
			}
			/* il n'existe pas */
			else
			{
				print("bash: mv: ");
				print(src);
				print(": Aucun fichier ou dossier de ce type\n");
			}
		}
		else{
			
			/* si le premier arg est un nom et le second arg est un fichier dans le tar alors
			   on fait un deplacement de fichier */
			if(file_exist (dst, path) || strlen(dst)==0)
			{	
				/* si arg 2 est un repertoire, on peut faire un deplacement */
				if(isDirectory(dst) || strlen(dst)==0)
				{
					move_tar(src, dst, path);
				}
				/* mv a b -> suppression du fichier b et le fichier a est renommé b */
				else if(isFile(src))
				{
					/* on supprime le fichier dst */
					rm_aux(dst,path);
					/* verification si on peut changer le nom (taille correcte ou si argument[1] existe */
					if(rename_is_possible(src, dst, path))
					{
						rename_tar(src, dst, path);
					}
					
				}
				/* mv a/ b -> erreur mais pas de suppression */
				else
				{
					print("tsh: mv : impossible d'écraser le non-répertoire '");
					print(argument[2]);
					print("' par le repertoire '");
					print(argument[1]);
					print("'\n");
				}
			
			}
			/* si le premier arg est un nom et le second arg n'est pas un fichier dans le tar alors
			   on fait un changement de nom */ 
			else
			{
				/* si les deux noms fournis sont compatibles (mv a b || mv a/ b/) */
				if((isFile(src) && isFile(dst)) || (isDirectory(src) && isDirectory(dst)))
				{	
					/* verification si on peut changer le nom (taille correcte ou si argument[1] existe */
					if(rename_is_possible(src, dst, path))
					{
						rename_tar(src, dst, path);
					}
				}
				/* dans le cas de mv a b/ */
				else if(isFile(src))
				{
					print("tsh: mv :");
					print(argument[1]);
					print(" ne peut etre renommé en repertoire\n");
				}
				/* dans le cas de mv a/ b */
				else
				{
					print("tsh: mv :");
					print(argument[1]);
					print(" ne peut etre renommé en fichier\n");
				}
			}
		}
	}
	else 
	{
		print("tsh: mv : probleme du nombre d'argument\n");
		print("pour des renseignement sur la commande mv faire : mv --help\n");
	}
	exit(0);
}

/* mv_help affiche la description de la commande mv dans un tar */
void mv_help()
{
	print("La commande mv : mv SOURCE DESTINATAIRE \n");
	print("Renommer SOURCE en DESTINATAIRE, ou déplacer SOURCE vers DESTINATAIRE \n");
	print("SOURCE et DESTINATAIRE peuvent etre des noms comme des chemins\n");
	print("La commande mv ne prends aucune option \n");
	print("Si vous souhaitez renommer un repertoire, n'oubliez pas pas '/'");
	print(" à la fin du nom et si vous souhaitez renommer un non-répertoire, ");
	print("ne mettez pas de '/' à la fin du nom\n");
	print("La taille des noms est limitee a 100 caracteres,");
	print(" et dans un tar, le nom du fichier contient son 'arborescence', il se peut ");
	print("vous atteignez rapidement la limite autorisee\n");
}


/* rename_tar permet de renommer le fichier src en dst, 
 * y compris si le fichier src est un repertoire 
 * Si le fichier src est un repertoire, alors on fera 
 * egalement un changement (dans la racine) pour son 
 * arborescence (les fichiers qu'il contient) */
void rename_tar(char * src, char * dst, char * path)
{
	/* OUVERTURE DU TAR */
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}
	
	int n;
	char tampon[BLOCKSIZE];
	struct posix_header * p_hdr;
	
	/* LECTURE DU TAR */
	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		
		/* si on trouve le fichier src, on change le nom
		 * du fichier src à partir de dst */
		if(strcmp(p_hdr->name, src) == 0 ){
			
			memset(p_hdr-> name, 0, 100);
			sprintf(p_hdr->name,"%s", dst);
			set_checksum(p_hdr);
			lseek(fd, -BLOCKSIZE, SEEK_CUR);
			if(write(fd, p_hdr, BLOCKSIZE) < 0)
			{
				perror("erreur write in mv_tar.c in rename_tar 1 ");
			}		
		}
		
		/* si on trouve des fichiers appartenant au ficher src, 
		 * (src est un repertoire), on va changer leur racine,
		 * on utilise la fonction suffixe pour obtenir le 
		 * suffixe (=la chaine de caractere apres src dans leur nom)
		 * afin ce modifier leur nom correctement */
		if(isSon(src, p_hdr -> name))
		{
			char * suf = suffixe(src, p_hdr -> name);
			//printf("nouveau nom : %s%s\n", new_name, suf);
			memset(p_hdr-> name, 0, 100);
			sprintf(p_hdr->name,"%s%s", dst, suf);
			set_checksum(p_hdr);
			lseek(fd, -BLOCKSIZE, SEEK_CUR);
			if(write(fd, p_hdr, BLOCKSIZE) < 0)
			{
				perror("erreur write in mv_tar.c in rename_tar 2");
			}	
		}
	}
	
	close(fd);
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
	/* si la chaine de caractere de son est plus longue que celle 
	 * de father d'un caratere et qu'il n'y a aucune difference 
	 * sur les (strlen(father)) premiers caracteres et que le dernier
	 * caractere de son est '/', on est dans le cas : 
	 * isSon("ROOT/test","ROOT/test/"), 
	 * et ce cas n'est pas accepter */
	if(strlen(son) == strlen(father)+1 && son[strlen(son)-1]=='/')
	{
		return 0;
	}
	return 1;
}



/* rename_is_possible retourne 1 si la taille de la chaine de caractere
 * dst est correcte et que le fichier src est présent dans le tar sinon 
 * retourne 0*/
int rename_is_possible(char * src, char * dst, char * path)
{
	/* si la taille du nouveau nom du fichier est < 100 */   
	if(strlen(dst) < 100)
	{
		/* si la taille du nouveau nom du fichier est plus
		 *  petite que celle du nom actuel,et que le fichier 
		 *  src existe, on peut changer le nom */
		if(strlen(dst) <= strlen(src) && file_exist(src, path ))
		{
			return 1;
		}

		/*sinon */
	
		/* OUVERTURE DU TAR */
		int fd = open(file_of_tar(path), O_RDONLY);
		if (fd < 0)
		{
			perror("error open of tarball in mv_tar in rename_is_possible");
		}
	
		int n;
		char tampon[BLOCKSIZE];
		struct posix_header * p_hdr;
		int found = 0; // found is 1 if we find the file else 0
		
		/* LECTURE DU TAR */
		while((n = read(fd, tampon, BLOCKSIZE)) > 0){
			p_hdr = (struct posix_header*) tampon;
		 
			/* si on trouve le fichier src */
			if(strcmp(p_hdr->name, src) == 0 )
			{
				found = 1;			
			}
			/* si on trouve des fichiers appartenant au ficher src, 
			* (src est un repertoire), on va verifier la taille de leur
			* nom, on utilise la fonction suffixe pour obtenir le 
			* suffixe (=la chaine de caractere apres src dans leur nom)
			* pour avoir la taille complete du nom avec la modification */
			if(isSon(src, p_hdr -> name))
			{
				char * suf = suffixe(src, p_hdr -> name);
				if(strlen(dst) + strlen(suf) > 100)
				{
					print("tsh: mv :");
					print(dst);
					print(suf);
					print(" : le nom est trop long \n");
					close(fd);
					return 0;
				}	
			}
		}
		
		/* si found == 0, cela signifie qu'on a pas trouvé le fichier
		 * src dans le tar */
		if(found == 0)
		{
			print("bash: mv: ");
			print(src);
			print(": Aucun fichier ou dossier de ce type\n");
			close(fd);
			return 0;
		}
		/* on a croisé aucune erreur, on peut alors renomer src en dst */
		close(fd);
		return 1;
	}
	/* le nom est trop long */
	print("tsh: mv :");
	print(dst);
	print(" : le nom est trop long\n");
	return 0;
}



/* isFile retourne 1 si le ficher name est un non-repertoire
 * dans le tar simplement en regardant si la chaine de caractere
 * name  ne se termine pas par '/' (on essaie maintenir
 *  cet invariant dans le tar) sinon retourne 0 */
int isFile(char * name)
{
	if(name[strlen(name)-1] != '/')
	{
		return 1;
	}
	return 0;
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

/* move_tar permet déplacer un fichier src dans dst (qui est un
 * répertoire) */
void move_tar(char * src, char * dst, char * path)
{
	
	char * new_name = strcat(dst,name(src));
	/* verification si on peut changer le nom (taille correcte ou
	 *  si le fichier src existe */
	if(rename_is_possible(src, new_name, path))
	{
		rename_tar(src, new_name, path);
	}
}


/* createFileInDirInTar permet de creer le fichier src qui se trouve en dehors
 * du tar dans le repertoire dst, renvoit 1 si tout s'est bien deroulé sinon
 * renvoit 0 */
int createFileInDirInTar(char * src, char * dst, char * path){
	char * newName = malloc(100 * sizeof(char));
	if(newName == NULL){
		perror("allocation error in mv_tar in createFileInDirTar");
		return 0;
	}
	memset(newName, 0, 100);
	sprintf(newName, "%s%s", dst, name(src));
	touch_aux(newName, path);
	ecriture(src,dst,path);
	return 1;
}

/* longueurBlock renvoie un entier qui represente le nombre de 
 * blocs de 512 octets qu'il faut pour ecrire le contenue du fichier
 * src */
int longueurBlock(char * src){
	int fd = open(src, O_RDONLY);
	char * buf = malloc(BLOCKSIZE * sizeof(char));
	memset(buf, 0, BLOCKSIZE);
	int compteur = 0;
	int n;
	while((n = read(fd, buf, BLOCKSIZE)) > 0){
		compteur++;
	}
	close(fd);
	free(buf);
	return compteur;
}

/* ecriture permet d'ecrire dans le fichier dst qui se trouve dans le tar, le 
 * contenu du fichier src qui se trouve en dehors du tar, renvoit 1 si tout
 * s'est bien deroulé sinon 0 */
int ecriture(char * src, char * dst, char * path){
	int nbBlock = longueurBlock(src);
	
	struct stat st;
				
	if(stat(file_of_tar(path), &st) == -1){
		perror("stat error in mv_tar in ecriture");
		exit(0);
	}
	if(truncate(file_of_tar(path), st.st_size + (nbBlock * BLOCKSIZE)) == -1){
		perror("truncate error in mv_tar in ecriture");
	}
	
	int fd = open(src, O_RDONLY);
	char * buf = malloc(BLOCKSIZE * sizeof(char));
	memset(buf, 0, BLOCKSIZE);
	int compteur = 0;
	int n;
	while((n = read(fd, buf, BLOCKSIZE)) > 0){
		writing(dst, path, buf, compteur);
		compteur++;
		memset(buf, 0, BLOCKSIZE);
	}
	close(fd);
	free(buf);
	return 1;
}
	
void writing(char * dst, char * path, char * buffer, int count){
	int fd = open(file_of_tar(path), O_RDWR);
	if (fd < 0)
	{
		perror("error open of tarball");
	}
	
	int n;
	char tampon[BLOCKSIZE];
	struct posix_header * p_hdr;
	
	/* LECTURE DU TAR */
	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		
		/* si on trouve le fichier src, on change le nom
		 * du fichier src à partir de dst */
		if(strcmp(p_hdr->name, dst) == 0 ){
			lseek(fd, (count * BLOCKSIZE), SEEK_CUR);
			if(write(fd, buffer, BLOCKSIZE) < 0)
			{
				perror("erreur write in mv_tar.c in writing ");
			}
			break;
		}
	}
	close(fd);
}
	

/* createDirInDirInTar permet de creer le repertoire src qui se trouve en dehors
 * du tar dans le repertoire dst, renvoit 1 si tout s'est bien deroulé sinon
 * renvoit 0 */
int createDirInDirInTar(char * src, char * dst, char * path){
	char * newName = malloc(100 * sizeof(char));
	if(newName == NULL){
		perror("allocation error in mv_tar in createDirInDirTar");
		return 0;
	}
	
	memset(newName, 0, 100);
	/* name_src est le nom du repertoire representée par la chaine de 
	 * caractere src */
	char * name_src = name(src);
	/* si l'utilisateur a mis '/' apres le nom du rep, on ne le
	 * rajoute pas dans le tar */
	if(name_src[strlen(name_src)-1] == '/'){
		sprintf(newName, "%s%s", dst, name_src);
	}
	/* sinon on le rajoute pour maintenir une coherance dans le tar */
	else{
		sprintf(newName, "%s%s/", dst, name_src);
	}
	
	mkdir_aux(newName, path);
	
	/* newSrc sera utilise pour stocker le chemin des fichiers
	 * qui sont dans le repertoire src */
	char * newSrc = malloc(100 * sizeof(char));
	if(newSrc == NULL){
		perror("allocation error in mv_tar in createDirInDirTar");
		return 0;
	}
	
	DIR * dirp = opendir(src);
	struct dirent *d;
	
	/* on parcourt le repertoire src */
	while((d = readdir(dirp)) != NULL)
	{
		/* on prends pas en compte . et .. */
		if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0){
			memset(newSrc, 0, 100);
			sprintf(newSrc, "%s%s", src, d->d_name);
			/* si c'est un fichier regulier */
			if(d -> d_type == DT_REG){
				createFileInDirInTar(newSrc, newName, path);
			}
			/* si c'est un repertoire */
			if(d -> d_type == DT_DIR){
				createDirInDirInTar(newSrc, newName, path);
			}
		}
	}
	closedir(dirp);
	free(d);
	free(newSrc);
	free(name_src);
	free(newName);
	
	//on fait cat 
	return 1;
}
