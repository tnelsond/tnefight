CC = gcc -ansi -pedantic -Wall -O3 -lSDL2

main.o : main.c tfighter.o
	$(CC) main.c tfighter.o -o run.out

tfighter.o : tfighter.c
	$(CC) tfighter.c -c
