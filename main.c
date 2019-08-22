#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <curses.h>

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

int setlsFlag(char argvs[1024][1024], int argc)
{
	int ret = -1;
	char lsValues[][10] = {"", "-l", "-a", "-al", "-la"};
	for (int i = 0; i < argc; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (!strcmp(argvs[i], lsValues[j]))
			{
				if ((ret == 1 && j == 2) || (ret == 2 && j == 1))
				{
					ret = 4;
					break;
				}
				else
					ret = j;
			}
		}
	}
	return ret;
}

void lsPrint(char *name)
{
	struct passwd *pw;
	struct group *gp;
	struct stat sb;

	stat(name, &sb);

	printf((S_ISDIR(sb.st_mode)) ? "d" : "-");
	printf((sb.st_mode & S_IRUSR) ? "r" : "-");
	printf((sb.st_mode & S_IWUSR) ? "w" : "-");
	printf((sb.st_mode & S_IXUSR) ? "x" : "-");
	printf((sb.st_mode & S_IRGRP) ? "r" : "-");
	printf((sb.st_mode & S_IWGRP) ? "w" : "-");
	printf((sb.st_mode & S_IXGRP) ? "x" : "-");
	printf((sb.st_mode & S_IROTH) ? "r" : "-");
	printf((sb.st_mode & S_IWOTH) ? "w" : "-");
	printf((sb.st_mode & S_IXOTH) ? "x" : "-");
	printf(" ");
	printf("%ld ", sb.st_nlink);
	pw = getpwuid(sb.st_uid);
	printf("%s ", pw->pw_name);
	gp = getgrgid(sb.st_gid);
	printf("%s ", gp->gr_name);
	printf("%5ld ", sb.st_size);
	char *c = ctime(&sb.st_mtime);
	for (int i = 4; i <= 15; i++)
		printf("%c", c[i]);
	printf(" ");
	printf("%s\n", name);
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
		{
			argsForCommand[i + strlen(swd)] = argsForCommand[i + 1];
		}
		// argsForCommand[strlen(swd)]='/';

		for (int i = 0; i < strlen(swd); i++)
			argsForCommand[i] = swd[i];
		// argsForCommand[strlen(cwd)+len+1]='\0';
	}
}

void ls(char argvs[1024][1024], int argc)
{
	int lsFlag = setlsFlag(argvs, argc);
	char dirname[100];
	DIR *p;
	struct dirent *d;

	if (lsFlag == -1)
	{
		if (!strcmp("", argvs[0]))
		{
			strcpy(dirname, ".");
		}
		else
		{
			adjustForTilda(argvs[0]);
			strcpy(dirname, argvs[0]);
		}
	}
	else
	{
		if (argvs[argc - 1][0] != '-')
		{
			adjustForTilda(argvs[argc - 1]);
			strcpy(dirname, argvs[argc - 1]);
		}
		else
			strcpy(dirname, ".");
	}

	p = opendir(dirname);
	if (p == NULL)
	{
		perror("Cannot find directory");
	}
	else
	{
		while (d = readdir(p))
		{
			if (lsFlag == 0)
			{
				if (d->d_name[0] != '.')
					printf("%s\n", d->d_name);
			}
			else if (lsFlag == 1)
			{
				if (d->d_name[0] != '.')
					lsPrint(d->d_name);
			}
			else if (lsFlag == 2)
			{
				printf("%s\n", d->d_name);
			}
			else if (lsFlag > 2)
			{
				lsPrint(d->d_name);
			}
			else if (lsFlag < 0)
			{
				if (d->d_name[0] != '.')
					printf("%s\n", d->d_name);
			}
		}
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

void printPinfoLocation(pid_t pid)
{

	char buff[100];
	char path[32];
	sprintf(path, "/proc/%d/exe", pid);
	readlink(path, buff, 100);
	char *location;
	location = directorySet(buff, swd);
	for (int i = strlen(location) - 1; i > -1; i--)
		if (location[i] > 'z' || location[i] < 'A')
		{
			if (location[i] != '~' && location[i] != '/' && location[i] != '.')
			{
				location[i] = '\0';
				break;
			}
		}
	printf("Executable Path -- %s\n", location);
}

void printPinfoStateandMemory(pid_t pid)
{

	char *buff;
	size_t buffsize = 0;
	char path[32];
	sprintf(path, "/proc/%d/status", pid);
	FILE *fd = fopen(path, "r");
	for (int i = 0; i < 3; i++)
		getline(&buff, &buffsize, fd);
	printf("Process Status -- %c\n", buff[7]);
	for (int i = 0; i < 14; i++)
		getline(&buff, &buffsize, fd);
	printf("Memory -- %s", buff + 10);
}

int checkPid(pid_t pid)
{
	char path[32];
	sprintf(path, "/proc/%d/status", pid);
	int fd = open(path, O_RDONLY);
	if (fd == -1)
		return 0;
	return 1;
}

void pinfo(char argvs[1024][1024], int argc)
{
	pid_t pid;
	if (argc == 0)
		pid = getpid();
	else if (argc == 1)
		pid = to_int(argvs[0]);
	else if (argc > 1)
	{
		printf("Too many arguments\n");
		return;
	}

	if (!checkPid(pid))
		return;
	printf("pid -- %d\n", pid);
	printPinfoStateandMemory(pid);
	printPinfoLocation(pid);
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

void history(char argvs[1024][1024], int argc)
{
	int len_required = 100;
	if (argc == 1)
	{
		len_required = to_int(argvs[0]);
	}

	HISTORY_STATE *myhist = history_get_history_state();

	HIST_ENTRY **mylist = history_list();

	int count = myhist->length > len_required ? len_required : myhist->length;
	for (int i = myhist->length - 1; i > -1 && count > 0; i--)
	{
		if (i != myhist->length - 1)
		{
			if (strcmp(mylist[i]->line, mylist[i + 1]->line))
			{
				printf("%s\n", mylist[i]->line);
				count--;
			}
		}
		else
		{
			printf("%s\n", mylist[i]->line);
			count--;
		}
	}
	putchar('\n');
}

void nightswatch_dirty()
{
	char *buff;
	size_t buffsize = 0;
	char path[32] = "/proc/meminfo\0";
	FILE *fd = fopen(path, "r");
	for (int i = 0; i < 17; i++)
		getline(&buff, &buffsize, fd);
	printf("%s\n", buff);
}

void nightswatch_interrupt_setup()
{
	char *buff;
	size_t buffsize = 0;
	char path[32] = "/proc/interrupts\0";
	FILE *fd = fopen(path, "r");
	getline(&buff, &buffsize, fd);
	printf("%s\n", buff);
}

void nightswatch_interrupt()
{
	char *buff;
	size_t buffsize = 0;
	char path[32] = "/proc/interrupts\0";
	FILE *fd = fopen(path, "r");
	for (int i = 0; i < 3; i++)
		getline(&buff, &buffsize, fd);
	printf("%s\n", buff);
}

void nightswatch(char argvs[1024][1024], int argc)
{
	int ttime = to_int(argvs[1]);
	if (ttime == 0)
	{
		//error handling
	}
	int type;
	if (!strcmp(argvs[2], "interrupt"))
	{
		type = 1;
	}
	else if (!strcmp(argvs[2], "dirty"))
	{
		type = 0;
	}
	else
	{
		//error handling
	}

	time_t starttime = time(NULL), prevtime = time(NULL);
	if (type == 1)
	{
		nightswatch_interrupt_setup();
	}
	while (1)
	{
		time_t curtime = time(NULL);
		if ((curtime - starttime) % ttime == 0 && curtime != prevtime)
		{
			prevtime = curtime;
			if (!type)
			{
				nightswatch_dirty();
			}
			else
			{
				nightswatch_interrupt();
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