LDFLAGS=-lncurses -readline

a.out: main.o bonus.o pinfo.o ls.o
		gcc main.o bonus.o pinfo.o ls.o -o a.out -lncurses -lreadline

main.o: main.c
		gcc -c main.c

bonus.o: bonus.h bonus.c
	gcc -c bonus.c -o bonus.o

pinfo.o: pinfo.h pinfo.c
	gcc -c pinfo.c -o pinfo.o

ls.o: ls.h ls.c
	gcc -c ls.c -o ls.o

clean:
		rm -rf main.o bonus.o pinfo.o ls.o a.out