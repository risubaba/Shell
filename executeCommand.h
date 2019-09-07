#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <curses.h>
#include <stdio.h>
#include <string.h>

extern char swd[4096];
void executeCommand(char *, char[1024][1024], int);
void startJob(char *[1024], int, int);