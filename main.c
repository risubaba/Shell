#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>

#define INP_MAX 100

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];

//set error messages wherever required
//add colour to printf
//add exit message for ^D

char* directorySet(char* cwd,char* swd){

	for (int i=0,k=0;i<PATH_MAX;i++){
		if (swd[i]=='\0'){

			ret[k++]='~';
			for (int j=i;j<PATH_MAX;j++){
				ret[k++]=cwd[i++];
			}
			return ret;
		
		} else if (cwd[i]=='\0'){
		
			return cwd;
		
		}
	}

}

void printSystemName(){

	char *uName;
	uName = getlogin();
	char sName[1024];
	gethostname(sName,1024);
	if (getcwd(cwd,PATH_MAX)!=NULL){
		char *directory;
		directory = directorySet(cwd,swd);
		printf("<%s@%s:%s>",uName,sName,directory);
	}

}

int setlsFlag(char argvs[100][100],int argc){
	int ret=-1;
	char lsValues[][10]={"","-l","-a","-al","-la"};
	for (int i=0;i<argc;i++){
		for (int j=0;j<5;j++){
			if (!strcmp(argvs[i],lsValues[j])){
				if ((ret==1 && j==2) || (ret==2 && j==1)){
					ret=4;
					break;
				}
				else
					ret=j;
			}
		}
	}
	return ret;
}

void lsPrint(char* name){
	struct passwd *pw;
	struct group *gp;
	struct stat sb;

	stat(name,&sb);

	printf( (S_ISDIR(sb.st_mode))  ? "d" : "-");
    printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb.st_mode & S_IROTH) ? "r" : "-");
    printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
	printf(" ");
    printf("%ld ",sb.st_nlink);
    pw=getpwuid(sb.st_uid);
    printf("%s ",pw->pw_name);
    gp=getgrgid(sb.st_gid);
    printf("%s ",gp->gr_name);
    printf("%5ld ",sb.st_size);
    char* c=ctime(&sb.st_mtime);
    for(int i=4;i<=15;i++)
      printf("%c",c[i]);
    printf(" ");
    printf("%s\n",name);
}

void ls(char argvs[100][100],int argc){
	int lsFlag=setlsFlag(argvs,argc);
	char dirname[100];
	DIR*p;
	struct dirent *d;
	if (lsFlag==-1)
		strcpy(dirname,argvs[0]);
	else
		strcpy(dirname,".");
	p=opendir(dirname);
	if(p==NULL){
		perror("Cannot find directory");
	} else {	
		while(d=readdir(p)){
			if (lsFlag==0){
				if (d->d_name[0]!='.')
				printf("%s\n",d->d_name);
			}
			else if (lsFlag==1){
				if (d->d_name[0]!='.')
					lsPrint(d->d_name);
			} else if (lsFlag==2){
				printf("%s\n",d->d_name);
			} else if (lsFlag>2) {
				lsPrint(d->d_name);
			} else if (lsFlag < 0) {
				if (d->d_name[0]!='.')
					printf("%s\n",d->d_name);
			}
		}
	}
}

int parseInput(char* curCommand,char* argsForCommand){
	int inOffset=0,k=strlen(curCommand);

	while (curCommand[inOffset]==' ') inOffset++;
	int argsOffset=inOffset;
	while (curCommand[argsOffset]!=' ' && curCommand[argsOffset]!='\0') argsOffset++;
	curCommand[argsOffset++]='\0';
	while (curCommand[argsOffset]==' ') argsOffset++;
	int l=0;
	for (int j=argsOffset;j<=k;j++,l++){
		argsForCommand[l]=curCommand[j];
	}
	argsForCommand[l]='\0';

	return inOffset;
}

int parseArgsForCommand(char* argsForCommand,char argvs[100][100]){
	int k=0,j=0;
	for (int i=0;i<strlen(argsForCommand);i++){
		if (argsForCommand[i]!=' ' || argsForCommand[i]=='\0'){
			argvs[k][j]=argsForCommand[i],j++;
		} else {
			argvs[k][j]='\0',k++,j=0;
		}
	}
	argvs[k][j]='\0';
	if (k || j)
		k++;
	argvs[k][0] = '\0';
	return k;
}

char* adjustForTilda(char* argsForCommand){
	int n=strlen(argsForCommand);
	if (argsForCommand[0]!='~')
		return argsForCommand;
	else {
		char* ret;
		strcpy(ret,swd);
		strcat(ret,"/");	
		strcat(ret,argsForCommand+2);
		return ret;
	}
}

void cd(char* argsForCommand){
	argsForCommand = adjustForTilda(argsForCommand);
	if (!strcmp("~",argsForCommand) || !strcmp("",argsForCommand)){
		if (chdir(swd)){
			perror("Error ");
		}
	}
	else if (chdir(argsForCommand)){
		perror("Error ");
	}
}

void pwd(char argvs[100][100]){
	printf("%s",argvs[0]);
	if (strcmp(argvs[0],"")){
		printf("Too many arguments\n");
	}
	else if (getcwd(cwd,PATH_MAX)!=NULL){
		printf("%s\n",cwd);
	}
}

void echo(char* argsForCommand){
	if (argsForCommand[0]=='\"'){
		for (int j=1;j<strlen(argsForCommand);j++){
			if (argsForCommand[j]!='\"')
				printf("%c",argsForCommand[j]);
			else{
				printf("\n");
				break;
			}
		}

	} else if (argsForCommand[0]=='\''){
		for (int j=1;j<strlen(argsForCommand);j++){
			if (argsForCommand[j]!='\'')
					printf("%c",argsForCommand[j]);
			else{
				printf("\n");
				break;
			}
		}
	} else {
		for (int j=0;j<strlen(argsForCommand);j++){
			if (argsForCommand[j]!=' ')
					printf("%c",argsForCommand[j]);
				else{
					printf("\n");
					break;
				}
		}
		if (argsForCommand[strlen(argsForCommand)-1]!=' ')
			printf("\n");

	}
}



int to_int(char* num){
	int ret = 0;
	for (int i=0;i<strlen(num);i++)
		ret = ret * 10 + num[i]-'0';
	return ret;
}

void printPinfoLocation(pid_t pid){

	char buff[100];
	char path[32];
	sprintf(path, "/proc/%d/exe", pid);
	readlink(path,buff,100);	
	char *location ;
	location = directorySet(buff,swd);
	for (int i=strlen(location)-1;i>-1;i--)
		if (location[i]>'z' || location[i]<'A'){
			if (location[i]!='~' && location[i]!='/' && location[i]!='.'){
				location[i]='\0';
				break;
			}
		}
	printf("Executable Path -- %s\n",location);

}

void printPinfoState(pid_t pid){

	char* buff;
	size_t buffsize = 0;
	char path[32];
	sprintf(path, "/proc/%d/status", pid);
	FILE* fd = fopen(path,"r");
	for (int i=0;i<3;i++)
		getline(&buff,&buffsize,fd);
	printf("Process Status -- %c\n",buff[7]);

}

int checkPid(pid_t pid){
	char path[32];
	sprintf(path, "/proc/%d/status", pid);
	int fd = open(path,O_RDONLY);
	if (fd == -1 )
		return 0;
	return 1;
}

void pinfo(char argvs[100][100],int argc){
	pid_t pid;
	if (argc==0)
		pid = getpid();
	else 
		pid = to_int(argvs[0]);	
	
	if (!checkPid(pid))
		return;
	printf("pid -- %d\n",pid);
	printPinfoState(pid);
	printPinfoLocation(pid);
}

void executeCommand(char* curCommand,char argvs[100][100],int argc){
	printf("%s\n\n",curCommand);
	for (int i=0;i<argc;i++)
		if (argvs[i][0]=='\0')
			printf("lylli\n");
		else
			printf("%s\n",argvs[i]);

}

void executeInBuiltCommand(){

	char curCommand[INP_MAX];
	for (int i=0,k=0;i<=strlen(inp);i++){
		if (inp[i]!=';' && i<strlen(inp) && inp[i]!='\0')
			curCommand[k++]=inp[i];
		else {
			curCommand[k]='\0';
			if (curCommand[k-1]=='\n') curCommand[k-1]='\0';
			char argsForCommand[INP_MAX];
			int inOffset=parseInput(curCommand,argsForCommand);
			char argvs[100][100];
			int argc=parseArgsForCommand(argsForCommand,argvs);
			if (!strcmp(curCommand+inOffset,"cd")){
				cd(argvs[0]);
			} else if (!strcmp(curCommand+inOffset,"pwd")){
				pwd(argvs);
			} else if (!strcmp(curCommand+inOffset,"echo")){
				echo(argsForCommand);
			} else if (!strcmp(curCommand+inOffset,"ls")){
				ls(argvs,argc);
			} else if (!strcmp(curCommand+inOffset,"pinfo")){
				pinfo(argvs,argc);
			} else {
				executeCommand(curCommand,argvs,argc);
			}
			k=0;
		}
	}

}

int main(){
	getcwd(swd,PATH_MAX);

	while (1){
		printSystemName();
		// printf("\n%s\n",swd);
		fgets(inp,INP_MAX,stdin);
		executeInBuiltCommand();	
	}

}