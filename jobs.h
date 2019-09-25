#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>


extern char swd[4096];
extern int to_int(char *);
void startJob(char *[1024], int, int);
void printJobs();
void endJob(pid_t);

extern int pid[100];
char jobs_command[100][4096];