#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>

extern char swd[4096];
extern int to_int(char *);
void executeCommand(char *, char[1024][1024], int);
extern void startJob(char *[1024], int, int);
extern void endJob(pid_t );
int fg_process_pid;
