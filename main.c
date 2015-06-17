#include <SDL2/SDL.h>

#include "tfighter.h"

#define projectnum(x) (int)(x * gscale + .5f)

int gheight = 600;
int gwidth = 800;
float gscale = 10;

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
	trect levelblocks[] = {{2, 16, 17, 1}, {3, 10, 2, 2}};
	hitbox boxes[100];
	tlevel level;
	int quit = 0;
	SDL_Event e;
	tfighter *p1 = tfighter_new(4, 4);
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
		const Uint8* keystates = SDL_GetKeyboardState(NULL);
		int i;
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				quit = 1;
				close_game();
			}
			else if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_w){
					p1->vy = -1.0f;
				}
				else if(e.key.keysym.sym == SDLK_f){
					tlevel_add_hitbox(&level, p1, p1->moves);
				}
			}
			else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
				gscale = e.window.data1 / 20;
				gwidth = e.window.data1;
				gheight = e.window.data2;
			}
			/*if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_A){
					boxes[0] = p1->move
				}
			}*/
		}
		if(keystates[SDL_SCANCODE_A]){
			p1->vx -= p1->accel;
			p1->left = 1;
		}
		if(keystates[SDL_SCANCODE_D]){
			p1->vx += p1->accel;
			p1->left = 0;
		}

		SDL_SetRenderDrawColor(gren, 0x00, 0x33, 0x00, 0xFF);
		SDL_RenderClear(gren);

		SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0xFF, 0xFF);
		fillrect(&p1->rect);
		SDL_SetRenderDrawColor(gren, 0x00, 0x60, 0x60, 0xFF);
		temprect.w /= 2;
		temprect.h /= 2;
		if(!p1->left){
			temprect.x += temprect.w;
		}
		SDL_RenderFillRect(gren, &temprect);

		tfighter_update(p1, &level);
	
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

	free(p1);
	close_game();
	return 0;
}
