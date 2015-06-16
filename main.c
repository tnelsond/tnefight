#include <SDL2/SDL.h>

#include "tfighter.h"

#define projectnum(x) (int)(x * gscale + .5f)

int gheight = 600;
int gwidth = 800;
float gscale = 10;

SDL_Window *gwin = NULL;
SDL_Renderer *gren = NULL;

hitbox gboxes[20];
int gcbox = 0;
int gid = 1;
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
/*
	if(r->x < 0){
		r->w += r->x;
	}
	if(r->y < 0){
		r->h += r->y;
	}
	if(r->x + r->w > gwidth){
		r->w = gwidth - r->x;
	}
	if(r->y + r->h > gheight){
		r->h = gheight - r->y;
	}
*/
	return r;
}

void fillrect(trect *t){
	project(&temprect, t);
	SDL_RenderFillRect(gren, &temprect);
}

int main(int argc, char *argv[]){
	/*trect rect = {20, 40, 80, 80};	*/
	trect levelblocks[1] = {{2, 16, 17, 1}};
	tfighter *p1 = tfighter_new(4, 4);
	int quit = 0;
	SDL_Event e;
	
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
				if(e.key.keysym.sym == SDLK_w){
					p1->vy = -1.0f;
				}
				else if(e.key.keysym.sym == SDLK_f){
					hitbox *box = &gboxes[gcbox];
					hitbox_clone(&p1->rect, p1->moves, box);
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
		const Uint8* keystates = SDL_GetKeyboardState(NULL);
		if(keystates[SDL_SCANCODE_A]){
			p1->vx -= p1->accel;
		}
		if(keystates[SDL_SCANCODE_D]){
			p1->vx += p1->accel;
		}
		SDL_SetRenderDrawColor(gren, 0x00, 0x33, 0x00, 0xFF);
		SDL_RenderClear(gren);

		SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0xFF, 0x77);

		fillrect(&p1->rect);
		tfighter_update(p1, levelblocks);
		hitbox_update(&gboxes[0]);
	
		SDL_SetRenderDrawColor(gren, 0x00, 0x99, 0x00, 0xFF);
		int i;
		for(i=sizeof(levelblocks); i>=0; --i){
			/*SDL_RenderFillRect(gren, project(&temprect, &levelblocks[i]));*/
			fillrect(&levelblocks[i]);
		}

		if(gboxes[0].owner != 0){
			fillrect(&gboxes[0]);
		}

		SDL_RenderPresent(gren);

		SDL_Delay(20);
	}

	free(p1);
	close_game();
	return 0;
}
