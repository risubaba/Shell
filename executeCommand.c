#include "executeCommand.h"
int fg_process_pid = 0;

void process_handler(int sig)
{
	int status;
	int pid = waitpid(-1, &status, WNOHANG);
	int temp = endJob(pid);
	if (WIFEXITED(status) && pid != -1)
	{
		if (temp > -1)
		{
			if (WEXITSTATUS(status) != 98)
			{
				fprintf(stderr, "\n%s with pid %d exited ", jobs_command[temp], -piddd[temp]);
				if (WEXITSTATUS(status) == 0)
					fprintf(stderr, "normally\n");
				else
					fprintf(stderr, "with status: %d\n", WEXITSTATUS(status));
			}
		}
	}
	if (WIFSIGNALED(status) && pid != -1)
	{
		if (temp > -1)
		{
			fprintf(stderr, "\n%s with pid %d terminated due to the signal %d", jobs_command[temp], -piddd[temp], WTERMSIG(status));
		}
	}
}

void executeCommand(char *curCommand, char argvs[1024][1024], int argc)
{
	fg_process_pid = 0;
	char *new_argvs[1024];
	for (int i = 0; i < argc + 1; i++)
	{
		if (!i)
			new_argvs[i] = curCommand;
		else
			new_argvs[i] = argvs[i - 1];
	}
	new_argvs[argc + 1] = NULL;
	int background = 0;
	if (!strcmp(new_argvs[argc], "&") && strlen(new_argvs[argc]) == 1)
		background = 1, new_argvs[argc] = '\0';

	if (background)
		signal(SIGCHLD, process_handler);
	else
		signal(SIGCHLD, SIG_IGN);

	int pid = fork();
	if (pid == -1)
	{
		perror("");
		return;
	}
	else if (pid == 0)
	{
		setpgid(0, 0);
		execvp(new_argvs[0], new_argvs);
		exit(98);
	}
	else
	{
		if (background)
			startJob(new_argvs, argc, pid);
		else
		{
			fg_process_pid = pid;
			int status;
			waitpid(pid, &status, WUNTRACED);
			if (WIFSTOPPED(status))
			{
				printf("am here\n");
				startJob(new_argvs, argc, pid);
			}
			fg_process_pid = 0;
			signal(SIGCHLD, process_handler);
		}
	}
}