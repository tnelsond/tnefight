#include <SDL2/SDL.h>

#define ATTACK (1 << 0)
#define BLOCK (1 << 1)
#define MOVEMENT (1 << 2)
#define REFLECT (1 << 3)
#define PROJECTILE (1 << 4)

struct hitbox{
	SDL_Rect rect;
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
	SDL_Rect rect;
	float vx, vy;
	float speed;
	float accel;
	float attack;
	float defense;
	float jump;
	float gravity;
	hitbox *box;
} tfighter;

tfighter tfighter_new(tfighterconf conf);

void tfighter_draw(SDL_Surface *surf, tfighter *t);
void tfighter_update(tfighter *t);

int intersect(SDL_Rect r, SDL_Rect o);
