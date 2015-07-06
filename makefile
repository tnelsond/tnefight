CC = gcc -ansi -pedantic -Wall -O2 -lSDL2 -lSDL2_image -lm

main.o : main.c tfighter.o
	$(CC) main.c tfighter.o -o run.out

tfighter.o : tfighter.c tfighter.h
	$(CC) tfighter.c -c
