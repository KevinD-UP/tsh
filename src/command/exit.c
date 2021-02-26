#include "../header/exit.h"

/*
	return 1 if the user call command exit else 0 if he call exit --help
*/
int exitTerminal(int entier, char ** argument)
{
	if(entier == 1)
	{
		return 1;
	}
	else if(entier == 2 && strcmp(argument[1], "--help") == 0 )
	{
		print("exit: exit [n]\n");
    	print("Ends the shell.\n");
    	print("Ends the shell with the return code 'N'.  If N is omitted, the return code is the one of the last executed command.\n");
    	return 0;
	}
	else
	{
		return 1;
	}
}
