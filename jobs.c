#include "jobs.h"

void startJob(char *new_argvs[1024], int argc, pid_t pid)
{
	char filepath[1024];
	strcpy(filepath, swd);
	strcat(filepath, "/.jobs");
	FILE *fd = fopen(filepath, "r+");
	if (fd == NULL)
	{
		printf("Error : Job Status for session won't be saved\n");
		return;
	}
	char *buff;
	size_t buffsize = 0;
	int lines = 0;
	for (int i = 0;; i++)
	{
		if (getline(&buff, &buffsize, fd) == -1)
		{
			lines = i;
			break;
		}
		if (!strcmp("", buff))
		{
			lines = i;
			break;
		}
	}
	char temp[1024];
	temp[0] = '\0';
	for (int i = 0; i < argc; i++)
	{
		strcat(temp, new_argvs[i]);
		strcat(temp, " ");
	}
	fprintf(fd, "[%d] Running %s[%d]\n", lines + 1, temp, pid);
	fclose(fd);
}

void endJob(char *new_argvs[1024], int argc, pid_t pid)
{
	char filepath[1024];
	strcpy(filepath, swd);
	strcat(filepath, "/.jobs");
	FILE *fd = fopen(filepath, "r+");
	if (fd == NULL)
	{
		printf("Error : Job Status for session won't be saved\n");
		return;
	}
	char *buff;
	size_t buffsize = 0;
	int lines = 0;
	for (int j = 0;; j++)
	{
		// fd_dup=fd;
		int bytes_read;
		bytes_read = getline(&buff, &buffsize, fd);
		if (bytes_read != -1)
		{
			char temp[1024];
			strcpy(temp, buff);
			for (int i = strlen(buff); i > -1; i--)
				if (buff[i] == '[')
				{
					buff = buff + i + 1;
					break;
				}
			for (int i = 0;; i++)
				if (buff[i] == ']')
				{
					buff[i] = '\0';
					break;
				}
			if (to_int(buff) == pid)
			{
				for (int i = 0;; i++)
				{
					if (temp[i] == ']')
					{
						strcpy(temp + i + 2, "Stopped");
						temp[i + 2 + 7] = ' ';
						break;
					}
				}
				fseek(fd, -bytes_read, SEEK_CUR);
				fprintf(fd, "%s", temp);
				break;
			}
		}
		else
		{
			break;
		}
	}
	fclose(fd);
}

void jobs(char argvs[1024][1024], int argc)
{
	char filepath[1024];
	strcpy(filepath, swd);
	strcat(filepath, "/.jobs");
	FILE *fd = fopen(filepath, "r+");
	if (fd == NULL)
	{
		printf("Error : Job Status for session won't be saved\n");
		return;
	}
	char *buff;
	size_t buffsize = 0;
	int lines = 0;
	while (getline(&buff, &buffsize, fd) != -1)
	{
		printf("%s", buff);
	}
}

void kjob(char argvs[1024][1024], int argc)
{
	// kjob <jobNumber> <signalNumber>

	char filepath[1024];
	strcpy(filepath, swd);
	strcat(filepath, "/.jobs");
	FILE *fd = fopen(filepath, "r+");
	if (fd == NULL)
	{
		printf("Error : Job Status for session won't be saved\n");
		return;
	}
	if (argc != 2)
	{
		printf("Invalid number of arguments\n");
		return;
	}
	int job_given = to_int(argvs[0]);
	int signal = to_int(argvs[1]);
	char *buff;
	size_t buffsize = 0;
	int lines = 0;
	pid_t pid_given = 0;
	while (getline(&buff, &buffsize, fd) != -1)
	{
		int pos = 0;
		while (buff[pos] != '[')
			pos++;
		buff += pos + 1;
		pos = 0;
		while (buff[pos] != ']')
			pos++;
		buff[pos] = '\0';
		// printf("JOB NUMBER ---- %s\n", buff);
		if (job_given != to_int(buff))
			continue;
		buff[pos] = ' ';
		while (buff[pos] != '[')
			pos++;
		buff += pos + 1;
		pos = 0;
		while (buff[pos] != ']')
			pos++;
		buff[pos] = '\0';
		// printf("PID ---- %s\n", buff);

		pid_given = to_int(buff);
	}
	if (kill(pid_given, signal) == -1)
	{
		printf("Not able to send signal\n");
		return;
	}
	// printf("%d\n", pid_given);
}