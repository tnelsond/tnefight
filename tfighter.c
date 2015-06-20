#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

int gid = 1;

int intersects(trect *r, trect *o){
	return r->x + r->w > o->x && r->x < o->x + o->w 
			&& r->y + r->h > o->y && r->y < o->y + o->h;
}

tlevel *tlevel_new(int len){
	tlevel *tl = malloc(sizeof(tlevel));
	tl->blocks = malloc(sizeof(trect)*len);
	tl->len = len;
}

void tlevel_free(tlevel *tl){
	free(tl->blocks);
	free(tl);
	tl = NULL;
}

tfighter *tfighter_new(float x, float y, SDL_Keycode *keys){
	tfighter *ret = malloc(sizeof(tfighter));
	ret->state = 0;
	ret->keys = keys;
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 2;
	ret->jump = 0.7f;
	ret->vx = 0;
	ret->vy = 0;
	ret->accel = 0.01f;
	ret->speed = 1.0f;
	ret->gravity = 0.05f;
	ret->id = gid++;
	ret->moves = malloc(sizeof(hitbox));
	ret->moves->rect.x = 2;
	ret->moves->rect.y = 0.1f;
	ret->moves->rect.w = 1.0f;
	ret->moves->rect.h = 0.4f;
	ret->moves->vx = 0.0f;
	ret->moves->vy = 0.0f;
	ret->moves->ax = 0.00f;
	ret->moves->ay = 0.00f;
	ret->moves->aw = 0.00f;
	ret->moves->ah = 0.00f;
	ret->moves->vw = 0.2f;
	ret->moves->maxtime = 20;
	ret->moves->mintime = 10;
	ret->moves->type = ATTACK;
	ret->moves->owner = ret->id;
	ret->moves->tick = 0;
	ret->moves->delay = 10;
	ret->moves->left = 1;
	ret->left = 1;
	return ret;
}

void tfighter_free(tfighter *t){
	free(t->moves);
	free(t);
	t = NULL;
}

void hitbox_update(hitbox *h){
	++h->tick;
	if(h->tick < h->delay)
		return;
	if(h->tick > h->maxtime){
		h->owner = 0;
		return;
	}

	h->rect.w += (h->vw += h->aw);
	h->rect.h += (h->vh += h->ah);
	/*h->vx += h->ax + (h->left ? -h->vw/2 : h->vw/2);*/
	h->vx += (h->left ? -h->vw/4 : 0);
	h->vy += h->ay;
	h->rect.x += h->vx;
	h->rect.y += h->vy + h->vh / 2;
}

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest){
	trect *offset = &t->rect;
	dest->rect.x = src->rect.x * (t->left ? -1 : 1) + offset->x + offset->w/2;
	dest->rect.y = src->rect.y + offset->y;

	dest->rect.w = src->rect.w;
	dest->rect.h = src->rect.h;

	dest->left = t->left;

	dest->vx = src->vx * (t->left ? -1 : 1);
	dest->vy = src->vy;
	dest->ax = src->ax * (t->left ? -1 : 1);
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
}

void tfighter_input(tfighter *t, tlevel *tl, int keydown, SDL_Keycode key){
	int i;
	int prevstate = t->state;
	for(i=0; i<6; ++i){
		if(key == t->keys[i]){
			if(keydown){
				t->state = t->state | (1 << i);
			}
			else{
				t->state = t->state & ~(1 << i);
			}
		}
	}
	if((~prevstate & JUMP) && (t->state & JUMP)){
		t->vy = -t->jump;
	}
	if((~prevstate & ATTACKING) && (t->state & ATTACKING)){
		tlevel_add_hitbox(tl, t, t->moves);
	}
}

void tfighter_update(tfighter *t, tlevel *tl){
	int i;
	if(t->state & LEFT){
		t->vx -= t->accel;
		t->left = 1;
	}
	else if(t->state & RIGHT){
		t->vx += t->accel;
		t->left = 0;
	}
	if(t->state & DOWN){
		t->vy += t->accel;
	}

	for(i=0; i<tl->MAX_BOXES; ++i){
		int owner = tl->boxes[i].owner;
		if(owner && owner != t->id && intersects(&t->rect, &tl->boxes[i].rect)){
			t->vy -= .2f;
			t->vx += tl->boxes[i].vx;
		}
	}

	t->vy += t->gravity;
	t->rect.y += t->vy;
	for(i=0; i < tl->len; ++i){
		if(intersects(&t->rect, &tl->blocks[i])){
			if(t->vy > 0){
				t->vy = 0;	
				t->rect.y = tl->blocks[i].y - t->rect.h;
			}
			else{
				t->vy = 0;	
				t->rect.y = tl->blocks[i].y + tl->blocks[i].h;
			}
		}
	}
	t->vx *= 0.94;
	t->rect.x += t->vx;
	for(i=0; i < tl->len; ++i){
		if(intersects(&t->rect, &tl->blocks[i])){
			if(t->vx > 0){
				t->vx = 0;	
				t->rect.x = tl->blocks[i].x - t->rect.w;
			}
			else{
				t->vx = 0;
				t->rect.x = tl->blocks[i].x + tl->blocks[i].w;
			}
		}
	}
}

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h){
	hitbox_spawn(t, h, &tl->boxes[tl->cbox]);
	tl->cbox = (tl->cbox + 1) % tl->MAX_BOXES;
}
