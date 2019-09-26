#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char swd[4096];
extern int to_int(char *);
extern char jobs_command[100][4096];
extern int piddd[100];
void startJob(char * [1024], int, int);
void printJobs();
void initializeJobs();
int endJob(pid_t);
void kjob(char[1024][1024], int);
void fg(char[1024][1024], int);
void bg(char[1024][1024], int);
extern int fg_process_pid;
extern void process_handler(int);
