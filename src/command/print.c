#include "../header/print.h"


void print(char * c)
{
	int taille = strlen(c);
	write(STDOUT_FILENO, c, taille);
}