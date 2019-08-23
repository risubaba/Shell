#include "bonus.h"

void history(char argvs[1024][1024], int argc)
{
    int len_required = 100;
    if (argc == 1)
    {
        len_required = to_int(argvs[0]);
    }

    HISTORY_STATE *myhist = history_get_history_state();

    HIST_ENTRY **mylist = history_list();

    int count = myhist->length > len_required ? len_required : myhist->length;
    for (int i = myhist->length - 1; i > -1 && count > 0; i--)
    {
        if (i != myhist->length - 1)
        {
            if (strcmp(mylist[i]->line, mylist[i + 1]->line))
            {
                printf("%s\n", mylist[i]->line);
                count--;
            }
        }
        else
        {
            printf("%s\n", mylist[i]->line);
            count--;
        }
    }
    putchar('\n');
}

void nightswatch_dirty()
{
    char *buff;
    size_t buffsize = 0;
    char path[32] = "/proc/meminfo\0";
    FILE *fd = fopen(path, "r");
    for (int i = 0; i < 17; i++)
        getline(&buff, &buffsize, fd);
    printf("%s\n", buff);
}

void nightswatch_interrupt_setup()
{
    char *buff;
    size_t buffsize = 0;
    char path[32] = "/proc/interrupts\0";
    FILE *fd = fopen(path, "r");
    getline(&buff, &buffsize, fd);
    printf("%s\n", buff);
}

void nightswatch_interrupt()
{
    char *buff;
    size_t buffsize = 0;
    char path[32] = "/proc/interrupts\0";
    FILE *fd = fopen(path, "r");
    for (int i = 0; i < 3; i++)
        getline(&buff, &buffsize, fd);
    printf("%s\n", buff);
}

void nightswatch(char argvs[1024][1024], int argc)
{
    int ttime = to_int(argvs[1]);
    if (ttime == 0)
    {
        //error handling
    }
    int type;
    if (!strcmp(argvs[2], "interrupt"))
    {
        type = 1;
    }
    else if (!strcmp(argvs[2], "dirty"))
    {
        type = 0;
    }
    else
    {
        //error handling
    }

    time_t starttime = time(NULL), prevtime = time(NULL);
    if (type == 1)
    {
        nightswatch_interrupt_setup();
    }
    while (1)
    {
        time_t curtime = time(NULL);
        if ((curtime - starttime) % ttime == 0 && curtime != prevtime)
        {
            prevtime = curtime;
            if (!type)
            {
                nightswatch_dirty();
            }
            else
            {
                nightswatch_interrupt();
            }
        }
    }
}