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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

extern char swd[4096];
extern char jobs_command[100][4096];
extern int piddd[100];
extern int to_int(char *);
void executeCommand(char *, char[1024][1024], int);
extern void startJob(char *[1024], int, int);
extern int endJob(pid_t );
int fg_process_pid;
