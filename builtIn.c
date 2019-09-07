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
	if (strcmp(argvs[0], ""))
	{
		printf("Too many arguments\n");
	}
	else if (getcwd(cwd, PATH_MAX) != NULL)
	{
		printf("%s\n", cwd);
	}
}

void shell_exit()
{
	printf("Are you sure you want to exit [y/N]??");
	char ex = getchar();
	if (ex == 'y' || ex == 'Y')
	{
		exit(0);
	}
}

void Echo(char argvs[1024][1024], int argc)
{
	for (int i = 0; i < argc; i++)
	{
		printf("%s ", argvs[i]);
	}
	printf("\n");
}

char commands[][1024] = {
	"cd", "pwd", "echo", "ls", "pinfo", "history", "nightswatch", "clear", "quit", "cronjob", "jobs"};

int commandtoExecute(int inOffset, char curCommand[1024], char argvs[1024][1024], int argc)
{
	if (!strcmp(curCommand + inOffset, commands[0]))
	{
		cd(argvs[0]);
	}
	else if (!strcmp(curCommand + inOffset, commands[1]))
	{
		pwd(argvs);
	}
	else if (!strcmp(curCommand + inOffset, commands[2]))
	{
		Echo(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[3]))
	{
		ls(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[4]))
	{
		pinfo(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[5]))
	{
		history(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[6]))
	{
		nightswatch(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[7]))
	{
		printf("\e[1;1H\e[2J");
	}
	else if (!strcmp(curCommand + inOffset, commands[8]))
	{
		shell_exit();
	}
	else if (!strcmp(curCommand + inOffset, commands[9]))
	{
		cronjob(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[10]))
	{
		jobs(argvs, argc);
	}
	else
	{
		executeCommand(curCommand, argvs, argc);
	}
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
			commandtoExecute(inOffset, curCommand, argvs, argc);
			k = 0;
		}
	}
}