#include "bonus.h"

int initializeHistory()
{
    char filepath[1024];
    strcpy(filepath, swd);
    strcat(filepath, "/.history");
    FILE *fd = fopen(filepath, "r+");
    if (fd == NULL)
    {
        printf("Error : History for session won't be saved\n");
        return 0;
    }
    else
        fclose(fd);
    return 1;
}

void addHistory(char *inp)
{
    char filepath[1024];
    strcpy(filepath, swd);
    strcat(filepath, "/.history");
    FILE *fd = fopen(filepath, "r+");
    if (fd == NULL)
    {
        printf("Error : History for session won't be saved\n");
        return;
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
        if (buff[0] == '\033')
        {
            lines = i;
            break;
        }
    }
    char temp[20][1024];
    if (lines == 20)
    {
        fseek(fd, 0, SEEK_SET);
        for (int i = 0; i < 20; i++)
        {
            getline(&buff, &buffsize, fd);
            if (i)
                strcpy(temp[i - 1], buff);
        }
        lines = 19;
        fseek(fd, 0, SEEK_SET);
        for (int i = 0; i < lines - 1; i++)
            fprintf(fd, "%s\n", temp[i]);
    }
    if (strcmp(buff, inp) && buff[0] != '\033')
    {
        fprintf(fd, "%s\n", inp);
    }
    fclose(fd);
    free(buff);
}

void history(char argvs[1024][1024], int argc)
{
    char *buff;
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
    strcpy(filepath, swd);
    strcat(filepath, "/.history");
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
        if (buff[0] == '\033')
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
        printf("%s", hist[i]);
        count--;
    }
}

char *recall_history(int recall_number)
{

    char filepath[1024];
    strcpy(filepath, swd);
    strcat(filepath, "/.history");
    FILE *fd = fopen(filepath, "r+");
    if (fd == NULL)
    {
        printf("Error : History for session won't be saved\n");
        return NULL;
    }
    char *temp;
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
        if (buff[0] == '\033')
        {
            lines = i;
            break;
        }
    }
    fseek(fd, 0, SEEK_SET);
    recall_number = lines - recall_number + 1 > 1 ? lines - recall_number + 1 : 1;
    for (int i = 0; i < recall_number; i++)
    {
        getline(&buff, &buffsize, fd);
        strcpy(temp, buff);
    }
    // printf("%s",temp);
    return temp;
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
        return;
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

    if (type == 1)
    {
        nightswatch_interrupt_setup();
    }
    time_t starttime = time(NULL), prevtime = time(NULL);
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

char commandsBuiltIn[][1024] = {
    "cd", "pwd", "echo", "ls", "pinfo", "history", "nightswatch", "clear", "quit", "cronjob", "jobs", "setenv", "unsetenv", "kjob"};

int checkBuiltIn(char cronCommand[1024])
{
    int i=0;
    i++;
    int len = 0;
    while (strlen(commandsBuiltIn[len])!=0) len++;
    while (strcmp(commandsBuiltIn[i],cronCommand) && i<len)
    {
        i++;
    }
    if (i==len)
        return 1;
    return 0;
}

void cronjob(char argvs[1024][1024], int argc)
{
    // cronjob -c ls -t 3 -p 6
    if (strcmp(argvs[0], "-c"))
    {
        printf("Please enter a valid argument\n");
        return;
    }
    char temp_argvs[1024][1024];
    int temp_argc;
    char temp_curCommand[1024];
    strcpy(temp_curCommand, argvs[1]);
    int i = 2, j = 0;
    for (; i < argc; i++)
    {
        if (strcmp("-t", argvs[i]))
        {
            strcpy(temp_argvs[j], argvs[i]);
            j++;
        }
        else
        {
            temp_argvs[j][0] == '\0';
            break;
        }
    }
    if (checkBuiltIn(temp_curCommand))
    {
        strcpy(temp_argvs[j], "&");
        j++;
        temp_argc = j;
    }
    // printf("%d\n", temp_argc);
    // for (int k = 0; k < j; k++)
    //     printf("%s\n", temp_argvs[k]);
    if (i == argc - 1)
    {
        printf("Please enter a valid argument\n");
        return;
    }
    int interval = to_int(argvs[i + 1]);
    if (interval < 1)
    {
        printf("Interval has to be a positive number\n");
        return;
    }
    if (strcmp(argvs[i + 2], "-p"))
    {
        printf("Please enter a valid argument\n");
        return;
    }
    int total_time = to_int(argvs[i + 3]);
    int repititions = total_time / interval;
    time_t starttime = time(NULL), prevtime = time(NULL);
    pid_t pid = fork();
    // int arr[100];
    // int i=0;
    if (pid == 0)
    {
        while (1 && repititions)
        {
            time_t curtime = time(NULL);
            if ((curtime - starttime) % interval == 0 && curtime != prevtime)
            {
                prevtime = curtime;
                commandtoExecute(0, temp_curCommand, temp_argvs, temp_argc);
                repititions--;
            }
        }
        exit(0);
    }
}
