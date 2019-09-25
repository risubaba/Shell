#include "jobs.h"
int piddd[100];
char jobs_command[100][4096];

void startJob(char *new_argvs[1024], int argc, pid_t pidinc)
{
	;
}

void printJobs()
{
	int counter=0;
	while (piddd[counter]!=-1)
		counter++;
	printf("%d\n",counter);
	for (int i = 0; i < counter; i++)
	{
			printf("%d %s\n", piddd[i], jobs_command[i]);
	}
}

void endJob(pid_t pidinc){
	int counter=0;
	// while (piddd[counter]!=0)
	// 	counter++;
	// for (int i=0;i<counter;i++)
	// if (piddd[i]=pidinc) piddd[i]=0;
}
