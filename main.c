#include <SDL2/SDL.h>

#include "tfighter.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define projectnum(x) (int)(x * gscale + .5f)

SDL_Window *gwindow = NULL;
SDL_Renderer *gren = NULL;
float gscale = 40;

void close_game(){
	if(gren)
		SDL_DestroyRenderer(gren);
	if(gwindow)
		SDL_DestroyWindow(gwindow);
	gwindow = NULL;
	gren = NULL;
	SDL_Quit();
}

void check(int exp){
	if(exp)
		return;
	SDL_Log("%s", SDL_GetError());
	close_game();
}

SDL_Rect *project(SDL_Rect *r, trect *t){
	r->x = projectnum(t->x);
	r->y = projectnum(t->y);
	r->w = projectnum(t->w);
	r->h = projectnum(t->h);
	return r;
}

int main(int argc, char *argv[]){
	/*trect rect = {20, 40, 80, 80};	*/
	SDL_Rect temprect;
	hitbox boxes[20];
	hitbox box1 = {{10, 10, 20, 20}, 1, 1, 0, 0, 0, 0, 0, 0, 10, 20, 0, NULL};
	movebase move1 = {&box1, 0, 100, 1000, ATTACK};
	tfighter *p1 = tfighter_new(4, 4);
	int quit = 0;
	SDL_Event e;
	
	check(SDL_Init(SDL_INIT_VIDEO) >= 0);
	gwindow = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	check(gwindow != NULL);
	gren = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
	check(gren != NULL);

	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				quit = 1;
				close_game();
			}
			else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
				gscale = e.window.data1 / 20;
			}
			/*if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_A){
					boxes[0] = p1->move
				}
			}*/
		}
		SDL_SetRenderDrawColor(gren, 0x00, 0x33, 0x00, 0xFF);
		SDL_RenderClear(gren);

		SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0xFF, 0x77);

		SDL_RenderFillRect(gren, project(&temprect, &p1->rect));
		/*tfighter_update(p1);*/

		SDL_RenderPresent(gren);

		SDL_Delay(20);
	}

	free(p1);
	close_game();
	return 0;
}
