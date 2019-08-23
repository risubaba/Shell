#include <sys/wait.h>
#include <curses.h>
#include "bonus.h"
#include "pinfo.h"
#include "ls.h"

#define INP_MAX 100
#define PATH_MAX 4096

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];

//set error messages wherever required
//add colour to printf
//add <usage> to error messages
//add exit message for ^D
//try to adjust for ls -l ~/child (Wrong permissions)
//add history size
//if & at the end change argc to argc-1

char *directorySet(char *cwd, char *swd)
{

	for (int i = 0, k = 0; i < PATH_MAX; i++)
	{
		if (swd[i] == '\0')
		{

			ret[k++] = '~';
			for (int j = i; j < PATH_MAX; j++)
			{
				ret[k++] = cwd[i++];
			}
			return ret;
		}
		else if (cwd[i] == '\0')
		{

			return cwd;
		}
	}
}

void printSystemName()
{

	char *uName;
	uName = getlogin();
	char sName[1024];
	gethostname(sName, 1024);
	if (getcwd(cwd, PATH_MAX) != NULL)
	{
		char *directory;
		directory = directorySet(cwd, swd);
		printf("<%s@%s:%s>", uName, sName, directory);
	}
}

void adjustForTilda(char *argsForCommand)
{
	int n = strlen(argsForCommand);
	int len = strlen(argsForCommand);
	if (argsForCommand[0] != '~')
	{
		for (int i = 0; i < strlen(argsForCommand); i++)
			argsForCommand[i + strlen(cwd) + 1] = argsForCommand[i];
		argsForCommand[strlen(cwd)] = '/';
		for (int i = 0; i < strlen(cwd); i++)
			argsForCommand[i] = cwd[i];
		argsForCommand[strlen(cwd) + len + 1] = '\0';
	}
	else
	{
		for (int i = 0; i < strlen(argsForCommand); i++)
			argsForCommand[i + strlen(swd)] = argsForCommand[i + 1];
		for (int i = 0; i < strlen(swd); i++)
			argsForCommand[i] = swd[i];
	}
}



int parseInput(char *curCommand, char *argsForCommand)
{
	int inOffset = 0, k = strlen(curCommand);

	while (curCommand[inOffset] == ' ')
		inOffset++;
	int argsOffset = inOffset;
	while (curCommand[argsOffset] != ' ' && curCommand[argsOffset] != '\0')
		argsOffset++;
	curCommand[argsOffset++] = '\0';
	while (curCommand[argsOffset] == ' ')
		argsOffset++;
	int l = 0;
	for (int j = argsOffset; j <= k; j++, l++)
	{
		argsForCommand[l] = curCommand[j];
	}
	argsForCommand[l] = '\0';

	return inOffset;
}

int parseArgsForCommand(char *argsForCommand, char argvs[1024][1024])
{
	int k = 0, j = 0;
	for (int i = 0; i < strlen(argsForCommand); i++)
	{
		if (argsForCommand[i] != ' ' || argsForCommand[i] == '\0')
		{
			argvs[k][j] = argsForCommand[i], j++;
		}
		else
		{
			argvs[k][j] = '\0', k++, j = 0;
		}
	}
	argvs[k][j] = '\0';
	if (k || j)
		k++;
	argvs[k][0] = '\0';
	return k;
}

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
	if (argsForCommand[0] == '\"')
	{
		for (int j = 1; j < strlen(argsForCommand); j++)
		{
			if (argsForCommand[j] != '\"')
				printf("%c", argsForCommand[j]);
			else
			{
				printf("\n");
				break;
			}
		}
	}
	else if (argsForCommand[0] == '\'')
	{
		for (int j = 1; j < strlen(argsForCommand); j++)
		{
			if (argsForCommand[j] != '\'')
				printf("%c", argsForCommand[j]);
			else
			{
				printf("\n");
				break;
			}
		}
	}
	else
	{
		for (int j = 0; j < strlen(argsForCommand); j++)
		{
			if (argsForCommand[j] != ' ')
				printf("%c", argsForCommand[j]);
			else
			{
				printf("\n");
				break;
			}
		}
		if (argsForCommand[strlen(argsForCommand) - 1] != ' ')
			printf("\n");
	}
}

int to_int(char *num)
{
	int ret = 0;
	for (int i = 0; i < strlen(num); i++)
		ret = ret * 10 + num[i] - '0';
	return ret;
}

void executeCommand(char *curCommand, char argvs[1024][1024], int argc)
{
	char *new_argvs[1024];
	for (int i = 0; i < argc + 1; i++)
	{
		if (!i)
			new_argvs[i] = curCommand;
		else
			new_argvs[i] = argvs[i - 1];
	}
	// for (int i=0;i<argc+1;i++)
	// 	printf("%s\n",new_argvs[i]);

	pid_t pid = fork();
	if (pid < 0)
	{
		printf("Not able to fork properly\n");
		return;
	}
	else if (pid == 0)
	{
		if (new_argvs[argc][0] == '&' && strlen(new_argvs[argc]) == 1)
		{
			new_argvs[argc][0] = '\0';
			pid_t pid_child = fork();
			if (pid_child < 0)
			{
				printf("Not able to fork properly\n");
				return;
			}
			else if (pid_child == 0)
			{
				if (execvp(new_argvs[0], new_argvs) < 0)
				{
					printf("Error in executing command\n");
				}
			}
			else
			{
				int status;
				waitpid(pid_child, &status, 0);
				if (status == 0)
				{
					printf("\nCommand %s with pid %d exited normally\n", new_argvs[0], (int)getpid());
				}
				else
				{
					printf("\nCommand %s with pid %d exited abnormally\n", new_argvs[0], (int)getpid());
				}
			}
		}
		else if (execvp(new_argvs[0], new_argvs) < 0)
		{
			printf("Error in executing command\n");
		}
	}
	else
	{
		if (new_argvs[argc][0] == '&' && strlen(new_argvs[argc]) == 1)
		{
			new_argvs[argc][0] = '\0';

			printf("Process started with pid %d\n", (int)pid);
		}
		else
		{
			int status = 0;
			waitpid(pid, &status, 0);
			if (status == 0)
			{
				printf("\nCommand %s with pid %d exited normally\n", new_argvs[0], (int)pid);
			}
			else
			{
				printf("\nCommand %s with pid %d exited abnormally\n", new_argvs[0], (int)pid);
			}
		}
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

int main()
{
	getcwd(swd, PATH_MAX);
	using_history();
	// int fd1 = open("/etc/inputrc",O_APPEND);
	// write(fd1,"set history-size 20\n",strlen("set history-size 20\n"));
	// close(fd1);
	while (1)
	{
		printSystemName();
		fgets(inp, INP_MAX, stdin);
		add_history(inp);
		executeInBuiltCommand();
	}
}