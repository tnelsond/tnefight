#include <stdlib.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

int gid = 1;

int intersects(trect *r, trect *o){
	return r->x + r->w > o->x && r->x < o->x + o->w 
			&& r->y + r->h > o->y && r->y < o->y + o->h;
}

int project(tcamera *tc, trect *t, SDL_Rect *r){
	r->x = (t->x - tc->x) * tc->scale;
	r->y = (t->y - tc->y) * tc->scale;
	r->w = t->w * tc->scale;
	r->h = t->h * tc->scale;
}

void tcamera_track(tcamera *tc, trect *a, trect *b){
	float width = (a->x + a->w/2) - (b->x + b->w/2);
	if(width < 0){
		width = -width;
	}
	float height = (a->y + a->h/2) - (b->y + b->h/2);
	if(height < 0){
		height = -height;
	}

	float x = ((a->x + a->w/2) + (b->x + b->w/2)) / 2;
	float y = ((a->y + a->h/2) + (b->y + b->h/2)) / 2;
	tc->scale = tc->swidth / (width > height ? width + 20 : height + 20);
	tc->cx = x;
	tc->cy = y;
	tc->x = tc->cx - tc->swidth/tc->scale/2;
	tc->y = tc->cy - tc->sheight/tc->scale/2;
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

tfighter *tfighter_new(float x, float y, int red, int green, int blue, SDL_Keycode *keys){
	tfighter *ret = malloc(sizeof(tfighter));
	ret->tick = 0;
	ret->red = red;
	ret->green = green;
	ret->blue = blue;
	ret->state = 0;
	ret->keys = keys;
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 2;
	ret->jump = 0.7f;
	ret->vx = 0;
	ret->vy = 0;
	ret->accel = 0.03f;
	ret->speed = 1.0f;
	ret->gravity = 0.03f;
	ret->id = gid;
	gid = gid << 1;

	ret->moves = malloc(sizeof(hitbox)*3);
	ret->moves[0].rect.x = 0;
	ret->moves[0].rect.y = 0.7f;
	ret->moves[0].rect.w = 1.0f;
	ret->moves[0].rect.h = 0.4f;
	ret->moves[0].vx = 0.3f;
	ret->moves[0].lag = 30;
	ret->moves[0].vy = 0.0f;
	ret->moves[0].ax = 0.00f;
	ret->moves[0].ay = 0.00f;
	ret->moves[0].aw = 0.00f;
	ret->moves[0].ah = 0.00f;
	ret->moves[0].vw = 0.6f;
	ret->moves[0].xknockback = 0.2f;
	ret->moves[0].yknockback = 4.5f;
	ret->moves[0].maxtime = 20;
	ret->moves[0].mintime = 10;
	ret->moves[0].type = ATTACK;
	ret->moves[0].owner = ret;
	ret->moves[0].tick = 0;
	ret->moves[0].delay = 10;
	ret->moves[0].left = 1;
	ret->moves[0].hit = 0;

	ret->moves[1].rect.x = 1.0f;
	ret->moves[1].rect.y = 0.5f;
	ret->moves[1].rect.w = 1.0f;
	ret->moves[1].rect.h = 1.0f;
	ret->moves[1].vx = 0.5f;
	ret->moves[1].lag = 30;
	ret->moves[1].vy = 0.5f;
	ret->moves[1].ax = 0.00f;
	ret->moves[1].ay = 0.01f;
	ret->moves[1].aw = 0.00f;
	ret->moves[1].ah = 0.00f;
	ret->moves[1].vw = 0.0f;
	ret->moves[1].xknockback = 0.2f;
	ret->moves[1].yknockback = 4.5f;
	ret->moves[1].maxtime = 20;
	ret->moves[1].mintime = 10;
	ret->moves[1].type = ATTACK;
	ret->moves[1].owner = ret;
	ret->moves[1].tick = 0;
	ret->moves[1].delay = 10;
	ret->moves[1].left = 1;
	ret->moves[1].hit = 0;

	ret->moves[2].rect.x = 1.0f;
	ret->moves[2].rect.y = 1.0f;
	ret->moves[2].rect.w = 1.0f;
	ret->moves[2].rect.h = 0.5f;
	ret->moves[2].vx = 0.0f;
	ret->moves[2].lag = 25;
	ret->moves[2].vy = -0.5f;
	ret->moves[2].ax = 0.00f;
	ret->moves[2].ay = 0.01f;
	ret->moves[2].aw = 0.00f;
	ret->moves[2].ah = 0.00f;
	ret->moves[2].vw = 0.0f;
	ret->moves[2].xknockback = 0.0f;
	ret->moves[2].yknockback = 6.5f;
	ret->moves[2].maxtime = 20;
	ret->moves[2].mintime = 10;
	ret->moves[2].type = ATTACK | MOVEMENT;
	ret->moves[2].owner = ret;
	ret->moves[2].tick = 0;
	ret->moves[2].delay = 0;
	ret->moves[2].left = 1;
	ret->moves[2].hit = 0;

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
		h->owner = NULL;
		return;
	}

	h->rect.w += (h->vw += h->aw);
	h->rect.h += (h->vh += h->ah);
	if(h->left){
		h->rect.x -= h->vw;
	}
	/*h->vx += h->ax + (h->left ? -h->vw/2 : h->vw/2);*/
	h->vx += h->ax;
	h->vy += h->ay;
	h->rect.x += h->vx;
	h->rect.y += h->vy + h->vh / 2;
}

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest){
	trect *offset = &t->rect;

	if(t->left){
		dest->rect.x = -src->rect.x - src->rect.w/2 + offset->x + offset->w/2;
	}
	else{
		dest->rect.x = src->rect.x + offset->x - src->rect.w/2 + offset->w/2;
	}

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
	dest->xknockback = src->xknockback;
	dest->yknockback = src->yknockback;
	dest->owner = src->owner;
	dest->hit = 0;

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
				if((1 << i) != ATTACKING){
					t->state = t->state & ~(1 << i);
				}
			}
		}
	}
	if((~prevstate & JUMP) && (t->state & JUMP)){
		t->vy = -t->jump;
	}
	if((~prevstate & ATTACKING) && (t->state & ATTACKING)){
		if(t->state & DOWN){
			tlevel_add_hitbox(tl, t, &t->moves[1]);
			t->tick = t->moves[0].lag;
		}
		else if(t->state & UP){
			tlevel_add_hitbox(tl, t, &t->moves[2]);
			t->tick = t->moves[2].lag;
		}
		else{
			tlevel_add_hitbox(tl, t, &t->moves[0]);
			t->tick = t->moves[1].lag;
		}
	}
}

void tfighter_update(tfighter *t, tlevel *tl){
	int i;
	if(t->state & ATTACKING){
		--t->tick;
		if(t->tick <= 0){
			t->state = t->state - ATTACKING;
		}
	}
	else if(t->state & LEFT){
		t->vx -= t->accel;
		t->left = 1;
	}
	else if(t->state & RIGHT){
		t->vx += t->accel;
		t->left = 0;
	}
	else if(t->state & DOWN){
		t->vy += t->accel;
	}

	for(i=0; i<tl->MAX_BOXES; ++i){
		tfighter *owner = tl->boxes[i].owner;
		if(owner && owner != t && !(tl->boxes[i].hit & t->id) && intersects(&t->rect, &tl->boxes[i].rect)){
			t->vy -= 0.1f * tl->boxes[i].yknockback;
			t->vx += tl->boxes[i].xknockback * (tl->boxes[i].left ? -1 : 1);
			tl->boxes[i].hit |= t->id;
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
			else if(tl->blocks[i].h >= 0.9f){
				t->vy = 0;	
				t->rect.y = tl->blocks[i].y + tl->blocks[i].h;
			}
		}
	}
	t->vx *= 0.94;
	t->rect.x += t->vx;
	for(i=0; i < tl->len; ++i){
		if(tl->blocks[i].h >= 1.0f){
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
}

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h){
	hitbox_spawn(t, h, &tl->boxes[tl->cbox]);
	tl->cbox = (tl->cbox + 1) % tl->MAX_BOXES;
	if(h->type & MOVEMENT){
		t->vy = h->vy;
		t->vx = h->vx;
	}
}
