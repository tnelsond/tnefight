#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tfighter.h"

#define min(a, b) a < b ? a : b
#define PLAYERS 2

SDL_Window *gwin = NULL;
SDL_Renderer *gren = NULL;
SDL_Joystick* gjoy = NULL;
SDL_Joystick* gjoy2 = NULL;

SDL_Texture *gatlas;
int glinew = 32;
SDL_Rect charrect = {0, 0, 6, 12};

tcamera camera = {0, 0, 1, 800, 600};
tlevel level;
tfighter *fighters[PLAYERS] = {NULL, NULL};

SDL_Rect temprect;

void drawtext(char *str, float x, float y, float w, float h){
	int i;
	projecthud(&camera, &temprect, x, y, w, h);
	for(i=0; str[i]; ++i){
		charrect.x = (str[i] % 32) * charrect.w;
		charrect.y = (str[i] / 32 - 1) * charrect.h;
		SDL_RenderCopy(gren, gatlas, &charrect, &temprect);	
		temprect.x += temprect.w;
	}
}


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

void fillrect(tcamera *tc, trect *t, trect *p, float alpha){
	project(tc, t, p, &temprect, alpha);
	SDL_RenderFillRect(gren, &temprect);
}

void fillrect2(tcamera *tc, trect *t, float alpha){
	project2(tc, t, &temprect, alpha);
	SDL_RenderFillRect(gren, &temprect);
}

void fillrect_simp(int x, int y, int w, int h){
	temprect.x = x;
	temprect.y = y;
	temprect.w = w;
	temprect.h = h;
	SDL_RenderFillRect(gren, &temprect);
}

void draw(float alpha){
	int i;
	char disp[] = "   %";
	SDL_SetRenderDrawColor(gren, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(gren);

	SDL_SetRenderDrawColor(gren, 0xaa, 0xaa, 0xaa, 0xFF);
	fillrect2(&camera, &level.rect, alpha);

	for(i=0; i<PLAYERS; ++i){
		SDL_SetRenderDrawColor(gren, fighters[i]->red, fighters[i]->green, fighters[i]->blue, 0xFF);
		fillrect(&camera, &fighters[i]->rect, &fighters[i]->prect, alpha);
		if(fighters[i]->state & HITSTUN){
			SDL_SetRenderDrawColor(gren, 0x77, 0, 0, 0xFF);
		}
		else if(fighters[i]->state & ATTACKING){
			SDL_SetRenderDrawColor(gren, 0, 0, 0xFF, 0x77);
		}
		else if(fighters[i]->state & SPECIAL){
			SDL_SetRenderDrawColor(gren, 0x99, 0x99, 0x00, 0x77);
		}
		else{
			SDL_SetRenderDrawColor(gren, fighters[i]->red/2, fighters[i]->green/2, fighters[i]->blue/2, 0xFF);
		}
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
	}

	SDL_SetRenderDrawColor(gren, 0x44, 0x44, 0x44, 0xFF);
	for(i=0; i<level.len; ++i){
		fillrect2(&camera, &level.blocks[i], alpha);
	}

	SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0x11);
	for(i=0; i<level.MAX_BOXES; ++i){
		if(level.boxes[i].owner){
			if(level.boxes[i].tick < level.boxes[i].maxdelay){
				SDL_SetRenderDrawColor(gren, 0x00, 0xFF, 0x00, 0xFF);
			}
			else{
				SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0xFF);
			}
			fillrect(&camera, &level.boxes[i].rect, &level.boxes[i].prect, alpha);
		}
	}
	for(i=0; i<PLAYERS; ++i){
		disp[2] = '0' + fighters[i]->damage % 10;
		disp[1] = '0' + (fighters[i]->damage % 100) / 10;
		disp[0] = '0' + fighters[i]->damage / 100;
		if(disp[0] == '0'){
			disp[0] = ' ';
			if(disp[1] == '0'){
				disp[1] = ' ';
			}
		}
		drawtext(disp, 0.01f + 0.05f * 5 * i, 0.01f, 0.05f, 0.1f);
	}
}

void loadfont(){
	SDL_Surface *surf = IMG_Load("font.gif");
	check(surf != NULL);
	gatlas = SDL_CreateTextureFromSurface(gren, surf);
	check(gatlas != NULL);
	SDL_FreeSurface(surf);
}

int main(int argc, char *argv[]){
	Uint32 time, oldtime;
	trect levelblocks[] = {{20, 35, 4, 20}, {23, 34, 3, 20}, {25, 32, 40, 20}, {47, 16, 5, 0.5f}, {50, 27, 5, 0.5f}, {57, 22, 5, 0.5f}, {60, 36, 20, 20}, {70, 22, 5, 8}};
	hitbox boxes[30];
	int quit = 0;
	int i;
	Uint32 accumulator = 0;
	Uint32 physicsstep = 1000 / 60; /* 60 fps physics */
	Uint32 vfps = 1000 / 60; /* 60 fps */
	SDL_Event e;
	SDL_Keycode c1[] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_j, SDLK_k, SDLK_l};
	Uint32 b1[] = {ATTACKING, SPECIAL, 0, JUMP, 0};
	SDL_Keycode c2[] = {SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_KP_0, SDLK_KP_PERIOD, SDLK_KP_ENTER};
	fighters[0] = tfighter_new(34, 15, 0x77, 0x55, 0x00, c1, b1, 0, SDL_JoystickGetAxis(gjoy, 0), SDL_JoystickGetAxis(gjoy, 1));
	fighters[1] = tfighter_new(36, 15, 0x00, 0x66, 0xbb, c2, b1, 1, SDL_JoystickGetAxis(gjoy2, 0), SDL_JoystickGetAxis(gjoy2, 1));
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
	
	check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) >= 0);

	gwin = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.swidth, camera.sheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	check(gwin != NULL);

	gren = SDL_CreateRenderer(gwin, -1, SDL_RENDERER_ACCELERATED);
	check(gren != NULL);

	check(IMG_Init(IMG_INIT_JPG));
	loadfont();

	gjoy = SDL_JoystickOpen( 0 );
	check(gjoy != NULL);
	gjoy2 = SDL_JoystickOpen( 1 );
	check(gjoy2 != NULL);

	oldtime = SDL_GetTicks();
	time = oldtime;

	while(!quit){
		Uint32 delta;
		int timesleep;
		float alpha = 0;
		oldtime = time;
		time = SDL_GetTicks();
		delta = time - oldtime;
		accumulator += min(delta, 100);
		while(accumulator >= physicsstep){
			while(SDL_PollEvent(&e) != 0){
				if(e.type == SDL_QUIT){
					quit = 1;
					close_game();
				}
				else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
					camera.swidth = e.window.data1;
					camera.sheight = e.window.data2;
				}
				else{
					for(i=0; i<PLAYERS; ++i){
						tfighter_input(fighters[i], &level, &e);
					}
				}
			}

			tcamera_track(&camera, &fighters[0]->rect, &fighters[1]->rect);

			for(i=0; i<PLAYERS; ++i){
				tfighter_update(fighters[i], &level);
			}
		
			for(i=0; i<level.MAX_BOXES; ++i){
				if(level.boxes[i].owner){
					hitbox_update(&level.boxes[i]);
				}
			}
			accumulator -= physicsstep;
		}
		alpha = ((float)accumulator) / physicsstep;
		draw(alpha);
		SDL_RenderPresent(gren);
		
		timesleep = vfps - delta;
		if(timesleep > 0)
			SDL_Delay(timesleep);
	}

	for(i=0; i<PLAYERS; ++i)
		free(fighters[i]);
	close_game();
	return 0;
}
