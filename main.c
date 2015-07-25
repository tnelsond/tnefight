#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <time.h>

#include "scripting.h"
#include "tfighter.h"

#define min(a, b) a < b ? a : b

SDL_Window *gwin = NULL;
SDL_Renderer *gren = NULL;
SDL_Joystick **gjoy = NULL;

SDL_Texture *gatlas = NULL;
int glinew = 32;
int gnlines = 3;
int PLAYERS = 0;
int MAXPLAYERS = 8;
SDL_Rect charrect = {0, 0, 6, 12};
SDL_Rect imgrect = {0, 0, 7, 8};

int debug = 0;
char debugtest[256];

tcamera camera = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 800, 600};
tfighter **fighters = NULL;

SDL_Rect temprect;

void drawtext(char *str, float alpha, float x, float y, float w, float h, int dynamic){
	int i;
	SDL_Rect temp2;
	if(dynamic){
		project3(&camera, &temp2, alpha, x, y, w, h);
	}
	else{
		projecthud(&camera, &temp2, x, y, w, h);
	}
	for(i=0; str[i]; ++i){
		charrect.x = (str[i] % glinew) * charrect.w;
		charrect.y = (str[i] / glinew - 1) * charrect.h;
		SDL_RenderCopy(gren, gatlas, &charrect, &temp2);	
		temp2.x += temp2.w;
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

void setimgrect(int num){
	imgrect.y = charrect.h * gnlines + num/glinew;
	imgrect.x = (num % glinew) * imgrect.w;
}

void copyrect(SDL_Rect *in, SDL_Rect *out){
	out->x = in->x;
	out->y = in->y;
	out->w = in->w;
	out->h = in->h;
}

void copytrect(trect *in, trect *out){
	out->x = in->x;
	out->y = in->y;
	out->w = in->w;
	out->h = in->h;
}

void draw(float alpha){
	int i;
	char disp[] = "   %";

	SDL_SetRenderDrawColor(gren, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(gren);

	SDL_SetRenderDrawColor(gren, 0xaa, 0xaa, 0xaa, 0xFF);
	fillrect2(&camera, &level.rect, alpha);

	for(i=0; i<PLAYERS; ++i){
		int walktick;
		if(fighters[i] == NULL){
			break;
		}
		SDL_Rect temp2;
		setimgrect(fighters[i]->skin[1]);
		SDL_SetRenderDrawColor(gren, fighters[i]->red, fighters[i]->green, fighters[i]->blue, 0xFF);
		project(&camera, &fighters[i]->rect, &fighters[i]->prect, &temprect, alpha);

		/* Feet */
		copyrect(&temprect, &temp2);
		walktick = (int)(fighters[i]->rect.x / fighters[i]->rect.w * 10) % 10; 
		temp2.x += walktick * fighters[i]->rect.w * camera.scale / 20;
		temp2.h /= 2;
		temp2.w /= 2;
		temp2.y += temp2.h;
		setimgrect(fighters[i]->skin[2]);
		SDL_SetTextureColorMod(gatlas, fighters[i]->red * 8/10, fighters[i]->green * 8/10, fighters[i]->blue * 8/10);
		SDL_RenderCopyEx(gren, gatlas, &imgrect, &temp2, fighters[i]->state & HITSTUN ? fighters[i]->tick * 5 * (fighters[i]->vx > 0 ? -1 : 1) : walktick * 4 * (fighters[i]->left ? -1 : 1), NULL, fighters[i]->left);	
		temp2.x = temprect.x * 2 + temprect.w / 2 - temp2.x;
		SDL_RenderCopyEx(gren, gatlas, &imgrect, &temp2, fighters[i]->state & HITSTUN ? fighters[i]->tick * 5 * (fighters[i]->vx > 0 ? -1 : 1) : 0, NULL, fighters[i]->left);	

		/* Chest */
		copyrect(&temprect, &temp2);
		temp2.h = temp2.h * 2 / 3;
		setimgrect(fighters[i]->skin[0]);
		SDL_SetTextureColorMod(gatlas, fighters[i]->red, fighters[i]->green, fighters[i]->blue);
		SDL_RenderCopyEx(gren, gatlas, &imgrect, &temp2, fighters[i]->state & HITSTUN ? fighters[i]->tick * 5 * (fighters[i]->vx > 0 ? 1 : -1) : 0, NULL, fighters[i]->left);	
		/*fillrect(&camera, &fighters[i]->rect, &fighters[i]->prect, alpha);*/


		/* Head */
		if(fighters[i]->state & HITSTUN){
			SDL_SetTextureColorMod(gatlas, 0x77, 0x0, 0x0);
		}
		else if(fighters[i]->state & ATTACKING){
			SDL_SetTextureColorMod(gatlas, 0x0, 0xFF, 0x77);
		}
		else if(fighters[i]->state & SPECIAL){
			SDL_SetTextureColorMod(gatlas, 0x99, 0x99, 0x00);
		}
		else{
			SDL_SetTextureColorMod(gatlas, fighters[i]->red*3/4, fighters[i]->green*3/4, fighters[i]->blue*3/4);
		}
		temprect.y -= temprect.h/2;
		temprect.w = temprect.w*3/4;
		temprect.h = temprect.h*3/4;
		temprect.x -= temprect.w/10;
		if(fighters[i]->state & DOWN){
			temprect.y += temprect.h/2;
		}
		else if(~fighters[i]->state & UP){
			temprect.y += temprect.h/4;
		}
		if(!fighters[i]->left){
			temprect.x += temprect.w/2;
		}
		setimgrect(fighters[i]->skin[1]);
		SDL_RenderCopyEx(gren, gatlas, &imgrect, &temprect, fighters[i]->tick * (fighters[i]->vx > 0 ? 1 : -1) * (fighters[i]->state & HITSTUN ? 10 : 1), NULL, fighters[i]->left);	
		SDL_SetTextureColorMod(gatlas, 0, 0, 0);
		if(fighters[i]->name){
			drawtext(fighters[i]->name, alpha, fighters[i]->rect.x, fighters[i]->rect.y - 1.8f, 1, 1, 1);/*fighters[i]->rect.x, fighters[i]->rect.y - 0.1f, 1.0f, 0.5f, 1); */
		}
	}

	SDL_SetRenderDrawColor(gren, 0x44, 0x44, 0x44, 0xFF);
	for(i=0; i<level.len; ++i){
		fillrect2(&camera, &level.blocks[i], alpha);
	}

	SDL_SetRenderDrawColor(gren, 0xFF, 0x00, 0x00, 0x11);
	for(i=0; i<level.MAX_BOXES; ++i){
		if(level.boxes[i].owner){
			if(level.boxes[i].type & SHIELD){
				SDL_SetTextureColorMod(gatlas, 0x00, 0x44, 0xFF);
			}
			else if(level.boxes[i].tick < level.boxes[i].maxdelay){
				SDL_SetTextureColorMod(gatlas, 0x00, 0xFF, 0x00);
			}
			else{
				SDL_SetTextureColorMod(gatlas, 0xFF, 0x00, 0x00);
			}
			setimgrect(level.boxes[i].hitlag > 0 ? 11 : level.boxes[i].image);
			project(&camera, &level.boxes[i].rect, &level.boxes[i].prect, &temprect, alpha);
			SDL_RenderCopyEx(gren, gatlas, &imgrect, &temprect, 0, NULL, level.boxes[i].left);	
			/*fillrect(&camera, &level.boxes[i].rect, &level.boxes[i].prect, &temprect, alpha);*/
		}
	}
	SDL_SetTextureColorMod(gatlas, 0, 0, 0);
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
		drawtext(disp, alpha, 0.01f + 0.025f * 5 * i, 0.01f, 0.025f, 0.04f, 0);
	}
	if(debug){
		sprintf(debugtest, "%d, %d, %d", fighters[0]->hitlag, fighters[0]->tick, fighters[0]->state);
		drawtext(debugtest, alpha, 0.01f + 0.05f, 0.1f, 0.01f, 0.02f, 0);
	}
}

void loadfont(){
	SDL_Surface *surf = IMG_Load("font.gif");
	check(surf != NULL);
	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
	gatlas = SDL_CreateTextureFromSurface(gren, surf);
	check(gatlas != NULL);
	SDL_FreeSurface(surf);
}

int main(int argc, char *argv[]){
	Uint32 ttime, oldtime;
	hitbox boxes[30];
	int quit = 0;
	int i;
	Uint32 accumulator = 0;
	Uint32 physicsstep = 1000 / 60; /* 60 fps physics */
	Uint32 vfps = 1000 / 60; /* 60 fps */
	SDL_Event e;
	SDL_Keycode c[][8] = {{SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_7, SDLK_8, SDLK_9, SDLK_0},
											{SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_u, SDLK_i, SDLK_o, SDLK_p},
											{SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_KP_0, SDLK_KP_PERIOD, SDLK_KP_ENTER, SDLK_KP_1},
											{SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON},
											{SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_m, SDLK_LESS, SDLK_GREATER, SDLK_QUESTION}
											};
	Uint32 b[] = {ATTACKING, SPECIAL, 0, JUMP, SHIELDING};
	fighters = malloc(sizeof(*fighters) * MAXPLAYERS);
	Uint8 *skin = malloc(sizeof(Uint8) * 3 * MAXPLAYERS);
	gjoy = malloc(sizeof(*gjoy) * MAXPLAYERS);
	camera.bw = level.rect.w;
	camera.bh = level.rect.h;


	check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) >= 0);

	gwin = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.swidth, camera.sheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	check(gwin != NULL);

	gren = SDL_CreateRenderer(gwin, -1, SDL_RENDERER_ACCELERATED);
	check(gren != NULL);

	check(IMG_Init(IMG_INIT_JPG));
	loadfont();

	level.len = 256;
	level.cbox = 0;
	level.rect.x = 0;
	level.rect.y = 0;
	linit();
	srand(time(NULL));
	for(i = 0; i < argc - 1; ++i){
		if(strcmp(argv[i + 1], "-level") != 0){
			gjoy[i] = SDL_JoystickOpen(i);
			fighters[i] = tfighter_new(34 + i * 2, 10, 0x77, 0x55, 0x00, c[i], b, i, SDL_JoystickGetAxis(gjoy[i], 0), SDL_JoystickGetAxis(gjoy[i], 1), &skin[i]);
			cfighter = fighters[i];
			PLAYERS = i + 1;
			lua_pushnumber(l, rand());
			lua_setglobal(l, "seed");	
			lrunscript(argv[i + 1]);
		}
		else{
			SDL_Log("Loading level");
			cfighter = NULL;
			lrunscript(argv[i + 2]);
			SDL_Log("Loaded level");
			break;
		}
	}
	if(level.blocks == NULL){
		lrunscript("defaultlevel.lua");
	}
	lclose();
	level.len = level.cbox;
	level.blocks = realloc(level.blocks, sizeof(trect) * level.len);


	level.boxes = boxes;
	level.cbox = 0;
	level.MAX_BOXES = 30;
	for(i=0; i<level.MAX_BOXES; ++i){
		boxes[i].owner = NULL;
	}
	
	oldtime = SDL_GetTicks();
	ttime = oldtime;

	while(!quit){
		Uint32 delta;
		int timesleep;
		float alpha = 0;
		oldtime = ttime;
		ttime = SDL_GetTicks();
		delta = ttime - oldtime;
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

			tcamera_track(&camera, fighters, PLAYERS);

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

	for(i=0; i<PLAYERS; ++i){
		if(fighters[i]->name){
			free(fighters[i]->name);
		}
		free(fighters[i]);
		/* BUGGY 
		if(gjoy[i] != NULL){
			SDL_JoystickClose(gjoy[i]);
			gjoy[i] = NULL;
		}
		*/
	}
	free(fighters);
	free(skin);
	free(gjoy);
	close_game();
	return 0;
}
