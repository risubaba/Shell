#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

extern char swd[1024];
extern int to_int(char *);
void startJob(char *[1024], int, int);
void endJob(char *[1024], int, int);
void jobs(char[1024][1024],int);