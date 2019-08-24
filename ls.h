#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define RESET "\x1B[0m"
#define KGRN  "\x1B[32m"
#define KBLU  "\x1B[34m"
#define GRN   "\x1B[32m"


extern void adjustForTilda(char *);
int setlsFlag(char [1024][1024],int );
void lsPrint(char * ,int );
void ls(char [1024][1024],int );