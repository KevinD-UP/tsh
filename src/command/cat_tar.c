#include "../header/cat_tar.h"

int launcher_cat(char** argument, int argc, char* path);
int cat_tar_bis(char * name, char* path);
void catOutTar(char * src);
int nbBlockFile(int headersize);

int main(int nbArg, char ** argument){
   // cat_tar(argument,nbArg,getenv("PATH"));
   launcher_cat(argument, nbArg, getenv("PATH"));
}

int launcher_cat(char** argument, int nbArg, char* path){
	
	char cat_buffer[BUFSIZ];
    int b_read = 0;
    int b_written = 0;
    int b_total_written = 0;
    
    //if cat has been prompted without any args :
    if(nbArg == 1){
        while ((b_read = read(STDIN_FILENO, cat_buffer, BUFSIZ)) > 0) {
            while ((b_written = write(STDOUT_FILENO, cat_buffer + b_total_written, b_read - b_total_written)) < 0)
                b_total_written += b_written;
        if (b_written  == -1) return -1;
        }
        return b_read;
        //then we must read from stdin until we get a EOF
        
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
					catOutTar(nameOutTar(src));
				}
				/* le rep src doit etre dans le tar */
				else{
					cat_tar_bis(src, path);
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
						print("tsh: cat: ");
						print(argument[i]);
						print(" n'existe pas \n");
						break;
					}
					/* sinon */
					else{
						char * src = analyse_name_in_tar(racine_of_tarBis(argument[i]));
						cat_tar_bis(src,analyse_name_in_tar(newPath));
					}
				}
				/* l'argument i doit etre en dehors du tar */
				else{
					catOutTar(argument[i]);
				}
			}
		}
		
	}
	return 0;
	
}


int cat_tar_bis(char * argument, char* path){
	
	int fd = open(file_of_tar(path), O_RDONLY);
	if (fd < 0)
	{
		perror("error open of tarball");
		exit(fd);
	}
	
	char tampon[BLOCKSIZE];
				
	struct posix_header * p_hdr;

	int n;
	int found = 0;
	int nb_blocks = 0;
	int size;

	while((n = read(fd, tampon, BLOCKSIZE)) > 0){
		p_hdr = (struct posix_header*) tampon;
		sscanf(p_hdr->size, "%o", &size); //Update the size
		
		if(found == 1 && nb_blocks > 0 ){
			print(tampon);
			nb_blocks --;
		}
		
		/* if we find the directory ( i mean his header ) */
		if(strcmp(p_hdr->name, argument) == 0){

			found = 1;

			/* si c'est un rep */
			if(p_hdr -> typeflag == '5')
			{
				print("tsh: cat: '");
				print(argument);
				print("': est un dossier \n");
				break;
			}
			
			/* les autres types de fichiers */
			else
			{
				nb_blocks = nbBlockFile(size);
			}			
		}
	}
	// if we didn't find the directory
	if(found == 0)
	{
		print("tsh: cat: '");
		print(argument);
		print("': Aucun fichier de ce type \n");
	}

	// we close the file.tar
	close(fd);
	return 0;
}

void catOutTar(char * src){
	int n = fork();
	switch(n)
	{
		case -1 : //error
			perror("error fork exit");
			EXIT_FAILURE ;
			break;

		case 0 : //son
			if(execl("/bin/cat","/bin/cat",src, (char*) NULL )  <0){
				perror("exec");
				print("tsh: cat");
				print(" : command not found \n");
				exit(0);
			}
			break;
		
		default : //father
			wait(NULL);
			break;
	}
}

	

int nbBlockFile(int headersize)
{
    if(headersize == 0){
        return 0;
    }
    if(headersize < 512){
        return 1;
    }
    else if(headersize % 512 == 0){
        return headersize/512;
    }else{
        return (headersize/512)+1;
    }
}
