#include <SDL2/SDL.h>

#include "tfighter.h"

#define projectnum(x) (int)(x * gscale + .5f)
#define PLAYERS 2

int gheight = 600;
int gwidth = 800;
float gscale = 5;

SDL_Window *gwin = NULL;
SDL_Renderer *gren = NULL;

SDL_Rect temprect;

void close_game(){
	if(gren)
		SDL_DestroyRenderer(gren);
	if(gwin)
		SDL_DestroyWindow(gwin);
	gwin = NULL;
	gren = NULL;
	SDL_Quit();
}

int max(int a, int b){
	return a > b ? a : b;
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

void fillrect(trect *t){
	project(&temprect, t);
	SDL_RenderFillRect(gren, &temprect);
}

int main(int argc, char *argv[]){
	trect levelblocks[] = {{2, 16, 30, 1}, {3, 10, 2, 2}};
	hitbox boxes[100];
	tlevel level;
	int quit = 0;
	int i;
	SDL_Event e;
	SDL_Keycode c1[] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_j, SDLK_k};
	SDL_Keycode c2[] = {SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_KP_0, SDLK_KP_PERIOD};
	tfighter *fighters[PLAYERS] = {NULL, NULL};
	fighters[0] = tfighter_new(4, 4, 0x77, 0x55, 0x00, c1);
	fighters[1] = tfighter_new(6, 4, 0x00, 0x66, 0xbb, c2);
	level.blocks = levelblocks;
	level.len = 2;
	level.boxes = boxes;
	level.cbox = 0;
	level.MAX_BOXES = 100;
	
	check(SDL_Init(SDL_INIT_VIDEO) >= 0);
	gwin = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gwidth, gheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	check(gwin != NULL);
	gren = SDL_CreateRenderer(gwin, -1, SDL_RENDERER_ACCELERATED);
	check(gren != NULL);

	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				quit = 1;
				close_game();
			}
			else if(e.type == SDL_KEYDOWN){
				for(i=0; i<PLAYERS; ++i){
					tfighter_input(fighters[i], &level, 1, e.key.keysym.sym);
				}
			}
			else if(e.type == SDL_KEYUP){
				for(i=0; i<PLAYERS; ++i){
					tfighter_input(fighters[i], &level, 0, e.key.keysym.sym);
				}
			}
			else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
				gscale = e.window.data1 / 40;
				gwidth = e.window.data1;
				gheight = e.window.data2;
			}
			/*if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_A){
					boxes[0] = p1->move
				}
			}*/
		}

		SDL_SetRenderDrawColor(gren, 0x00, 0x33, 0x00, 0xFF);
		SDL_RenderClear(gren);

		for(i=0; i<PLAYERS; ++i){
			SDL_SetRenderDrawColor(gren, fighters[i]->red, fighters[i]->green, fighters[i]->blue, 0xFF);
			fillrect(&fighters[i]->rect);
			SDL_SetRenderDrawColor(gren, fighters[i]->red/2, fighters[i]->green/2, fighters[i]->blue/2, 0xFF);
			temprect.w /= 2;
			temprect.h /= 2;
			if(fighters[i]->state & DOWN){
				temprect.y += temprect.h;
			}
			else if(fighters[i]->state & UP){
				
			}
			else{
				temprect.y += temprect.h/2;
			}

			if(!fighters[i]->left){
				temprect.x += temprect.w;
			}
			SDL_RenderFillRect(gren, &temprect);
			tfighter_update(fighters[i], &level);
		}
	
		SDL_SetRenderDrawColor(gren, 0x00, 0x99, 0x00, 0xFF);
		for(i=0; i<level.len; ++i){
			fillrect(&level.blocks[i]);
		}

		SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0x11);
		for(i=0; i<level.MAX_BOXES; ++i){
			if(level.boxes[i].owner != 0){
				if(level.boxes[i].tick < level.boxes[i].delay){
					SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0x00, 0xFF);
				}
				else{
					SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0xFF);
				}
				hitbox_update(&level.boxes[i]);
				fillrect(&level.boxes[i].rect);
			}
		}

		SDL_RenderPresent(gren);

		SDL_Delay(20);
	}

	for(i=0; i<PLAYERS; ++i)
		free(fighters[i]);
	close_game();
	return 0;
}
