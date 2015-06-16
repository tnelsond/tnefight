#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

int intersects(trect *r, trect *o){
	return r->x + r->w > o->x && r->x < o->x + o->w 
			&& r->y + r->h > o->y && r->y < o->y + o->h;
}

tfighter *tfighter_new(double x, double y){
	tfighter *ret = malloc(sizeof(tfighter));
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 2;
	ret->vx = 0;
	ret->vy = 0;
	ret->accel = 0.1f;
	ret->speed = 1.0f;
	ret->gravity = 0.05f;
	return ret;
}

void tfighter_update(tfighter *t, trect blocks[]){
	int i;
	t->rect.y += t->vy += t->gravity;
	t->rect.x += t->vx;
	t->vx *= 0.94;
	for(i=sizeof(blocks); i>=0; --i){
		if(intersects(&t->rect, &blocks[i])){
			t->vy = 0;	
			t->rect.y = blocks[i].y - t->rect.h;
		}
	}
	/*hitbox *b = t->box;
	while(b){
		b->rect.x += b->vx += b->ax;
		b->rect.y += b->vy += b->ay;
		b = b->next;
	}
	*/
}
