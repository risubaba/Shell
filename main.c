#include "bonus.h"
#include "pinfo.h"
#include "ls.h"
#include "builtIn.h"
#include "executeCommand.h"

#define INP_MAX 1024
#define PATH_MAX 4096
#define RESET "\x1B[0m"
#define KCYN "\x1B[36m"
#define CYN "\x1B[36m"

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];

//set error messages wherever required
//add colour to printf
//add <usage> to error messages
//add exit message for ^D
//try to adjust for ls -l ~/child (Wrong permissions)

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
		printf(KCYN "<%s@%s:%s>" RESET, uName, sName, directory);
	}
	else
	{
		printf("Error getting current working directorty file path\n");
		return;
	}
}

void adjustForTilda(char *argsForCommand)
{
	int n = strlen(argsForCommand);
	int len = strlen(argsForCommand);
	if (getcwd(cwd, 4096) == NULL)
		printf("Error in updating cwd\n");
	if (argsForCommand[0] != '~')
	{
		if (argsForCommand[0] == '/')
			return;
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

int get_input()
{
	char inp_ch = '\0';
	int i = 0, ret = 0;
	int last_car = 0;
	while (inp_ch != '\n')
	{
		if (inp_ch == '\033')
			last_car = i;
		if (inp_ch == 'A')
		{
			if (i - last_car == 2)
			{
				if (i > 3 && ret)
					ret++;
				else if (i == 3)
					ret++;
				else
				{
					ret = -1;
					inp[0] = '\n';
					while ((getchar()) != '\n')
						; // to flush something
					return ret;
				}
			}
		}
		scanf("%c", &inp_ch);
		inp[i++] = inp_ch;
	}
	inp[i - 1] = '\0';
	return ret;
}

int main()
{
	getcwd(swd, PATH_MAX);
	int hist = initializeHistory();
	char recall_command[1024];
	while (1)
	{
		printSystemName();
		int recall = get_input();
		if (hist)
			if (recall == 0)
				addHistory(inp);
		if (recall > 0)
		{
			strcpy(recall_command, recall_history(recall));
			printSystemName();
			recall_command[strlen(recall_command)-1]='\0';
			printf("%s",recall_command);
			char ch[1024];
			fgets(ch,1024,stdin);
			if (strlen(ch)==1)
			{
				strcpy(inp, recall_command);
				recall = 0;
			}
			else
			{
				recall = -1;
				continue;
			}
		}
		if (recall == 0)
			executeInBuiltCommand();
	}
}