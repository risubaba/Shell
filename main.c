#include "bonus.h"
#include "pinfo.h"
#include "ls.h"
#include "builtIn.h"
#include "executeCommand.h"
#include <signal.h>
#include "jobs.h"

#define INP_MAX 1024
#define PATH_MAX 4096
#define RESET "\x1B[0m"
#define KCYN "\x1B[36m"
#define CYN "\x1B[36m"

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];
int piddd[100];
char jobs_command[100][4096];
//set error messages wherever required
//add <usage> to error messages
//try to adjust for ls -l ~/child (Wrong permissions)
//add recalled command to .history file
//implement cd .. when at / (root)
//shift all parsing commands to one file
//adjust ls for using pipe multiple times
//adjust for tab properly "ls \t doesn't work"
//change up arrow recall to work in 1 enter -> up arrow enter -> command executed directly
//handle ctrl-D

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
		int len = strlen(argsForCommand);
		for (int i = 0; i < len; i++)
		{
			argsForCommand[i + strlen(cwd) + 1] = argsForCommand[i];			// exit(0);
		}
		argsForCommand[strlen(cwd)] = '/';
		for (int i = 0; i < strlen(cwd); i++)
			argsForCommand[i] = cwd[i];
		argsForCommand[strlen(cwd) + len + 1] = '\0';
	}
	else
	{
		int len = strlen(argsForCommand);

		for (int i = 0; i < len; i++)
			argsForCommand[i + strlen(swd)] = argsForCommand[i + 1];
		for (int i = 0; i < strlen(swd); i++)
			argsForCommand[i] = swd[i];
	}
}

int parseInput(char *curCommand, char *argsForCommand)
{
	int inOffset = 0, k = strlen(curCommand);

	while (curCommand[inOffset] == ' ' || curCommand[inOffset] == '\t')
		inOffset++;
	int argsOffset = inOffset;
	while (curCommand[argsOffset] != ' ' && curCommand[argsOffset] != '\0' && curCommand[argsOffset] != '\t')
		argsOffset++;
	curCommand[argsOffset++] = '\0';
	while (curCommand[argsOffset] == ' ' || curCommand[argsOffset] == '\t')
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
	// printf("%s\n",argsForCommand);
	for (int i = 0; i < strlen(argsForCommand); i++)
	{
		if (argsForCommand[i] != ' ' || argsForCommand[i] == '\0' && argsForCommand[i] != '\t')
		{
			argvs[k][j] = argsForCommand[i], j++;
		}
		else if (j != 0)
		{
			argvs[k][j] = '\0', k++, j = 0;
		}
	}
	argvs[k][j] = '\0';
	if (j)
		k++;
	argvs[k][0] = '\0';
	return k;
}

int checkInpRedir(char argvs[1024][1024], int *argc, char input_file[])
{
	int i = 0;
	for (; i < *argc; i++)
	{
		if (!strcmp(argvs[i], "<"))
		{
			if (i == *argc - 1)
			{
				printf("Wrong Syntax for input redirection\n");
				return -1;
			}
			strcpy(input_file, argvs[i + 1]);
			argvs[i][0] = '\0';
			*argc = i;
			return 1;
		}
	}
	return 0;
}

int checkOutRedir(char argvs[1024][1024], int *argc, char output_file[])
{
	int i = 0;
	for (; i < *argc; i++)
	{
		if (!strcmp(argvs[i], ">"))
		{
			if (i == *argc - 1)
			{
				printf("Wrong Syntax for output redirection\n");
				return -1;
			}
			strcpy(output_file, argvs[i + 1]);
			argvs[i][0] = '\0';
			*argc = i;
			return 1;
		}
		else if (!strcmp(argvs[i], ">>"))
		{
			if (i == *argc - 1)
			{
				printf("Wrong Syntax for output redirection\n");
				return -1;
			}
			strcpy(output_file, argvs[i + 1]);
			argvs[i][0] = '\0';
			*argc = i;
			return 2;
		}
	}
	return 0;
}

int checkPipe(char argvs[1024][1024], int *argc)
{
	int i = 0;
	int ret = 0;
	for (; i < *argc; i++)
	{
		if (!strcmp(argvs[i], "|"))
		{
			if (i == *argc - 1)
			{
				printf("Wrong Syntax for piping\n");
				return -1;
			}
			// strcpy(argvs[i]," ");
			ret++;
		}
	}
	return ret;
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
	while (inp_ch = getchar())
	{
		// printf("%c\n",inp_ch);
		if (inp_ch == EOF || inp_ch == '\n')
		{
			inp[i] = '\0';
			break;
			exit(0);
		}
		else if (inp_ch == '\033')
		{
			inp_ch = getchar();
			inp_ch = getchar();
			if (inp_ch == 'A')
				ret++;
			else
			{
				ret = -1;
				break;
			}
		}
		else
		{
			inp[i] = inp_ch;
		}
		i++;
	}
	inp[i] = '\0';
	return ret;
}

void handler(int x)
{
	// printf("\n%d\n",fg_process_pid);
	if (fg_process_pid)
		kill(fg_process_pid, x);
}

int main()
{
	memset(piddd, -1, 400);
	getcwd(swd, PATH_MAX);
	int hist = initializeHistory();
	char recall_command[1024];
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	signal(SIGTSTP, handler);
	initializeJobs();
	while (1)
	{
		printSystemName();
		// return 1;
		int recall = get_input();
		// printf("%s\n",inp);

		if (inp[0] == ' ' || inp[0] == '\0')
			continue;

		if (hist)
			if (recall == 0)
				addHistory(inp);
		if (recall > 0)
		{
			strcpy(recall_command, recall_history(recall));
			printSystemName();
			recall_command[strlen(recall_command) - 1] = '\0';
			printf("%s", recall_command);
			char ch[1024];
			fgets(ch, 1024, stdin);
			if (strlen(ch) == 1)
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