#include "bonus.h"
#include "pinfo.h"
#include "ls.h"
#include "builtIn.h"
#include "executeCommand.h"

#define INP_MAX 1024
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

int to_int(char *num)
{
	int ret = 0;
	for (int i = 0; i < strlen(num); i++)
		ret = ret * 10 + num[i] - '0';
	return ret;
}

int main()
{
	getcwd(swd, PATH_MAX);
	using_history();
	while (1)
	{
		printSystemName();
		fgets(inp, INP_MAX, stdin);
		add_history(inp);
		executeInBuiltCommand();
	}
}