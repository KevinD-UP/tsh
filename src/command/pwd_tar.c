#include "../header/pwd_tar.h"

int pwd_tar(int nbArg, char ** argument, char * path);

int main(int nbArg, char ** argument)
{
	pwd_tar(nbArg, argument, getenv("PATH"));
}

int pwd_tar(int nbArg, char ** argument, char * path)
{
	/* PWD */
	if(nbArg == 1)
	{
		print(path);
		print("\n");
	}
	/* PWD --HELP / PWD -[LP]*/
	else if(nbArg == 2)
	{
		/* gestion des liens symbolique pas encore implementer */
		if(strcmp(argument[1], "-P") == 0)
		{
			print(path);
			print("\n");
		}
		/* repertoire de travail courant */
		if(strcmp(argument[1], "-L") == 0)
		{
			print(path);
			print("\n");
		}
		/* help */
		else if(strcmp(argument[1],"--help") == 0)
		{
			print("pwd: pwd [-LP]\n");
			print("\t Affiche le nom du répertoire de travail courant.\n \n");
			print("\t Options :\n");
			print("\t -L	affiche la valeur de $PWD s'il nomme le répertoire de travail courant\n");
    		print("\t -P	affiche le répertoire physique, sans aucun lien symbolique\n \n");
    		print("\t Par défaut, « pwd » se comporte comme si « -L » était spécifié.\n \n");
    		print("\t Code de retour : \n\t Renvoie 0 à moins qu'une option non valable ne soit donnée ou que le répertoire courant ne peut pas être lu.\n");
		}
		/* bad option/argument useless*/
		else
		{
			/* bad options */
			if(argument[1][0] == '-')
			{
				print("tsh: pwd: ");
				print(argument[1]);
				print(" : option non valable\npwd : utilisation : pwd [-LP]\n");
			}
			/* argument useless */
			else
			{
				print(path);
				print("\n");
			}
		}
	}
	/* argument useless */
	else
	{
		print(path);
		print("\n");
	}
	return 0;
}