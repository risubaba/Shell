#include "jobs.h"

int counter;

void initializeJobs()
{
	counter = 0;
}
void startJob(char *new_argvs[1024], int argc, pid_t pidinc)
{
	piddd[counter] = pidinc;
	char temp_command[1024];
	temp_command[0] = '\0';
	strcpy(temp_command, new_argvs[0]), strcat(temp_command, " ");
	for (int i = 1; i < argc; i++)
		strcat(temp_command, new_argvs[i]), strcat(temp_command, " ");
	strcpy(jobs_command[counter], temp_command);
	counter++;
	// printf("%s %d\n",jobs_command[counter],pidinc);
}

void printJobs()
{
	int job_counter = 1;
	for (int i = 0; i < counter; i++)
	{
		if (piddd[i] <= 0)
			continue;
		char path[32];
		char *buff;
		ssize_t buffsize = 0;
		sprintf(path, "/proc/%d/status", piddd[i]);
		FILE *fd = fopen(path, "r");
		if (fd == NULL)
		{
			continue;
		}
		for (int i = 0; i < 3; i++)
			getline(&buff, &buffsize, fd);
		char status[1024];
		if (buff[7] == 'S' || buff[7] == 'R')
			strcpy(status, "Running");
		else if (buff[7] == 'T')
			strcpy(status, "Stopped");
		else
			status[0] = buff[7], status[1] = '\0';
		printf("[%d] %s %s [%d]\n", job_counter, status, jobs_command[i], piddd[i]);
		job_counter++;
	}
}

int endJob(pid_t pidinc)
{
	for (int i = 0; i < counter; i++)
		if (piddd[i] = pidinc)
		{
			piddd[i] = -pidinc;
			return i;
		}
}

void kjob(char argvs[1024][1024], int argc)
{
	if (argc != 2)
	{
		printf("Invalid number of arguments for kjob");
		return;
	}
	int index = -1, temp_counter;
	int job_number = to_int(argvs[0]);
	int signal_number = to_int(argvs[1]);
	if (job_number < 0 || signal_number < 0)
	{
		printf("Job Number and Signal Number have to be positive\n");
		return;
	}
	for (int i = 0; i < counter; i++)
	{
		if (piddd[i] <= 0)
			continue;
		temp_counter++;
		if (temp_counter == job_number)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		printf("Could not find job with given job number\n");
		return;
	}
	if (signal_number == 9)
	{
		endJob(piddd[index]);
	}
	if (kill(piddd[index], signal_number) == -1)
	{
		printf("Error in killing job with pid %d", piddd[index]);
		return;
	}
}

void fg(char argvs[1024][1024], int argc)
{
	if (argc != 1)
	{
		printf("Invalid number of arguments for fg");
		return;
	}
	int job_number = to_int(argvs[0]);
	if (job_number < 0)
	{
		printf("Job Number has to be positive\n");
		return;
	}
	int index = -1, temp_counter = 0;
	for (int i = 0; i < counter; i++)
	{
		if (piddd[i] <= 0)
			continue;
		temp_counter++;
		if (temp_counter == job_number)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		printf("Could not find job with given job number\n");
		return;
	}
	signal(SIGCHLD, SIG_IGN);
	kill(piddd[index], SIGCONT);
	fg_process_pid = piddd[index];
	endJob(piddd[index]);
	int status;
	waitpid(piddd[index], &status, WUNTRACED);
	fg_process_pid = 0;
	if (WIFSTOPPED(status))
	{
		strcpy(jobs_command[counter], jobs_command[index]);
		piddd[counter] = piddd[index];
		counter++;
	}
	signal(SIGCHLD, process_handler);
}

void bg(char argvs[1024][1024], int argc)
{
	if (argc != 1)
	{
		printf("Invalid number of arguments for bg");
		return;
	}
	int job_number = to_int(argvs[0]);
	if (job_number < 0)
	{
		printf("Job Number has to be positive\n");
		return;
	}
	int index = -1, temp_counter;
	for (int i = 0; i < counter; i++)
	{
		if (piddd[i] <= 0)
			continue;
		temp_counter++;
		if (temp_counter == job_number)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		printf("Could not find job with given job number\n");
		return;
	}
	kill(piddd[index],SIGCONT);
}

void overkill()
{
	for (int i = 0; i < counter; i++)
	{
		if (piddd <= 0)
			continue;
		kill(piddd[i], 9);
	}
}
