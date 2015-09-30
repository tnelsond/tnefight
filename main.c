#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <time.h>

#include <SDL/SDL_net.h>

#include "scripting.h"
#include "tfighter.h"

#define min(a, b) a < b ? a : b
#define MAXPARTICLES 200

SDL_Window *gwin = NULL;
SDL_GLContext gcon;
SDL_Joystick **gjoy = NULL;

SDL_Texture *gatlas = NULL;
#define glinew 32
#define gnlines 3
int PLAYERS = 0;
int MAXPLAYERS = 8;
int cparticle = 0;

char disp[5] = {' ', ' ', ' ', '%', '\0'};

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

void drawtext(trect *t, char *str, float r, float g, float b, float a){
	glColor4f(r, g, b, a);
	while(*str){
		settchar(*str);
		glBegin(GL_QUADS);
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
	pf.BitsPerPixel = 32;
	pf.BytesPerPixel = 4;
	pf.Rshift = pf.Rloss = pf.Gloss = pf.Bloss = pf.Aloss = 0;
	pf.Rmask = 0x000000ff;
	pf.Gshift = 8;
	pf.Gmask = 0x0000ff00;
	pf.Bshift = 16;
	pf.Bmask = 0x00ff0000;
	pf.Ashift = 24;
	pf.Amask = 0xff000000;
	surf = IMG_Load("font.png");
	check(surf != NULL);
	surf2 = SDL_ConvertSurface(surf, &pf, 0);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf2->w, surf2->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf2->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	free(surf);
	free(surf2);
}

void fillrect(float r, float g, float b, float a){
	glBegin(GL_QUADS);
		glColor4f(r, g, b, a);
		glVertex2f(0, 0);
		glVertex2f(0, 1);
		glVertex2f(1, 1);
		glVertex2f(1, 0);
	glEnd();
}

void filltriangle(float r, float g, float b, float a){
	glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		glVertex2f(0, 0);
		glVertex2f(1, 0);
		glVertex2f(0.5f, 1);
	glEnd();
}

void fillslope(int hyp, float r, float g, float b, float a){
	glBegin(GL_TRIANGLES);
		glColor4f(r, g, b, a);
		if(hyp != 0)
			glVertex2f(0, 0);
		if(hyp != 1)
			glVertex2f(0, 1);
		glColor4f(r*2, g*2, b*2, a);
		if(hyp != 2)
			glVertex2f(1, 1);
		if(hyp != 3)
		glColor4f(r/2, g/2, b/2, a);
			glVertex2f(1, 0);
	glEnd();
}

void fillpolygon(int skin, float r, float g, float b, float a){
	glColor4f(r, g, b, a);
	switch(skin){
		case 0:
			glBegin(GL_QUADS);
				glVertex2f(0, 0);
				glVertex2f(0, 1);
				glVertex2f(1, 1);
				glVertex2f(1, 0);
			break;
		case 1:
			glBegin(GL_TRIANGLES);
				glVertex2f(.2f, 0);
				glVertex2f(.5f, 0.25f);
				glVertex2f(0, 0.25f);

				glVertex2f(.8f, 0);
				glVertex2f(.5f, 0.25f);
				glVertex2f(1, 0.25f);

				glVertex2f(0, 0.25f);
				glVertex2f(0.5f, 1);
				glVertex2f(1, 0.25f);
			break;
		case 2:
			glBegin(GL_TRIANGLES);
				glVertex2f(0, 0);
				glVertex2f(0, 1);
				glVertex2f(1, 1);
			break;
		case 3:
			glBegin(GL_QUADS);
				glVertex2f(0.25f, 0);
				glVertex2f(0.25f, 1);
				glVertex2f(0.75f, 1);
				glVertex2f(0.75f, 0);

				glVertex2f(0, 0.25f);
				glVertex2f(0, 0.75f);
				glVertex2f(1, 0.75f);
				glVertex2f(1, 0.25f);
			break;
		case 4:
			glBegin(GL_TRIANGLES);
				glVertex2f(0, 1);
				glVertex2f(0.3f, 1);
				glVertex2f(0.4f, 0.5f);

				glVertex2f(0.7f, 1);
				glVertex2f(0.6f, 0.5f);
				glVertex2f(1, 1);

				glVertex2f(0.3f, 0.7f);
				glVertex2f(0.7f, 0.7f);
				glVertex2f(0.5f, 0);
			break;
		default:
			glBegin(GL_TRIANGLES);
				glVertex2f(0, 0);
				glVertex2f(0.5f, 0.7f);
				glVertex2f(1, 0);

				glVertex2f(0, 1);
				glVertex2f(0.5f, 0.3f);
				glVertex2f(1, 1);
	}
	glEnd();
}

void draw(float alpha){
	int i;
	trect temp = {0, 0, 20, 20};

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	tcamera_interpolate(&camera, alpha);
	glOrtho(0.0, camera.width, camera.height, 0.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();
	glTranslatef(-camera.ix, -camera.iy, 0.0f);
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT);
	/*
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPushMatrix();
	*/

	glTranslatef(level.rect.x, level.rect.y, 0);
	glScalef(level.rect.w, level.rect.h, 0);
	fillrect(0.6f, 0.6f, 0.7f, 1);
	glPopMatrix();
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glTranslatef(25, 14, 0.0f);
	glPopMatrix();
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);

	for(i = 0; i < level.len; ++i){
		glTranslatef(level.blocks[i].x, level.blocks[i].y, 0.0f);
		glScalef(level.blocks[i].w, level.blocks[i].h, 0);
		if(level.blocks[i].type == RECT){
			fillrect(0.1f, 0.1f, 0.f, 1);
		}
		else{
			fillslope(level.blocks[i].type, 0.1f, 0.1f, 0.f, 1);
		}
		glPopMatrix();
		glPushMatrix();
	}

	for(i = 0; i < PLAYERS; ++i){
		tfighter *t = fighters[i];
		glPopMatrix();
		glPushMatrix();
		glTranslatef(terp(t->prect.x, t->rect.x, alpha) + t->rect.w / 2,
			terp(t->prect.y, t->rect.y, alpha) + t->rect.h / 2, 0);
		glPushMatrix();
		/*glRotatef(t->tick*10 * (t->state & LEFT ? 1 : -1), 0, 1, 0);*/
		if(t->left){
			glRotatef(180, 0, 1, 0);
		}
		glRotatef(t->anim, 0, 1, 0);
		if(t->state & HITSTUN){
			glRotatef(t->tick * 10 * (t->vx < 0 ? 1 : -1), 0, 0, 1);
		}
		/* Chest */
		glTranslatef(-t->rect.w/2, -t->rect.h/2, 0);
		glScalef(t->rect.w, t->rect.w, 0);
		fillpolygon(t->skin[1], tofloatcolor(t->color), 1);

		/* Head */
		glTranslatef(0, -0.5f, 0);
		fillpolygon(t->skin[0], tofloatcolor(t->headcolor), 1);

		/* Legs */
		glPushMatrix();
		glTranslatef(0.3f, 1.5f, 0);
		if(t->left){
			glRotatef(30 + (int)(t->rect.x * -30) % 90, 0, 0, 1);
		}
		else{
			glRotatef(-60 + (int)(t->rect.x * 30) % 90, 0, 0, 1);
		}
		glScalef(0.25f, 0.75f, 0);
		glTranslatef(-0.2f, -.2f, 0);
		fillpolygon(t->skin[3], tofloatcolor(t->footcolor), 1);

		glPopMatrix();
		/* Arm */
		glTranslatef(0, 0.6f, 0);
		glRotatef(t->tick, 0, 0, 1);
		glScalef(0.3f, 0.7f, 0);
		fillpolygon(t->skin[2], tofloatcolor(t->armcolor), 1);

		/* Name */
		temp.w = 1;
		temp.h = 1.5f;
		glPopMatrix();
		glTranslatef(strlen(t->name)/-temp.w/2, -t->rect.h - 1.0f, 0);
		glEnable(GL_TEXTURE_2D);
		drawtext(&temp, t->name, 1, 1, 1, 0.8f);
		glDisable(GL_TEXTURE_2D);
	}

	glPopMatrix();
	glPushMatrix();
	for(i=0; i<level.MAX_BOXES; ++i){
		if(level.boxes[i].owner){
			glTranslatef(level.boxes[i].rect.x, level.boxes[i].rect.y, 0);
			glScalef(level.boxes[i].rect.w, level.boxes[i].rect.h, 0);
			fillrect(0, 1, 0, 0.5f);
			glPopMatrix();
			glPushMatrix();
			if(level.boxes[i].tick < level.boxes[i].maxdelay){
				glTranslatef(level.boxes[i].rect.x, level.boxes[i].rect.y, 0);
				glPushMatrix();
				glScalef(1.0f, level.boxes[i].tick / 20.0f, 0);
				fillrect(1, 1, 0, 0.5f);
				glPopMatrix();
				glTranslatef(0, level.boxes[i].mindelay / 20.0f, 0);
				fillrect(1, 0, 0, 0.5f);
			}
		}
	}

	for(i=0; i<MAXPARTICLES; ++i){
		if(particles[i].time > 0){
			glTranslatef(particles[i].x, particles[i].y, 0.0f);
			glRotatef(particles[i].time * 10, 0, 0, 1);
			glScalef(particles[i].size, particles[i].size, 0);
			filltriangle(tofloatcolor(particles[i].color), particles[i].time / 128.0f);
			glPopMatrix();
			glPushMatrix();
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	temp.w = camera.width / 40;
	temp.x = temp.w/2;
	temp.y = temp.w/2;
	temp.h = temp.w * 1.5f;
	glEnable(GL_TEXTURE_2D);
	/* text */
	for(i=0; i<PLAYERS; ++i){
		disp[2] = '0' + ((int)fighters[i]->damage) % 10;
		disp[1] = '0' + (((int)fighters[i]->damage) % 100) / 10;
		disp[0] = '0' + (int)fighters[i]->damage / 100;
		if(disp[0] == '0'){
			disp[0] = ' ';
			if(disp[1] == '0'){
				disp[1] = ' ';
			}
		}
		drawtext(&temp, disp, 1, 1, 0, 1);
		glTranslatef(temp.w, 0, 0);
	}
	glDisable(GL_TEXTURE_2D);

	SDL_GL_SwapWindow(gwin);
}

void setViewport(){
	glViewport(0.0f, 0.0f, camera.swidth, camera.sheight);
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

UDPsocket sd;
UDPpacket *p;
char *server = NULL;
IPaddress serveradd;
int port = 0;
void initNetwork(){
	if(SDLNet_Init() < 0){
		SDL_Log("Error initializing SDLNet: %s\n", SDLNet_GetError());
	}
	if(!(sd = SDLNet_UDP_Open(3000))){
		SDL_Log("Error SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	}
	if(!(p = SDLNet_AllocPacket(128))){
		SDL_Log("Error SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	}
	if(port && server){
		if(SDLNet_ResolveHost(&serveradd, server, port) == -1){
			SDL_Log("Error SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		}
		p->channel = 0;
	}
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
	Uint8 *skin = malloc(sizeof(Uint8) * 4 * MAXPLAYERS);
	gjoy = malloc(sizeof(*gjoy) * MAXPLAYERS);


	check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) >= 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
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
		if(strcmp(argv[i + 1], "-level") == 0){
			SDL_Log("Loading level");
			cfighter = NULL;
			lrunscript(argv[i + 2]);
			SDL_Log("Loaded level");
		}
		else if(strcmp(argv[i + 1], "-server") == 0){
			server = argv[i + 2];
			++i;
		}
		else if(strcmp(argv[i + 1], "-p") == 0){
			port = atoi(argv[i + 2]);
			++i;
		}
		else{
			gjoy[i] = SDL_JoystickOpen(i);
			fighters[i] = tfighter_new(34 + i * 2, 10, 0x775500FF, (i <= 1) ?  c[i] : NULL, b, i, SDL_JoystickGetAxis(gjoy[i], 0), SDL_JoystickGetAxis(gjoy[i], 1), &skin[i*3]);
			cfighter = fighters[i];
			PLAYERS = i + 1;
			lua_pushnumber(l, rand());
			lua_setglobal(l, "seed");	
			lrunscript(argv[i + 1]);

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

	initNetwork();
	
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

			if(port){
				p->address.host = serveradd.host;
				p->address.port = serveradd.port;
				p->channel = 0;
				*p->data = (Uint8)fighters[p->channel]->input;
				p->len = sizeof(Uint8);
				SDLNet_UDP_Send(sd, p->channel, p);
			}
			while(SDLNet_UDP_Recv(sd, p)){
				fighters[p->channel]->input = (Uint8)*p->data;
			}

			for(i=0; i<PLAYERS; ++i){
				tfighter_update(fighters[i], &level);
				if(fighters[i]->state & JUMP){
					float mag = (float)(rand() % 1000 / 1000.0f);
					Uint32 color = rand() % 0xFF;
					color = 0x55 + color * 0x100 + color * 0x10000;
					tparticle_set(&particles[cparticle], fighters[i]->rect.x + ((rand() % 1000) / 1000.0) * fighters[i]->rect.w, fighters[i]->rect.y + ((rand() % 1000) / 1000.0) * fighters[i]->rect.h, fighters[i]->vx * mag, .2f*mag, (rand() % 100) / 100.0f + 0.05f, 90, color);
					cparticle = (cparticle + 1) % MAXPARTICLES;
				}
				else if(fighters[i]->state & HITSTUN){
					float mag = (float)(rand() % 1000 / 1000.0f);
					Uint32 color = rand() % 0xFF;
					color = 0x55 + color * 0x100 + color * 0x10000 + color * 0x1000000;
					tparticle_set(&particles[cparticle], fighters[i]->rect.x + ((rand() % 1000) / 1000.0) * fighters[i]->rect.w, fighters[i]->rect.y + ((rand() % 1000) / 1000.0) * fighters[i]->rect.h, fighters[i]->vx * mag, fighters[i]->vy * mag, (rand() % 100) / 100.0f + 0.05f, 90, color);
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

	SDLNet_FreePacket(p);
	SDLNet_Quit();

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
