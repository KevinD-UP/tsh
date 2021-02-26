 #include "../header/analyse_lexicale.h"

int analyseP(char * c, int debut);

//return a string of characters (reduction of the string read by read since STDIN_FILENO)
char * analyse(char *c)
{	
	/* INITIALISATION */
	int debut = 1;
	int compteur = 0;
	char * retour = malloc((strlen(c)+1)*sizeof(char));
	if(retour == NULL)
	{
		perror("allocation error in analyse");
	}
	
	/* REDUCTION */
	for(int i=0; i<strlen(c); i++)
	{
		if(isspace(c[i]) && (debut == 0))
		{
			// if there are still useful characters to copy
			if(analyseP(c, i+1) == 0)
			{
				retour[compteur] = c[i];
				compteur ++;
			}
			else
			{
				break;
			}
		}
		// copy begin if character i is punct's char or if is a letter or a number
		if(isalnum(c[i]) || ispunct(c[i])){
			debut = 0;
			retour[compteur] = c[i];
			compteur ++;
		}
	}

	retour[compteur] = '\0';
	retour = realloc( retour, (compteur+1) * sizeof(char) );
	if(retour == NULL)
	{
		perror("reallocation error in analyse");
	}
	return retour;
}

//return 1 if only spaces or non-printable characters remain else 0
int analyseP(char * c, int debut)
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

/*
a string pointer from the reduced string
the chain is separated according to the separator
*/
char ** decoupage(char * c, char separateur, int taille)
{	
	/* INITIALISATION */
	char **tableau = malloc((taille+1)* sizeof(*tableau)); // +1 pour le dernier element NULL
	if(tableau == NULL)
	{
		perror("allocation error in decoupage");
	}
	int debut = 0;
	int compteur = 0;

	/* MANAGEMENT */
	for(int i=0; i< strlen(c); i++)
	{
		if(c[i] == separateur)
		{
			tableau[compteur] = malloc((i-debut+1)*sizeof(char));
			if(tableau[compteur] == NULL)
			{
				perror("allocation error in for in decoupage");
			}
			/* copy the string */
			for(int j=debut; j < i; j++)
			{
				tableau[compteur][j-debut] = c[j];
			}
			tableau[compteur][i-debut] = '\0';
			/* update of variables */
			compteur ++;
			debut = i+1;
		}
	}

	/* LAST STRING OR THE ONE STRING ( IF THERE IS NO SEPARATOR IN C )*/
	tableau[compteur] = malloc((strlen(c)-debut+1)*sizeof(char));
	if(tableau[compteur] == NULL)
	{
		perror("allocation error in last for in decoupage");
	}
	/* copy the string */
	for(int l=debut; l<strlen(c); l++)
	{
		tableau[compteur][l-debut] = c[l];
	}
	tableau[compteur][strlen(c)-debut] = '\0';
	/* on rajoute un element NULL pour execv */
	tableau[taille] = NULL;
	return tableau;
}

//return the length of the string array
int taille(char * c, char separateur)
{
	int compteur = 0;
	for(int i=0; i< strlen(c); i++)
	{
		if(c[i] == separateur)
		{
			compteur ++;
		}
	}
	return compteur+1;
}

char ** analysePipe(int debut, int fin, char ** arg)
{
	int taille = fin - debut;
	char **tableau = malloc((taille+1)* sizeof(*tableau)); // +1 pour le dernier element NULL
	if(tableau == NULL)
	{
		perror("allocation error inpipe");
	}
	int compteur = 0;
	for (int i=debut; i<fin; i++)
	{
		tableau[compteur] = malloc((strlen(arg[i])+1)*sizeof(char));
		if(tableau[compteur] == NULL)
		{
			perror("allocation error in for in pipe 2");
		}
		memset(tableau[compteur], 0, strlen(arg[i]));
		strcpy(tableau[compteur], arg[i]);
		compteur ++;
	}
	tableau[taille] = NULL;
	return tableau;
}