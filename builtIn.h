#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PATH_MAX 4096
#define INP_MAX 1024
extern char swd[4096];
extern char inp[1024];
extern char cwd[4096];
extern void adjustForTilda(char* );
extern int parseInput(char *, char *);
extern int parseArgsForCommand(char *, char [1024][1024]);
extern void ls (char [1024][1024],int );
extern void history (char [1024][1024],int );
extern void pinfo (char [1024][1024],int );
extern void executeCommand (char *,char [1024][1024],int );
extern void nightswatch (char [1024][1024],int );
void cd (char* );
void pwd (char [1024][1024]);
void Echo (char* );
void executeInBuiltCommand();