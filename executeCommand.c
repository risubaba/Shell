#include "executeCommand.h"

void executeCommand(char *curCommand, char argvs[1024][1024], int argc)
{
	char *new_argvs[1024];
	for (int i = 0; i < argc + 1; i++)
	{
		if (!i)
			new_argvs[i] = curCommand;
		else
			new_argvs[i] = argvs[i - 1];
	}
	new_argvs[argc + 1] = NULL;
	pid_t pid = fork();
	if (pid < 0)
	{
		printf("Not able to fork properly\n");
		return;
	}
	else if (pid == 0)
	{
		if (new_argvs[argc][0] == '&' && strlen(new_argvs[argc]) == 1)
		{
			//change to running in job file
			new_argvs[argc][0] = '\0';
			pid_t pid_child = fork();
			if (pid_child < 0)
			{
				printf("Not able to fork properly\n");
				return;
			}
			else if (pid_child == 0)
			{
				if (execvp(new_argvs[0], new_argvs) < 0)
				{
					//change to stopped in job file
					printf("Error in executing command\n");
					return;
				}
			}
			else
			{
				int status = 0;
				waitpid(pid_child, &status, 0);
				int currpid = getpid();
				endJob(new_argvs, argc, currpid);
				//change to stopped in job file
				if (!status)
				{
					printf("\nCommand %s with pid %d exited normally\n", new_argvs[0], currpid);
				}
				else
				{
					printf("\nCommand %s with pid %d exited abnormally\n", new_argvs[0], currpid);
				}
			}
		}
		else if (execvp(new_argvs[0], new_argvs) < 0)
		{
			printf("Error in executing command\n");
			return;
		}
	}
	else
	{
		if (new_argvs[argc][0] == '&' && strlen(new_argvs[argc]) == 1)
		{
			new_argvs[argc][0] = '\0';

			printf("Process started with pid %d\n", pid);
			startJob(new_argvs, argc, pid);
		}
		else
		{
			int status = 0;
			waitpid(pid, &status, 0);
			// if (!status)
			// {
			// 	printf("\nCommand %s with pid %d exited normally\n", new_argvs[0], pid);
			// }
			// else
			// {
			// 	printf("\nCommand %s with pid %d exited abnormally\n", new_argvs[0], pid);
			// }
		}
	}
}