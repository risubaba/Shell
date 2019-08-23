#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


extern void adjustForTilda(char *);
int setlsFlag(char [1024][1024],int );
void lsPrint(char * );
void ls(char [1024][1024],int );