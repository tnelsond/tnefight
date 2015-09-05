#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <time.h>

#include "scripting.h"
#include "tfighter.h"

#define min(a, b) a < b ? a : b
#define MAXPARTICLES 100

SDL_Window *gwin = NULL;
SDL_GLContext gcon;
SDL_Joystick **gjoy = NULL;

SDL_Texture *gatlas = NULL;
#define glinew 32
#define gnlines 3
int PLAYERS = 0;
int MAXPLAYERS = 8;
int cparticle = 0;

trect tchar = {0, 0, (float)(1.0/glinew), (float)(1.0/gnlines)};
tparticle particles[MAXPARTICLES];

int debug = 0;

tcamera camera = {0, 0, 0, 0, 1, 1, 800, 600};
tfighter **fighters = NULL;
GLuint texture[1];

void close_game(){
	if(gwin)
		SDL_DestroyWindow(gwin);
	gwin = NULL;
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

void settchar(int num){
	tchar.y = tchar.h * (int)(num/glinew - 1);
	tchar.x = (num % glinew) * tchar.w;
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

void drawtext(trect *t, char *str){
	while(*str){
		settchar(*str);
		glBegin(GL_QUADS);
			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(tchar.x, tchar.y);
			glVertex2f(t->x, t->y);
			glTexCoord2f(tchar.x+tchar.w, tchar.y);
			glVertex2f(t->x + t->w, t->y);
			glTexCoord2f(tchar.x+tchar.w, tchar.y+tchar.h);
			glVertex2f(t->x + t->w, t->y + t->h);
			glTexCoord2f(tchar.x, tchar.y+tchar.h);
			glVertex2f(t->x, t->y + t->h);
		glEnd();
		glTranslatef(t->w, 0, 0);
		++str;
	}
}

void loadfont(){
	SDL_Surface *surf;
	SDL_Surface *surf2;
	SDL_PixelFormat pf;
	pf.palette = 0;
	pf.BitsPerPixel = 24;
	pf.BytesPerPixel = 4;
	pf.Rshift = pf.Rloss = pf.Gloss = pf.Bloss = pf.Aloss = 0;
	pf.Rmask = 0x000000ff;
	pf.Gshift = 8;
	pf.Gmask = 0x0000ff00;
	pf.Bshift = 16;
	pf.Bmask = 0x00ff0000;
	surf = IMG_Load("font.png");
	check(surf != NULL);
	surf2 = SDL_ConvertSurface(surf, &pf, 0);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surf2->w, surf2->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surf2->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	free(surf);
	free(surf2);
}

void filltrect(trect *t, float r, float g, float b, float a){
	glBegin(GL_QUADS);
		glColor4f(r, g, b, a);
		glVertex2f(t->x, t->y);
		glVertex2f(t->x, t->y + t->h);
		glVertex2f(t->x + t->w, t->y + t->h);
		glVertex2f(t->x + t->w, t->y);
	glEnd();
}

void filltriangle(trect *t, float r, float g, float b, float a){
	glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		glVertex2f(t->x, t->y);
		glVertex2f(t->x + t->w, t->y);
		glVertex2f(t->x + t->w/2, t->y + t->h);
	glEnd();
}

void fillslope(trect *t, int hyp, float r, float g, float b, float a){
	glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		if(hyp != 0)
			glVertex2f(t->x, t->y);
		if(hyp != 1)
			glVertex2f(t->x, t->y + t->h);
		glColor4f(r*2, g*2, b*2, a);
		if(hyp != 2)
			glVertex2f(t->x + t->w, t->y + t->h);
		if(hyp != 3)
		glColor4f(r/2, g/2, b/2, a);
			glVertex2f(t->x + t->w, t->y);
	glEnd();
}

void fillpolygon(float r, float g, float b, float a){
	glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		glVertex2f(0, 0);
		glVertex2f(-0.1f, 0.5f);
		glVertex2f(0.0f, 0.9f);
		glVertex2f(0.5f, 1.0f);
		glVertex2f(1.0f, 0.9f);
		glVertex2f(1.1f, 0.5f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(0.5f, 0.5f);
	glEnd();	
}

void draw(float alpha){
	int i;
	trect temp = {0, 0, 20, 20};

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, camera.width, camera.height, 0.0, 1.0, -1.0);

	tcamera_interpolate(&camera, alpha);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();
	glTranslatef(-camera.ix, -camera.iy, 0.0f);
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPushMatrix();

	filltrect(&level.rect, 0.6f, 0.6f, 0.7f, 1);

	glEnable(GL_TEXTURE_2D);
	glTranslatef(25, 14, 0.0f);
	temp.w = 1;
	temp.h = 1.5f;
	drawtext(&temp, "Hello 456. Lazy!!");
	glPopMatrix();
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);

	for(i = 0; i < level.len; ++i){
		glTranslatef(level.blocks[i].x, level.blocks[i].y, 0.0f);
		temp.w = level.blocks[i].w;
		temp.h = level.blocks[i].h;
		if(level.blocks[i].type == RECT){
			filltrect(&temp, 0.1f, 0.1f, 0.f, 1);
		}
		else{
			fillslope(&temp, level.blocks[i].type, 0.1f, 0.1f, 0.f, 1);
		}
		glPopMatrix();
		glPushMatrix();
	}

	for(i = 0; i < PLAYERS; ++i){
		glTranslatef(terp(fighters[i]->prect.x, fighters[i]->rect.x, alpha) + fighters[i]->rect.w / 2,
			terp(fighters[i]->prect.y, fighters[i]->rect.y, alpha) + fighters[i]->rect.h / 2, 0);
		if(fighters[i]->state & HITSTUN){
			glRotatef(fighters[i]->tick * 10, 0, 0, 1);
		}
		glTranslatef(-fighters[i]->rect.w/2, -fighters[i]->rect.h/2, 0);
		temp.w = fighters[i]->rect.w;
		temp.h = fighters[i]->rect.h;
		filltrect(&temp, tofloatcolor(fighters[i]->color), 1);
		/*fillpolygon(0, 1, 0, 1);*/
		glPopMatrix();
		glPushMatrix();
	}

	for(i=0; i<level.MAX_BOXES; ++i){
		if(level.boxes[i].owner){
			filltrect(&level.boxes[i].rect, 0, 1, 0, 0.5f);
		}
	}

	for(i=0; i<MAXPARTICLES; ++i){
		if(particles[i].time > 0){
			glTranslatef(particles[i].x, particles[i].y, 0.0f);
			glRotatef((particles[i].x + particles[i].y) * 10, 0, 0, 1);
			temp.w = particles[i].size;
			temp.h = particles[i].size;
			filltriangle(&temp,
				tofloatcolor(particles[i].color), particles[i].time / 128.0f);
			glPopMatrix();
			glPushMatrix();
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPopMatrix();

	temp.x = 3;
	temp.y = 3;
	temp.w = 1;
	temp.h = 1.5f;
	glEnable(GL_TEXTURE_2D);
	drawtext(&temp, "Hello 456. Lazy!!");
	glDisable(GL_TEXTURE_2D);

	SDL_GL_SwapWindow(gwin);
}

void setViewport(){
	glViewport(0.0f, 0.0f, camera.swidth, camera.sheight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, camera.width, camera.height, 0.0, 1.0, -1.0);
}

int initGL(){
	SDL_Log("Initting OPENGL.\n");
	GLenum error = GL_NO_ERROR;
	
	setViewport();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	error = glGetError();
	if(error != GL_NO_ERROR){
		SDL_Log("Error initializing OpenGL! %s\n", gluErrorString(error));
		return -1;
	}

	glEnable(GL_TEXTURE_2D);
	loadfont();

	SDL_Log("Initted OPENGL.\n");
	return 0;
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
	SDL_Keycode c[][8] = {{SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON},
											{SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_KP_0, SDLK_KP_PERIOD, SDLK_KP_ENTER, SDLK_KP_1}
											};
	Uint32 b[] = {ATTACKING, SPECIAL, 0, JUMP, SHIELDING};
	fighters = malloc(sizeof(*fighters) * MAXPLAYERS);
	Uint8 *skin = malloc(sizeof(Uint8) * 3 * MAXPLAYERS);
	/*for(i = 0; i < 3 * MAXPLAYERS; ++i){
		skin[i] = 0;
	}*/
	gjoy = malloc(sizeof(*gjoy) * MAXPLAYERS);


	check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) >= 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	gwin = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.swidth, camera.sheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	check(gwin != NULL);

	gcon = SDL_GL_CreateContext(gwin);
	check(gcon != NULL);
	check(SDL_GL_SetSwapInterval(1) >= 0);
	check(initGL() == 0);

	level.len = 256;
	level.cbox = 0;
	level.rect.x = 0;
	level.rect.y = 0;
	linit();
	srand(time(NULL));
	for(i = 0; i < argc - 1; ++i){
		if(strcmp(argv[i + 1], "-level") != 0){
			gjoy[i] = SDL_JoystickOpen(i);
			fighters[i] = tfighter_new(34 + i * 2, 10, 0x775500FF, (i <= 1) ?  c[i] : NULL, b, i, SDL_JoystickGetAxis(gjoy[i], 0), SDL_JoystickGetAxis(gjoy[i], 1), &skin[i*3]);
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
	level.spawnx = level.rect.w / 2;
	level.spawny = level.rect.y + 2;
	level.len = level.cbox;
	level.blocks = realloc(level.blocks, sizeof(trect) * level.len);
	for(i=0; i<PLAYERS; ++i){
		fighters[i]->rect.x = level.spawnx;
		fighters[i]->rect.y = level.spawny;
	}

	level.boxes = boxes;
	level.cbox = 0;
	level.MAX_BOXES = 30;
	for(i=0; i<level.MAX_BOXES; ++i){
		boxes[i].owner = NULL;
	}
	
	ttime = oldtime = SDL_GetTicks();
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
					setViewport();
				}
				else{
					for(i=0; i<PLAYERS; ++i){
						tfighter_input(fighters[i], &level, &e);
					}
				}
			}

			tcamera_track(&camera, &level, fighters, PLAYERS);

			for(i=0; i<PLAYERS; ++i){
				tfighter_update(fighters[i], &level);
				if(fighters[i]->state & HITSTUN){
					float mag = (float)(rand() % 1000 / 1000.0f);
					Uint32 color = rand() % 0xFF;
					color = 0xFF + color * 0x100 + color * 0x10000 + color * 0x1000000;
					tparticle_set(&particles[cparticle], fighters[i]->rect.x + ((rand() % 1000) / 1000.0) * fighters[i]->rect.w, fighters[i]->rect.y + ((rand() % 1000) / 1000.0) * fighters[i]->rect.h, fighters[i]->vx * mag, fighters[i]->vy * mag, 0.5f, 90, color);
					cparticle = (cparticle + 1) % MAXPARTICLES;
				}
			}
		
			for(i=0; i<level.MAX_BOXES; ++i){
				if(level.boxes[i].owner){
					hitbox_update(&level.boxes[i]);
				}
			}
			accumulator -= physicsstep;
		}

		for(i=0; i<MAXPARTICLES; ++i){
			if(particles[i].time > 0){
				tparticle_update(&particles[i]);
			}
		}
		alpha = ((float)accumulator) / physicsstep;
		draw(alpha);
		
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
