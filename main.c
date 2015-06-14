#include <SDL2/SDL.h>

#include "tfighter.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_Window *window = NULL;
SDL_Surface *screensurf = NULL;

void close_game(){
	if(window)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void check(int exp){
	if(exp)
		return;
	SDL_Log("%s", SDL_GetError());
	close_game();
}

int main(int argc, char *argv[]){
	/*trect rect = {20, 40, 80, 80};	*/
	hitbox box1 = {{10, 10, 20, 20}, 1, 1, 0, 0, 0, 0, 0, 0, 10, 20, 1, NULL};
	tfighter p1 = {{20, 40, 80, 80}, 0, 0, 10, 5, 5, 5, 10, 2, &box1};
	int quit = 0;
	SDL_Event e;
	
	check(SDL_Init(SDL_INIT_VIDEO) >= 0);
	window = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	check(window != NULL);
	screensurf = SDL_GetWindowSurface(window);

	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				quit = 1;
			}
		}
		SDL_FillRect(screensurf, NULL, SDL_MapRGB(screensurf->format, 0xFF, 0xFF, 0x00));
		tfighter_update(&p1);
		tfighter_draw(screensurf, &p1);
		SDL_Delay(20);
		SDL_UpdateWindowSurface(window);
	}

	close_game();
	return 0;
}
