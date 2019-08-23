#include "ls.h"

int setlsFlag(char argvs[1024][1024], int argc)
{
	int ret = -1;
	char lsValues[][10] = {"", "-l", "-a", "-al", "-la"};
	for (int i = 0; i < argc; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (!strcmp(argvs[i], lsValues[j]))
			{
				if ((ret == 1 && j == 2) || (ret == 2 && j == 1))
				{
					ret = 4;
					break;
				}
				else
					ret = j;
			}
		}
	}
	return ret;
}

void lsPrint(char *name)
{
	struct passwd *pw;
	struct group *gp;
	struct stat sb;

	stat(name, &sb);

	printf((S_ISDIR(sb.st_mode)) ? "d" : "-");
	printf((sb.st_mode & S_IRUSR) ? "r" : "-");
	printf((sb.st_mode & S_IWUSR) ? "w" : "-");
	printf((sb.st_mode & S_IXUSR) ? "x" : "-");
	printf((sb.st_mode & S_IRGRP) ? "r" : "-");
	printf((sb.st_mode & S_IWGRP) ? "w" : "-");
	printf((sb.st_mode & S_IXGRP) ? "x" : "-");
	printf((sb.st_mode & S_IROTH) ? "r" : "-");
	printf((sb.st_mode & S_IWOTH) ? "w" : "-");
	printf((sb.st_mode & S_IXOTH) ? "x" : "-");
	printf(" ");
	printf("%ld ", sb.st_nlink);
	pw = getpwuid(sb.st_uid);
	printf("%s ", pw->pw_name);
	gp = getgrgid(sb.st_gid);
	printf("%s ", gp->gr_name);
	printf("%5ld ", sb.st_size);
	char *c = ctime(&sb.st_mtime);
	for (int i = 4; i <= 15; i++)
		printf("%c", c[i]);
	printf(" ");
	printf("%s\n", name);
}

void ls(char argvs[1024][1024], int argc)
{
	int lsFlag = setlsFlag(argvs, argc);
	char dirname[100];
	DIR *p;
	struct dirent *d;

	if (lsFlag == -1)
	{
		if (!strcmp("", argvs[0]))
		{
			strcpy(dirname, ".");
		}
		else
		{
			adjustForTilda(argvs[0]);
			strcpy(dirname, argvs[0]);
		}
	}
	else
	{
		if (argvs[argc - 1][0] != '-')
		{
			adjustForTilda(argvs[argc - 1]);
			strcpy(dirname, argvs[argc - 1]);
		}
		else
			strcpy(dirname, ".");
	}

	p = opendir(dirname);
	if (p == NULL)
	{
		perror("Cannot find directory");
	}
	else
	{
		while (d = readdir(p))
		{
			if (lsFlag == 0)
			{
				if (d->d_name[0] != '.')
					printf("%s\n", d->d_name);
			}
			else if (lsFlag == 1)
			{
				if (d->d_name[0] != '.')
					lsPrint(d->d_name);
			}
			else if (lsFlag == 2)
			{
				printf("%s\n", d->d_name);
			}
			else if (lsFlag > 2)
			{
				lsPrint(d->d_name);
			}
			else if (lsFlag < 0)
			{
				if (d->d_name[0] != '.')
					printf("%s\n", d->d_name);
			}
		}
	}
}