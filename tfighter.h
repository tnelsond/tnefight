#include <SDL2/SDL.h>

#define ATTACK (1 << 0)
#define BLOCK (1 << 1)
#define MOVEMENT (1 << 2)
#define REFLECT (1 << 3)
#define PROJECTILE (1 << 4)

typedef struct{
	double x, y, w, h;
} trect;

struct hitbox{
	trect rect;
	float vx, vy, ax, ay; /* Location change variables */
	float vw, vh, aw, ah; /* Size change variables */
	float attack, knockback;
	char owner; /* ID of the owner */
	struct hitbox *next;
};

typedef struct hitbox hitbox;

typedef struct{
	hitbox *hitboxes;
	int delay, mintime, maxtime; /* In Milliseconds */
	int type; 
} movebase;

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
	float gravity;
	movebase *moves[];
} tfighter;

tfighter *tfighter_new(double x, double y);

void tfighter_update(tfighter *t, trect blocks[]);

int intersects(trect *r, trect *o);
