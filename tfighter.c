#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

int intersect(trect *r, trect *o){
	return r->x + r->w > o->x && r->x < o->x + o->w 
			&& r->y + r->h > o->y && r->y < o->y + o->h;
}

tfighter *tfighter_new(float x, float y){
	tfighter *ret = malloc(sizeof(tfighter));
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 2;
	return ret;
}

void tfighter_update(tfighter *t){
	/*hitbox *b = t->box;
	while(b){
		b->rect.x += b->vx += b->ax;
		b->rect.y += b->vy += b->ay;
		b = b->next;
	}
	*/
}
