#include "../header/parser.h"

char * analyse(char *c);
int analyseP(char * c, int debut);
void decoupage(char * c, char separateur, argument * a);
void taille(char * c, char separateur, argument * a);


argument * parser(char * buffer)
{
	argument * b = malloc(sizeof(argument));

	char * buf = analyse(buffer);
	printf("%s\n",buf);
	printf("\n");
	taille(buf, ' ', b);
	decoupage(buf, ' ', b);
	return b;
}

//return une chaine de caractere (reduction de la chaine lu par read depuis STDIN_FILENO)
char * analyse(char *c)
{	
	int debut = 1;
	int compteur = 0;
	char * retour = malloc(BUFSIZE*sizeof(char));
	for(int i=0; i<BUFSIZE; i++)
	{
		if(isspace(c[i]) && (debut == 0))
		{
			if(analyseP(c, i+1) == 0)
			{
				retour[compteur] = c[i];
				compteur ++;
			}
			else
			{
				//printf("break\n");
				break;
			}
		}
		if(isalnum(c[i]) || ispunct(c[i])){
			//printf("%c\n", c[i]);
			//printf("%d\n", i);
			//printf("comp : %d\n", compteur);
			debut = 0;
			retour[compteur] = c[i];
			compteur ++;
		}
	}
	retour[compteur] = '\0';
	retour = realloc( retour, (compteur+1) * sizeof(char) );
	return retour;
}

//return 1 s'il ne reste que des espaces ou des caractere non imprimable sinon 0
int analyseP(char * c, int debut)
{
	for(int i=debut; i<BUFSIZE; i++)
	{
		if(isprint(c[i]) && (isspace(c[i]) == 0))
		{
			//printf("aaaaaaaaaaaaaaaaaa\n");
			//printf("%c \n", c[i]);
			return 0;
		}
	}
	return 1;
}

//return un tableau de chaine de caractere a partir de la chaine de caractere reduit
//la chaine est separer selon le separateur
void decoupage(char * c, char separateur, argument * arguments)
{	
	
	arguments -> args = malloc(arguments -> nbArg * sizeof(argument));
	int debut = 0;
	int compteur = 0;
	for(int i=0; i< strlen(c); i++)
	{
		if(c[i] == separateur)
		{
			arguments -> args[compteur] = malloc((i-debut+1)*sizeof(char));
			for(int j=debut; j < i; j++)
			{
				arguments -> args[compteur][j-debut] = c[j];
			}
			arguments -> args[compteur][i-debut] = '\0';
			compteur ++;
			debut = i+1;

		}
	}
	arguments -> args[compteur] = malloc((strlen(c)-debut+1)*sizeof(char));
	for(int l=debut; l<strlen(c); l++)
	{
		arguments -> args[compteur][l-debut] = c[l];
	}
	arguments -> args[compteur][strlen(c)-debut] = '\0';
}

//retour la longueur du tableau de chaine de caractere
void taille(char * c, char separateur, argument * arg)
{
	int compteur=0;
	for(int i=0; i< strlen(c); i++)
	{
		if(c[i] == separateur)
		{
			compteur ++;
		}
	}
	arg -> nbArg = compteur + 1;
}