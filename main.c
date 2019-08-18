#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>

#define INP_MAX 100

char cwd[PATH_MAX];
char swd[PATH_MAX];
char ret[PATH_MAX];
char inp[INP_MAX];

//set error messages wherever required
//use strcmp in executeInBuiltCommand


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

int setlsFlag(char* argsForCommand){
	char arggs[10][10];
	int k=0,j=0;
	for (int i=0;i<strlen(argsForCommand);i++){
		if (argsForCommand[i]!=' '){
			arggs[k][j]=argsForCommand[i],j++;
		} else {
			arggs[k][j]='\0',k++,j=0;
		}
	}
	arggs[k][j]='\0';
	k++;
	int ret=-1;
	char lsValues[][10]={"","-l","-a","-al","-la"};
	for (int i=0;i<k;i++){
		for (int j=0;j<5;j++){
			// printf("(%d)\n",strcmp(arggs[i],lsValues[j]));
			if (!strcmp(arggs[i],lsValues[j]))
				ret=j;
		}
	}
	return ret;
}

void executeCommand(char* curCommand,char* argsForCommand){
	if (strcmp(curCommand,"ls\n") || strcmp(curCommand,"ls")){
		int lsFlag=setlsFlag(argsForCommand);
		char dirname[100];
		DIR*p;
		struct dirent *d;
		if (lsFlag==-1)
			strcpy(dirname,argsForCommand);
		else
			strcpy(dirname,".");

		p=opendir(dirname);
		if(p==NULL)
		  {
		  perror("Cannot find directory");
		  }
		while(d=readdir(p)){
			if (lsFlag==0){
				if (d->d_name[0]!='.')
				  printf("%s %d\n",d->d_name,(d->d_type==DT_DIR));
			}
			else if (lsFlag==1){
					//implement ls -l
			} else if (lsFlag==2){
				printf("%s %d\n",d->d_name,(d->d_type==DT_DIR));
			} else if (lsFlag>2) {
				// implemnet ls -la
			} else if (lsFlag < 0) {
				if (d->d_name[0]!='.')
					printf("%s %d\n",d->d_name,(d->d_type==DT_DIR));
			}
		}
  	}
}

void executeInBuiltCommand(){

	char curCommand[INP_MAX];
	for (int i=0,k=0;i<=strlen(inp);i++){
		if (inp[i]!=';' && i<strlen(inp) && inp[i]!='\0')
			curCommand[k++]=inp[i];
		else {
			// adjust for blank spaces at start
			curCommand[k]='\0';
			if (curCommand[k-1]=='\n') curCommand[k-1]='\0';
			int inOffset=0;
			while (curCommand[inOffset]==' ') inOffset++;
			int argsOffset=inOffset;
			while (curCommand[argsOffset]!=' ' && curCommand[argsOffset]!='\0') argsOffset++;
			while (curCommand[argsOffset]==' ') argsOffset++;
			char argsForCommand[INP_MAX];

			for (int j=argsOffset,l=0;j<=k;j++,l++){
				argsForCommand[l]=curCommand[j];
			}
			if (curCommand[0+inOffset]=='c' && curCommand[1+inOffset]=='d'  && (curCommand[2+inOffset]==' ' || curCommand[2+inOffset]=='\0') ){

				if (chdir(argsForCommand)){
					perror("Error ");
				}

			} else if (curCommand[0+inOffset]=='p' && curCommand[1+inOffset]=='w' && curCommand[2+inOffset]=='d' && (curCommand[3+inOffset]==' ' || curCommand[3+inOffset]=='\0')){

				if (k-argsOffset!=1){
					printf("Too many arguments\n");

				}
				else if (getcwd(cwd,PATH_MAX)!=NULL){
					printf("%s\n",cwd);
				}

			} else if (curCommand[0+inOffset]=='e' && curCommand[1+inOffset]=='c' && curCommand[2+inOffset]=='h' && curCommand[3+inOffset]=='o' && (curCommand[4+inOffset]==' ' || curCommand[4+inOffset]=='\0')){

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
			} else {
				executeCommand(curCommand,argsForCommand);
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