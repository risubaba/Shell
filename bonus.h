#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#define INP_MAX 1024

extern int to_int(char *);
extern char swd[4096];
extern char inp[INP_MAX];
int initializeHistory();
void addHistory(char *);
char *recall_history(int);
void history(char[1024][1024], int);
void nightswatch_dirty();
void nightswatch_interrupt_setup();
void nightswatch_interrupt();
void nightswatch(char[1024][1024], int);
void cronjob(char[1024][1024], int);
extern int commandtoExecute(int,char[1024],char[1024][1024],int);