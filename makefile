CC = gcc -pedantic -Wall -O2 `sdl2-config --cflags --libs` -lSDL2_net -lGL -lGLU -lSDL2_image -lm
LUA = `pkg-config --libs lua`

run.out : main.c tfighter.o scripting.o
	$(CC) main.c tfighter.o scripting.o $(LUA) -o run.out

tfighter.o : tfighter.c tfighter.h
	$(CC) tfighter.c -c

scripting.o : scripting.c scripting.h
	$(CC) scripting.c -c

clean:
	rm *.o
