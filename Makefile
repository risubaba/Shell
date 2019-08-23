LDFLAGS=-lncurses -readline

a.out: main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o
		gcc main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o -o a.out -lncurses -lreadline

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

clean:
		rm -rf main.o bonus.o pinfo.o ls.o builtIn.o executeCommand.o a.out