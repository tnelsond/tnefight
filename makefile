CC = gcc -pedantic -Wall -O2 -lSDL2 -lSDL2_image -lm

main.o : main.c tfighter.o scripting.o
	$(CC) main.c tfighter.o scripting.o -llua -o run.out

tfighter.o : tfighter.c tfighter.h
	$(CC) tfighter.c -c

scripting.o : scripting.c scripting.h
	$(CC) scripting.c -c

clean:
	rm *.o
