#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

extern char swd[4096];
extern char* directorySet(char* , char *);
extern int to_int(char *);
void printPinfoLocation(pid_t );
void printPinfoStateandMemory(pid_t );
int checkPid(pid_t );
void pinfo(char [1024][1024],int );