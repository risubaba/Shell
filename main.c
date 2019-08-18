#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#define INP_MAX 100

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];

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

void executeInBuiltCommand(){

	// printf("%s",inp);
	char curCommand[INP_MAX];
	for (int i=0,k=0;i<=strlen(inp);i++){
		if (inp[i]!=';' && i<strlen(inp))
			curCommand[k++]=inp[i];
		else {
			// adjust for blank spaces at start
			if (curCommand[0]=='c' && curCommand[1]=='d'){
				printf("cd\n");
			} else if (curCommand[0]=='p' && curCommand[1]=='w' && curCommand[2]=='d'){
				printf("pwd\n");

			} else if (curCommand[0]=='e' && curCommand[1]=='c' && curCommand[2]=='h' && curCommand[3]=='o'){
				printf("echo\n");

			} else {

			}
			k=0;
		}
	}

}


int main(){
	getcwd(swd,PATH_MAX);

	while (1){
		printSystemName();
		fgets(inp,INP_MAX,stdin);
		executeInBuiltCommand();	
	}
}