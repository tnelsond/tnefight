#include <SDL2/SDL.h>
#include <math.h>

/* hitbox type */
#define ATTACK (1 << 0)
#define BLOCK (1 << 1)
#define MOVEMENT (1 << 2)
#define REFLECT (1 << 3)
#define PROJECTILE (1 << 4)
#define AIRONCE (1 << 5)

/* tfighter state */
#define LEFT (1 << 0)
#define RIGHT (1 << 1)
#define UP (1 << 2)
#define DOWN (1 << 3)
#define JUMP (1 << 4)
#define ATTACKING (1 << 5)
#define STUNNED (1 << 6)

#define NUMKEYS 5;
#define PI 3.14159265358979323846 

typedef struct{
	float x, y, w, h;
} trect;

typedef struct{
	float px, py, x, y, cx, cy, pscale, scale;
	int swidth, sheight;
} tcamera;

typedef struct tfighter tfighter;

struct hitbox{
	trect rect, prect;
	float vx, vy, ax, ay; /* Location change variables */
	float vw, vh, aw, ah; /* Size change variables */
	int attack;
	float kb;
	float kbgrowth;
	double kbangle;
	tfighter *owner;
	char hit;
	char left;
	int lag, delay, mintime, maxtime; /* In Frames */
	int type; 
	int tick;
	char usable;
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
	float speed;
	float accel;
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
	int tick;
	int state;
	char usable;
	int damage;
};

tfighter *tfighter_new(float x, float y, int red, int green, int blue, SDL_Keycode *keys);

void tfighter_free(tfighter *t);

void tfighter_update(tfighter *t, tlevel *tl);

void tfighter_input(tfighter *t, tlevel *tl, int down, SDL_Keycode key);

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
