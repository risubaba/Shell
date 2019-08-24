#include "pinfo.h"

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
	printf("Process Status -- %c%c\n", buff[7],buff[8]);
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