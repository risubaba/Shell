#include "bonus.h"

int initializeHistory()
{
    char filepath[1024];
    strcpy(filepath,swd);
    strcat(filepath,"/.history");
    FILE *fd = fopen(filepath, "r+");
    if (fd == NULL)
    {
        printf("Error : History for session won't be saved\n");
        return 0;
    }
    fclose(fd);
    return 1;
}

void addHistory(char *inp)
{
    char filepath[1024];
    strcpy(filepath,swd);
    strcat(filepath,"/.history");
    FILE *fd = fopen(filepath, "r+");
    if (fd==NULL){
        printf("Error : History for session won't be saved\n");
        return ;
    }
    char *buff;
    size_t buffsize = 0;
    int lines = 0;
    for (int i = 0;; i++)
    {
        if (getline(&buff, &buffsize, fd) == -1)
        {
            lines = i;
            break;
        }
        if (!strcmp("", buff))
        {
            lines = i;
            break;
        }
    }
    if (lines == 20)
    {
        fseek(fd, 0, SEEK_SET);
        char temp[20][1024];
        for (int i = 0; i < 20; i++)
        {
            getline(&buff, &buffsize, fd);
            if (i)
                strcpy(temp[i - 1], buff);
        }
        lines = 19;
    }
    fseek(fd, 0, SEEK_SET);
    while ((lines)--)
        getline(&buff, &buffsize, fd);
    if (strcmp(buff, inp))
    {
        fprintf(fd, "%s", inp);
    }
    fclose(fd);
}

void history(char argvs[1024][1024], int argc)
{
    char* buff;
    size_t buffsize = 0;
    int len_required = 10;
    if (argc == 1)
    {
        len_required = to_int(argvs[0]);
    }

    if (len_required > 20)
    {
        printf("Max history size is 20\n");
        return;
    }
    int lines = 0;
    char filepath[1024];
    strcpy(filepath,swd);
    strcat(filepath,"/.history");
    FILE *fd = fopen(filepath, "r+");
    for (int i = 0;; i++)
    {
        if (getline(&buff, &buffsize, fd) == -1)
        {
            lines = i;
            break;
        }
        if (!strcmp("", buff))
        {
            lines = i;
            break;
        }
    }
    fseek(fd, 0, SEEK_SET);
    char hist[20][1024];
    for (int i = 0; i < lines; i++)
    {
        getline(&buff, &buffsize, fd);
        strcpy(hist[i], buff);
    }
    int count = lines > len_required ? len_required : lines;
    for (int i = lines; i > -1 && count > -1; i--)
    {
        printf("%s",hist[i]);
        count--;
    }
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
        printf("Time interval has to be a positive number\n");
        return ;
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
        printf("Please enter a valid argument\n");
        return;
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