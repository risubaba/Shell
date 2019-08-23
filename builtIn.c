#include "builtIn.h"

void cd(char *argsForCommand)
{
	if (!strcmp("", argsForCommand))
	{
		if (chdir(swd))
		{
			perror("Error ");
		}
	}
	else
	{
		adjustForTilda(argsForCommand);
		if (chdir(argsForCommand))
		{
			perror("Error ");
		}
	}
}

void pwd(char argvs[1024][1024])
{
	printf("%s", argvs[0]);
	if (strcmp(argvs[0], ""))
	{
		printf("Too many arguments\n");
	}
	else if (getcwd(cwd, PATH_MAX) != NULL)
	{
		printf("%s\n", cwd);
	}
}

void Echo(char *argsForCommand)
{
    for (int j = 0; j < strlen(argsForCommand); j++)
    {
        if (argsForCommand[j] != ' ')
            printf("%c", argsForCommand[j]);
        else
        {
            while (argsForCommand[j]==' '){
                j++;
            }
            j--;
            printf(" ");
        }
    }
    printf("\n");
}

void executeInBuiltCommand()
{

	char curCommand[INP_MAX];
	for (int i = 0, k = 0; i <= strlen(inp); i++)
	{
		if (inp[i] != ';' && i < strlen(inp) && inp[i] != '\0')
			curCommand[k++] = inp[i];
		else
		{
			curCommand[k] = '\0';
			if (curCommand[k - 1] == '\n')
				curCommand[k - 1] = '\0';
			char argsForCommand[INP_MAX];
			int inOffset = parseInput(curCommand, argsForCommand);
			char argvs[1024][1024];
			int argc = parseArgsForCommand(argsForCommand, argvs);
			if (!strcmp(curCommand + inOffset, "cd"))
			{
				cd(argvs[0]);
			}
			else if (!strcmp(curCommand + inOffset, "pwd"))
			{
				pwd(argvs);
			}
			else if (!strcmp(curCommand + inOffset, "echo"))
			{
				Echo(argsForCommand);
			}
			else if (!strcmp(curCommand + inOffset, "ls"))
			{
				ls(argvs, argc);
			}
			else if (!strcmp(curCommand + inOffset, "pinfo"))
			{
				pinfo(argvs, argc);
			}
			else if (!strcmp(curCommand + inOffset, "history"))
			{
				history(argvs, argc);
			}
			else if (!strcmp(curCommand + inOffset, "nightswatch"))
			{
				nightswatch(argvs, argc);
			}
			else
			{	
				executeCommand(curCommand, argvs, argc);
			}
			k = 0;
		}
	}
}