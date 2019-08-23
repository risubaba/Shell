LDFLAGS=-lncurses -readline

a.out: main.o bonus.o
		gcc main.o bonus.o -o a.out -lncurses -lreadline

main.o: main.c
		gcc -c main.c

bonus.o: bonus.h bonus.c
	gcc -c bonus.c -o bonus.o

clean:
		rm -rf main.o bonus.o a.out