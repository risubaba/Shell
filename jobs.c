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
	int job_counter=1;
	for (int i = 0; i < counter; i++)
	{
		if (piddd[i] <= 0)
			continue;
		char path[32];
		char *buff;
		ssize_t buffsize=0;
		sprintf(path, "/proc/%d/status", piddd[i]);
		FILE *fd = fopen(path,"r");
		if (fd==NULL){
			continue;
		}
		for (int i=0;i<3;i++)
			getline(&buff,&buffsize,fd);
		char status[1024];
		if (buff[7]=='S' || buff[7]=='R') strcpy(status,"Running");
		else if (buff[7]=='T') strcpy(status,"Stopped");
		else status[0]=buff[7],status[1]='\0';
		printf("[%d] %s %s [%d]\n",job_counter,status, jobs_command[i],piddd[i]);
		job_counter++;
	}
}

int endJob(pid_t pidinc)
{
	for (int i = 0; i < counter; i++)
		if (piddd[i] = pidinc){
			piddd[i] = -pidinc;
			return i;
		}
}


