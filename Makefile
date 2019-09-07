LDFLAGS=-lncurses -lreadline

a.out: main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o jobs.o ExtraFiles
		gcc main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o jobs.o -o a.out -lncurses -lreadline -g

main.o: main.c
		gcc -c main.c

bonus.o: bonus.h bonus.c
	gcc -c bonus.c -o bonus.o

pinfo.o: pinfo.h pinfo.c
	gcc -c pinfo.c -o pinfo.o

ls.o: ls.h ls.c
	gcc -c ls.c -o ls.o

builtIn.o: builtIn.h builtIn.c
	gcc -c builtIn.c -o builtIn.o

executeCommand.o: executeCommand.h executeCommand.c
	gcc -c executeCommand.c -o executeCommand.o

jobs.o: jobs.h jobs.c
	gcc -c jobs.c -o jobs.o

ExtraFiles: 
	touch .history .jobs

clean:
		rm -rf main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o jobs.o .history .jobs a.out