#include "../header/fonction_aux.h"

/*============================TOUCH==========================*/

void touch_aux(char * argument, char * path){

	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar 

	int n;
	
	char buffer[BLOCKSIZE]; //Reader of the header

	struct posix_header * p_hdr; //Pointeur of the header 

	//Init the new Header
	struct posix_header * newFile = malloc(BLOCKSIZE);
	memset(newFile, 0, BLOCKSIZE);
	sprintf(newFile -> name, "%s",argument);
	sprintf(newFile -> mode, "0000700");
	sprintf(newFile -> size, "%011o", 0);
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

/*============================MKDIR==========================*/


void mkdir_aux(char *argument, char * path)
{
	int fd = open(file_of_tar(path), O_RDWR); //Opening the tar 

	int n;
	
	char buffer[BLOCKSIZE]; //Reader of the header

	struct posix_header * p_hdr; //Pointeur of the header 

	//Init the new Header
	struct posix_header * newDir = malloc(BLOCKSIZE);
	memset(newDir, 0, BLOCKSIZE);
	sprintf(newDir -> name, "%s",argument);
	sprintf(newDir -> mode, "000000700");
	sprintf(newDir -> size, "%011o", 0);
	newDir -> typeflag = '5'; //Typeflag for a directory
	sprintf(newDir -> magic, TMAGIC);
	sprintf(newDir -> version, "00");
	set_checksum(newDir);
	if(check_checksum(newDir) == 0){
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
			write(fd, newDir, BLOCKSIZE); //Write the header
			break; //Exit the loop
		}
	}
	lseek(fd, 0, SEEK_SET);
	close(fd);
	free(newDir);
}

/*============================RM FILE==========================*/

void rm_aux(char *argument, char * path)
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

/*============================FUNCTIONS AUX IN TAR==========================*/

/*=========================NOM COMPLET==========================*/

/* name_in_tar renvoie le nom complet du fichier representé par 
 * la chaine de caractere name (name peut etre le nom du fichier
 * ou un chemin y conduisant), c'est à dire le nom representant 
 * ce fichier dans le tar (son chemin depuis le debut du tar) */
char * name_in_tar(char * name, char * path)
{
	char * racine = malloc(100 * sizeof(char));
	if(racine==NULL)
	{
		perror("allocation error in mv_tar in name_in_tar");
	}
	memset(racine, 0, 100);
	
	if(racine_of_tar(path) == NULL)
	{
		strcpy(racine, name);
	}
	else
	{
		sprintf(racine, "%s%s",racine_of_tar(path),name); 
	}
	/* on utilise la fonction analyse_name_in_tar pour vérifier que le 
	 * nouveau nom est correct car on peut fournir un chemin à
	 * name_in_tar et il peut contenir des "../" et le modifier 
	 * en consequence */ 
	char * new_name = analyse_name_in_tar(racine);
	return new_name;
}

/* analyse_name_in_tar va analyser une chaine de caractere chaine qui 
 * represente un chemin qui peut contenir des ../ alors on doit retourner
 * une chaine de caractere representant ce chemin 'mis a jour'
 * Par exemple : 
 * - analyse_name_in_tar("ROOT/../file2") renvoie "file2
 * - analyse_name_in_tar("../ROOT/FOLDER1/../") renvoie "../ROOT/" */
char * analyse_name_in_tar(char * chaine)
{
	int longueur = strlen(chaine);
	int debut;
	int pos;
	int changed = 1;
	
	/* tant que peut faire des modifications */
	while(changed==1)
	{
		debut = 0;
		pos = 0;
		changed = 0;
		
		/* on parcourt la chaine de caractere chaine (sauf
		 * les deux derniers caracteres, pas besoin) */
		for(int i=0; i<longueur-2; i++)
		{
			/* si on croise '/', on met à jour debut et pos */
			if(chaine[i]=='/')
			{
				debut = pos;
				pos = i+1;
			}
			/* si on croise "../" */
			if(chaine[i]=='.' && chaine[i+1]=='.' && chaine[i+2]=='/')
			{
				/* si on a pos != 0 cela signifie qu'on a vu des 
				 * repertoires avant ('/') */
				if(pos!=0)
				{
					/* on met a 0 les caracteres qui represente le 
					 * répertoire qui precedait ce ../ et le ../ */
					for(int j=debut;j<i+3;j++)
					{
						//printf("%c\n", chaine
						chaine[j]=0;
					}
					/* on a fait un changement */
					changed = 1;
					break;
				}
				/* on a croisé aucun repertoire avant donc on ne peut
				 * pas supprimer ce ../, et on avance de i+2 pour
				 * voir s'il reste des ../ qui peuvent etre supprimé */
				else
				{
					i=i+2;
				}
			}
		}
	}
	
	char * new_name = malloc(100*sizeof(char));
	if(new_name == NULL)
	{
		perror("allocation error in mv_tar in analyse_name_in_tar");
	}
	memset(new_name, 0, 100);
	int curseur = 0;
	/* on remplit la chaine de caractere new_name à partir des caracteres
	 * restants dans chaine */
	for(int k=0;k<longueur;k++)
	{
		if(chaine[k]!=0)
		{
			new_name[curseur]=chaine[k];
			curseur++;
		}		
	}
	return new_name;
}

/*=====================NAME FILES OUT TAR=======================*/


/* isOutTar renvoie 1 si le fichier fourni se trouve en dehors du tar, 
 * sinon 0, (on cherche ../ au debut de la chaine caractere car cela indique 
 * que le fichier(path) fournit se trouve en dehors du tar)
 * Par exemple : 
 * - isOutTar("../../texte/file1") -> 1
 * - isOutTar("docum/texte") -> 0 */
int isOutTar(char * name)
{
	if(strlen(name) >= 3 && name[0]=='.' && name[1]=='.' && name[2]=='/'){
		return 1;
	}
	return 0;
}

/* nameOutTar retourne une chaine de caractere qui represente le path 
 * de name dans le shell interactif (bash), on recopie name sauf les 3 
 * premiers caracteres "../" car isOutTar a ete verifie avant.
 * Par exemple : 
 * - nameOutTar("../document/file") -> "document/file" 
 * - nameOutTar("../doc/") -> "doc/" */
char * nameOutTar(char * name){
	char * buf = malloc((strlen(name)-2)*sizeof(char));
	if(buf == NULL)
	{
		perror("allocation error in mv_tar in nameOutTar");
	}
	memset(buf,0,strlen(buf));
	for(int i=3; i<strlen(name); i++){
		buf[i-3] = name[i];
	}
	return buf;
}

/*=====================NAME FILES=======================*/

/* name retourne une chaine de caractere qui represente un nom de 
 * fichier (repertoire ou non repertoire).
 * Par exemple : 
 * - name("ROOT/FOLDER1/") retourne "FOLDER1/"
 * - name("ROOT/FOLDER1/file2") retourne "file2" */
char * name(char * chaine)
{
	int debut = 0;
	/* on commence par la fin (plus presicément à partir de l'avant 
	 * dernier caractere) car on peut avoir '/' au dernier caractere */
	for(int i=strlen(chaine)-2;i>=0;i--)
	{
		/* des qu'on croise '/' on initialise debut et on sort du for */
		if(chaine[i] == '/')
		{
			debut = i+1;
			break;
		}
	}

	char * name = malloc((strlen(chaine)-debut+1) * sizeof(char));
	if(name==NULL)
	{
		perror("allocation error in mv_tar in name");
	}
	memset(name, 0, (strlen(chaine)-debut+1));
	/* on remplit la chaine de caractere name grace à debut */
	for(int j=debut; j < strlen(chaine); j++)
	{
		name[j-debut] = chaine[j];
	}
	//printf("name is : %s\n",name);
	return name;
}


/* path_name retourne une chaine de caractere qui represente le chemin
 * du fichier (repertoire ou non repertoire). Cette fonction est l'inverse 
 * de name.
 * Par exemple : 
 * - path_name("ROOT/FOLDER1/") retourne "ROOT/"
 * - path_name("ROOT/FOLDER1/file2") retourne "ROOT/FOLDER1/" */
char * path_name(char * chaine)
{
	int fin = strlen(chaine);
	/* on commence par la fin (plus presicément à partir de l'avant 
	 * dernier caractere) car on peut avoir '/' au dernier caractere */
	for(int i=strlen(chaine)-2;i>=0;i--)
	{
		/* des qu'on croise '/' on initialise fin et on sort du for */
		if(chaine[i] == '/')
		{
			fin = i+1;
			break;
		}
	}
	/* si on a rencontré aucun /, alors ce fichier n'a pas de path */
	if(fin == strlen(chaine)){
		return NULL;
	}
	char * name = malloc((fin+1) * sizeof(char));
	if(name==NULL)
	{
		perror("allocation error in mv_tar in name");
	}
	memset(name, 0, (fin+1));
	/* on remplit la chaine de caractere name grace à debut */
	for(int j=0; j < fin; j++)
	{
		name[j] = chaine[j];
	}
	//printf("name is : %s\n",name);
	return name;
}

/*=====================ANALYSE IN TAR=====================*/

/* suffixe retourne une chaine de caractere qui est le 
 * suffixe de la chaine de caractere de son par rapport à 
 * la chaine de caractere father (renvoie la chaine de caractere
 * qui represente la difference entre father et son). Le fichier
 * son est dans l'arborescence de father.
 * par exemple : 
 * - suffixe("ROOT/FOLDER1/","ROOT/FOLDER1/file4") renvoie "file4"
 * - suffixe("ROOT/FOLDER1/","ROOT/FOLDER1/file/file2") renvoie "file/file2" */
char * suffixe(char * name, char * son)
{
	int debut = strlen(name);
	
	char * suf = malloc((strlen(son)-strlen(name))*sizeof(char));
	if(suf==NULL)
	{
		perror("allocation error in mv_tar in suffixe");
	}
	
	memset(suf, 0, strlen(son)-strlen(name));
	/* on parcourt la chaine de caractere son à partir de strlen(name)
	 * et on stocke les caracteres qu'on voit dans suf */ 
	for(int i=debut; i< strlen(son); i++)
	{
		suf[i-debut] = son[i];
	}
	return suf;
}

/* file_exist retourne 1 si le fichier name est present dans le 
 * tar sinon 0 */
int file_exist(char * name, char * path)
{
	/* OUVERTURE DU TAR */
	int fd = open(file_of_tar(path), O_RDONLY);
	if (fd < 0)
	{
		perror("error open of tarball in mv_tar in file_exist");
	}
	
	int n;
	char tampon[BLOCKSIZE];
	struct posix_header * p_hdr;
	int found = 0; // found is 1 if we find the file else 0
	
	/* LECTURE DU TAR */
	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		 
			if(strcmp(p_hdr->name, name) == 0 )
			{
				found = 1;			
			}
	}
	return found;
}
