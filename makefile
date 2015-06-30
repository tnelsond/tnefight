CC = gcc -ansi -pedantic -Wall -O2 -lSDL2 -lSDL2_image

main.o : main.c tfighter.o
	$(CC) main.c tfighter.o -o run.out

tfighter.o : tfighter.c
	$(CC) tfighter.c -c
