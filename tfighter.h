#ifndef TFIGHTER_H
#define TFIGHTER_H

#include <SDL2/SDL.h>

/* hitbox type */
#define ATTACK (1 << 0)
#define MOVEMENT (1 << 1)
#define REFLECT (1 << 2)
#define PROJECTILE (1 << 3)
#define AIRONCE (1 << 4)

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

/* trect type */
#define TRIUL 0
#define TRIDL 1
#define TRIDR 2
#define TRIUR 3
#define RECT 4

/* Other Constants */
#define NUMKEYS 5
#define MAXMOVES 9
#define PI 3.14159265358979323846 
#define JOYDEADZONE 8000
#define ATTACKPRECEDENCE 9
#define DAMPENING 0.9f
#define MAXSKIN 13
#define CAMERABORDER 20
#define KBRESISTANCE 0.03f
#define INTERSECT_TOLERANCE 0.001f

#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

#define tofloatcolor(color)\
	(float)((color >> 24) % (0x100)),\
	(float)((color >> 16) % (0x100)),\
	(float)((color >> 8) % (0x100))

#define terp(p, c, alpha) (p + (c - p) * alpha)
#define clamp(a, b, c) ((a) < (b) ? (b) : ((a) > (c) ? (c) : (a)))

#define settrect(t, a, b, c, d)\
	t.x = a;\
	t.y = b;\
	t.w = c;\
	t.h = d;

typedef struct{
	float px, py, x, y, size;
	int time;
	Uint32 color;
} tparticle;

void tparticle_set(tparticle *part, float x, float y, float vx, float vy, float size, int ttime, Uint32 color);
void tparticle_update(tparticle *part);

typedef struct{
	float x, y, w, h;
	char type;
} trect;

typedef struct{
	float px, py, x, y, width, height;
	int swidth, sheight;
	float ix, iy;
} tcamera;

typedef struct tfighter tfighter;

struct hitbox{
	trect rect, prect;
	float vx, vy, ax, ay; /* Location change variables */
	float vw, vh, aw, ah; /* Size change variables */
	int attack;
	float attackmultiply;
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
	float spawnx;
	float spawny;
} tlevel;

tlevel level;

typedef struct{
	char bruiserness;
	char floatiness;
	char size;
} tfighterconf;

struct tfighter{
	trect rect, prect;
	float vx, vy;
	float walk, run, speed, accel;
	float strength;
	float defense;
	float jumpvel;
	int MAXJUMPS;
	int jump;
	float gravity;
	float fallspeed, driftspeed;
	float launchresistance;
	char id;
	char left;
	Uint32 color;
	hitbox *moves;
	SDL_Keycode *keys;
	Uint32 *jbuttons;
	SDL_JoystickID joy;
	int joyxoffset;
	int joyyoffset;
	int tick;
	Uint32 state, pstate;
	float damage;
	int hitlag;
	Uint8 *skin;
	char *name;
};

tfighter *tfighter_new(float x, float y, Uint32 color, SDL_Keycode *keys, Uint32 *joybuttons, SDL_JoystickID joy, int joyxoffset, int joyyoffset, Uint8 *skin);

void tfighter_setmove(tfighter *t, int index, int attack, int kb, int kbgrowth, int mindelay, int maxdelay, int duration, int endlag, int x, int y, int width, int height, int angle, int kbangle, int speed, int type, int img);
void tfighter_balance_move(tfighter *t, int index, int attack, int kb, int chargetime, int duration, int endlag, int x, int y, int width, int height, int angle, int kbangle, int speed, int type, int img);

void tfighter_free(tfighter *t);

void tfighter_update(tfighter *t, tlevel *tl);

void tfighter_input(tfighter *t, tlevel *tl, SDL_Event *e);

void hitbox_update(hitbox *h);

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest);

int intersects(trect *r, trect *o, float xintersect, float yintersect);

tlevel *tlevel_new(int len);

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h);

void tlevel_free(tlevel *tl);

void tcamera_track(tcamera *tc, tlevel *tl, tfighter **t, int len);
void tcamera_interpolate(tcamera *tc, float alpha);

#endif
