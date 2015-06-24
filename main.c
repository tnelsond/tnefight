#include <SDL2/SDL.h>

#include "tfighter.h"

#define projectnum(x) (int)(x * gscale + .5f)
#define PLAYERS 2

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

void fillrect(tcamera *tc, trect *t){
	project(tc, t, &temprect);
	SDL_RenderFillRect(gren, &temprect);
}

int main(int argc, char *argv[]){
	tcamera camera = {0, 0, 1, 800, 600};
	trect levelblocks[] = {{20, 35, 4, 20}, {23, 34, 3, 20}, {25, 32, 40, 20}, {47, 16, 5, 0.5f}, {50, 27, 5, 0.5f}, {57, 22, 5, 0.5f}, {60, 36, 20, 20}, {70, 22, 5, 8}};
	hitbox boxes[30];
	tlevel level;
	int quit = 0;
	int i;
	SDL_Event e;
	SDL_Keycode c1[] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_j, SDLK_k};
	SDL_Keycode c2[] = {SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_KP_0, SDLK_KP_PERIOD};
	tfighter *fighters[PLAYERS] = {NULL, NULL};
	fighters[0] = tfighter_new(34, 15, 0x77, 0x55, 0x00, c1);
	fighters[1] = tfighter_new(36, 15, 0x00, 0x66, 0xbb, c2);
	level.blocks = levelblocks;
	level.len = 8;
	level.rect.x = 0;
	level.rect.y = 0;
	level.rect.w = 100;
	level.rect.h = 52;
	level.boxes = boxes;
	level.cbox = 0;
	level.MAX_BOXES = 30;

	for(i=0; i<level.MAX_BOXES; ++i){
		boxes[i].owner = NULL;
	}
	
	check(SDL_Init(SDL_INIT_VIDEO) >= 0);
	gwin = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.swidth, camera.sheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
				camera.swidth = e.window.data1;
				camera.sheight = e.window.data2;
			}
		}

		SDL_SetRenderDrawColor(gren, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gren);

		SDL_SetRenderDrawColor(gren, 0xaa, 0xaa, 0xaa, 0xFF);
		fillrect(&camera, &level.rect);

		tcamera_track(&camera, &fighters[0]->rect, &fighters[1]->rect);

		for(i=0; i<PLAYERS; ++i){
			SDL_SetRenderDrawColor(gren, fighters[i]->red, fighters[i]->green, fighters[i]->blue, 0xFF);
			fillrect(&camera, &fighters[i]->rect);
			SDL_SetRenderDrawColor(gren, fighters[i]->red/2, fighters[i]->green/2, fighters[i]->blue/2, 0xFF);
			temprect.w /= 2;
			temprect.h /= 2;
			if(fighters[i]->state & DOWN){
				temprect.y += temprect.h;
			}
			else if(~fighters[i]->state & UP){
				temprect.y += temprect.h/2;
			}
			if(!fighters[i]->left){
				temprect.x += temprect.w;
			}
			SDL_RenderFillRect(gren, &temprect);
			tfighter_update(fighters[i], &level);
		}
	
		SDL_SetRenderDrawColor(gren, 0x44, 0x44, 0x44, 0xFF);
		for(i=0; i<level.len; ++i){
			fillrect(&camera, &level.blocks[i]);
		}

		SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0x11);
		for(i=0; i<level.MAX_BOXES; ++i){
			if(level.boxes[i].owner){
				if(level.boxes[i].tick < level.boxes[i].delay){
					SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0x00, 0xFF);
				}
				else{
					SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0xFF);
				}
				hitbox_update(&level.boxes[i]);
				fillrect(&camera, &level.boxes[i].rect);
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
