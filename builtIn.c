#include "builtIn.h"

void cd(char *argsForCommand)
{
	if (!strcmp("", argsForCommand))
	{
		if (chdir(swd))
		{
			perror("Error ");
		}
	}
	else
	{
		adjustForTilda(argsForCommand);
		if (chdir(argsForCommand))
		{
			perror("Error ");
		}
	}
}

void pwd(char argvs[1024][1024])
{
	if (strcmp(argvs[0], ""))
	{
		printf("Too many arguments\n");
	}
	else if (getcwd(cwd, PATH_MAX) != NULL)
	{
		printf("%s\n", cwd);
	}
}

void shell_exit()
{
	printf("Are you sure you want to exit [y/N]??");
	char ex = getchar();
	if (ex == 'y' || ex == 'Y')
	{
		exit(0);
	}
}

void Echo(char argvs[1024][1024], int argc)
{
	for (int i = 0; i < argc; i++)
	{
		printf("%s ", argvs[i]);
	}
	printf("\n");
}

void Setenv(char argvs[1024][1024], int argc)
{
	if (argc < 1 || argc > 2)
	{
		printf("Incorrect number of arguments\n");
		return;
	}
	if (setenv(argvs[0], argvs[1], 1) == -1)
	{
		printf("Error in setting environment variable");
		return;
	}
}

void Unsetenv(char argvs[1024][1024], int argc)
{
	if (argc != 1)
	{
		printf("Incorrect number of arguments\n");
		return;
	}
	if (unsetenv(argvs[0]) == -1)
	{
		printf("Error in unsetting environment variable");
		return;
	}
}

char commands[][1024] = {
	"cd", "pwd", "echo", "ls", "pinfo", "history", "nightswatch", "clear", "quit", "cronjob", "jobs", "setenv", "unsetenv", "kjob"};

int commandtoExecute(int inOffset, char curCommand[1024], char argvs[1024][1024], int argc)
{
	if (!strcmp(curCommand + inOffset, commands[0]))
	{
		cd(argvs[0]);
	}
	else if (!strcmp(curCommand + inOffset, commands[1]))
	{
		pwd(argvs);
	}
	else if (!strcmp(curCommand + inOffset, commands[2]))
	{
		Echo(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[3]))
	{
		ls(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[4]))
	{
		pinfo(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[5]))
	{
		history(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[6]))
	{
		nightswatch(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[7]))
	{
		printf("\e[1;1H\e[2J");
	}
	else if (!strcmp(curCommand + inOffset, commands[8]))
	{
		shell_exit();
	}
	else if (!strcmp(curCommand + inOffset, commands[9]))
	{
		cronjob(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[10]))
	{
		jobs(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[11]))
	{
		Setenv(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[12]))
	{
		Unsetenv(argvs, argc);
	}
	else if (!strcmp(curCommand + inOffset, commands[13]))
	{
		kjob(argvs, argc);
	}
	else
	{
		executeCommand(curCommand, argvs, argc);
	}
}

void executeWithoutPipe(int inOffset, char curCommand[], char argvs[1024][1024], int argc)
{
	int original_argc = argc;
	int input_redir = 0, output_redir = 0;
	char input_file[FILENAME_MAX];
	char output_file[FILENAME_MAX];
	input_redir = checkInpRedir(argvs, &argc, input_file);
	int savestdin = dup(0);
	int savestdout = dup(1);
	if (input_redir == -1)
	{
		return;
	}
	else if (input_redir == 1)
	{
		int fd = open(input_file, O_RDONLY);
		if (fd == -1)
		{
			printf("File doesn't exist\n");
			return;
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	int input_argc = argc;
	argc = original_argc;
	output_redir = checkOutRedir(argvs, &argc, output_file);
	if (output_redir == -1)
	{
		dup2(savestdin, 0); //resetting stdin
		return;
	}
	else if (output_redir > 0)
	{
		int ffd;
		if (output_redir == 1)
			ffd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
			ffd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (ffd == -1)
		{
			printf("File doesn't exist\n");
			return;
		}
		dup2(ffd, STDOUT_FILENO);
	}

	argc = input_argc < argc ? input_argc : argc;
	commandtoExecute(inOffset, curCommand, argvs, argc);
	dup2(savestdin, 0);  //resetting stdin
	dup2(savestdout, 1); //resetting stdin
}

void executeWithPipe(int inOffset, char curCommand[1024], char argvs[1024][1024], int argc)
{
	// printf("%d\n", argc);
	int pipefd[2];
	pipe(pipefd); // pipefd[0] is read and pipefd[1] is write end for pipe
	char *curPipeCommand[1024];
	for (int i = 0, j = 0; i < argc + 1; i++)
	{
		if (i == 0)
			curPipeCommand[j] = curCommand, j++;
		else
		{
			if (strcmp(argvs[i - 1], "|"))
				curPipeCommand[j] = argvs[i - 1], curPipeCommand[j + 1] = NULL, j++;
		}
		if (!strcmp(argvs[i], "|"))
		{
			// printf("HEllo %d \n", i);
			// if (i==argc)
			// 	j++;
			// printf("Current command with j ++++ %d\n", j);
			// for (int k = 0; k < j; k++)
				// printf("%s\n", curPipeCommand[k]);
			if (i != argc - 1)
				curPipeCommand[j] = NULL;
			if (!fork())
			{
				// close(pipefd[0]);
				dup2(pipefd[1], 1);
				// close(pipefd[1]);
				execvp(curPipeCommand[0], curPipeCommand);
				// executeWithoutPipe(inOffset,curPipeCommand)
				abort();
			}
			else
			{
				dup2(pipefd[0], 0);
				close(pipefd[1]);
				// close(pipefd[0]);
			}
			j = 0;
		}
	}
	execvp(curPipeCommand[0], curPipeCommand);
}

void executeInBuiltCommand()
{

	char curCommand[INP_MAX];
	for (int i = 0, k = 0; i <= strlen(inp); i++)
	{
		if (inp[i] != ';' && i < strlen(inp) && inp[i] != '\0')
			curCommand[k++] = inp[i];
		else
		{
			curCommand[k] = '\0';
			if (curCommand[k - 1] == '\n')
				curCommand[k - 1] = '\0';
			char argsForCommand[INP_MAX];
			int inOffset = parseInput(curCommand, argsForCommand);
			char argvs[1024][1024];
			int argc = parseArgsForCommand(argsForCommand, argvs);
			int pipe_present = checkPipe(argvs, &argc);
			if (pipe_present > 0)
			{
				executeWithPipe(inOffset, curCommand, argvs, argc);
			}
			else
				executeWithoutPipe(inOffset, curCommand, argvs, argc);

			k = 0;
		}
	}
}