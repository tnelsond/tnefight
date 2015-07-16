#ifndef TFIGHTER_H
#define TFIGHTER_H

#include <SDL2/SDL.h>

/* hitbox type */
#define ATTACK (1 << 0)
#define BLOCK (1 << 1)
#define MOVEMENT (1 << 2)
#define REFLECT (1 << 3)
#define PROJECTILE (1 << 4)
#define AIRONCE (1 << 5)
#define SHIELD (1 << 6)

/* tfighter state */
#define LEFT (1 << 0)
#define RIGHT (1 << 1)
#define UP (1 << 2)
#define DOWN (1 << 3)
#define JUMP (1 << 4)
#define ATTACKING (1 << 5)
#define SPECIAL (1 << 6)
#define SHIELDING (1 << 7)
#define CHARGING (1 << 8)
#define HITSTUN (1 << 9)
#define HELPLESS (1 << 10)
#define GROUND (1 << 11)
#define WALKING (1 << 12)
#define RUNNING (1 << 13)

/* tfighter attack numbers */
#define OATTACK 0
#define UATTACK 1
#define DATTACK 3
#define NATTACK 2

/* Other Constants */
#define NUMKEYS 5;
#define PI 3.14159265358979323846 
#define JOYDEADZONE 8000

typedef struct{
	float x, y, w, h;
} trect;

typedef struct{
	float px, py, x, y, cx, cy, pscale, scale, bx, by, bw, bh;
	int swidth, sheight;
} tcamera;

typedef struct tfighter tfighter;

struct hitbox{
	trect rect, prect;
	float vx, vy, ax, ay; /* Location change variables */
	float vw, vh, aw, ah; /* Size change variables */
	int attack, maxattack;
	float kb;
	float kbgrowth;
	double kbangle;
	tfighter *owner;
	char hit;
	char left;
	int mindelay, maxdelay, time, endlag; /* In Frames */
	int type; 
	int tick;
	int hitlag;
	int image;
};

typedef struct hitbox hitbox;

typedef struct tlevel{
	trect *blocks;
	hitbox *boxes;
	int cbox;
	trect rect;
	int len;
	int MAX_BOXES;
} tlevel;

typedef struct{
	char bruiserness;
	char floatiness;
	char size;
} tfighterconf;

struct tfighter{
	trect rect, prect;
	float vx, vy;
	float walk, run, speed, accel;
	float attack;
	float defense;
	float jumpvel;
	int MAXJUMPS;
	int jump;
	float gravity;
	char id;
	char left;
	int red;
	int green;
	int blue;
	hitbox *moves;
	SDL_Keycode *keys;
	Uint32 *jbuttons;
	SDL_JoystickID joy;
	int joyxoffset;
	int joyyoffset;
	int tick;
	Uint32 state, pstate;
	int damage;
	int hitlag;
	Uint8 *skin;
};

tfighter *tfighter_new(float x, float y, int red, int green, int blue, SDL_Keycode *keys, Uint32 *joybuttons, SDL_JoystickID joy, int joyxoffset, int joyyoffset, Uint8 *skin);

void tfighter_free(tfighter *t);

void tfighter_update(tfighter *t, tlevel *tl);

void tfighter_input(tfighter *t, tlevel *tl, SDL_Event *e);

void hitbox_update(hitbox *h);

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest);

int intersects(trect *r, trect *o);

tlevel *tlevel_new(int len);

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h);

void tlevel_free(tlevel *tl);

void project(tcamera *tc, trect *t, trect *p, SDL_Rect *r, float alpha);
void project2(tcamera *tc, trect *t, SDL_Rect *r, float alpha);
void projecthud(tcamera *tc, SDL_Rect *r, float x, float y, float w, float h);

void tcamera_track(tcamera *tc, trect *a, trect *b);

#endif
