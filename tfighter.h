#include <SDL2/SDL.h>

/* hitbox type */
#define ATTACK (1 << 0)
#define BLOCK (1 << 1)
#define MOVEMENT (1 << 2)
#define REFLECT (1 << 3)
#define PROJECTILE (1 << 4)

typedef struct{
	float x, y, w, h;
} trect;

struct hitbox{
	trect rect;
	float vx, vy, ax, ay; /* Location change variables */
	float vw, vh, aw, ah; /* Size change variables */
	float attack, knockback;
	char owner; /* ID of the owner */
	char left;
	int delay, mintime, maxtime; /* In Frames */
	int type; 
	int tick;
};

typedef struct hitbox hitbox;

typedef struct tlevel{
	trect *blocks;
	hitbox *boxes;
	int cbox;
	float w;
	float h;
	int len;
	int MAX_BOXES;
} tlevel;

typedef struct{
	char bruiserness;
	char floatiness;
	char size;
} tfighterconf;

typedef struct{
	trect rect;
	float vx, vy;
	float speed;
	float accel;
	float attack;
	float defense;
	float jump;
	char id;
	float gravity;
	char left;
	hitbox *moves;
} tfighter;

tfighter *tfighter_new(float x, float y);

void tfighter_free(tfighter *t);

void tfighter_update(tfighter *t, tlevel *tl);

void hitbox_update(hitbox *h);

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest);

int intersects(trect *r, trect *o);

tlevel *tlevel_new(int len);

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h);

void tlevel_free(tlevel *tl);
