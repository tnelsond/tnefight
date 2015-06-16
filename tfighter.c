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
	ret->id = 1;
	ret->moves = malloc(sizeof(hitbox));
	ret->moves->rect.x = 0.4f;
	ret->moves->rect.y = 0.1f;
	ret->moves->rect.w = 1.0;
	ret->moves->rect.h = 2.0;
	ret->moves->vx = 0.5f;
	ret->moves->vy = 0.1f;
	ret->moves->ax = -0.01f;
	ret->moves->ay = 0.003f;
	ret->moves->maxtime = 20;
	ret->moves->mintime = 10;
	ret->moves->type = ATTACK;
	ret->moves->owner = ret->id;
	ret->moves->tick = 0;
	ret->moves->delay = 0;
	return ret;
}

void tfighter_free(tfighter *t){
	free(t->moves);
	free(t);
}

void hitbox_update(hitbox *h){
	++h->tick;
	if(h->tick < h->delay)
		return;
	if(h->tick > h->maxtime){
		h->owner = 0;
		return;
	}
	
	h->rect.x += h->vx += h->ax;
	h->rect.y += h->vy += h->ay;
	h->rect.w += h->vw += h->aw;
	h->rect.h += h->vh += h->ah;
}

void hitbox_clone(trect *offset, hitbox *src, hitbox *dest){
	dest->rect.x = src->rect.x + offset->x;
	dest->rect.y = src->rect.y + offset->y;

	dest->rect.w = src->rect.w;
	dest->rect.h = src->rect.h;

	dest->vx = src->vx;
	dest->vy = src->vy;
	dest->ax = src->ax;
	dest->ay = src->ay;

	dest->vw = src->vw;
	dest->vh = src->vh;
	dest->aw = src->aw;
	dest->ah = src->ah;

	dest->attack = src->attack;
	dest->knockback = src->knockback;
	dest->owner = src->owner;

	dest->delay = src->delay;
	dest->mintime = src->mintime;
	dest->maxtime = src->maxtime;

	dest->type = src->type;
	dest->tick = 0;
	dest->next = NULL; /* FIX THIS LATER */
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
}
