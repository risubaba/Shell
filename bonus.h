#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <readline/history.h>
#include <readline/readline.h>

extern int to_int(char* );
void history (char [1024][1024],int);
void nightswatch_dirty();
void nightswatch_interrupt_setup();
void nightswatch_interrupt();
void nightswatch (char [1024][1024],int);