#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

#define terp(p, c, alpha) (p + (c - p) * alpha)

int gid = 1;

int intersects(trect *r, trect *o){
	return r->x + r->w > o->x && r->x < o->x + o->w 
			&& r->y + r->h > o->y && r->y < o->y + o->h;
}

void project(tcamera *tc, trect *t, trect *p, SDL_Rect *r, float alpha){
	r->x = (int)((terp(p->x, t->x, alpha) - terp(tc->px, tc->x, alpha)) * terp(tc->pscale, tc->scale, alpha) + 0.5f);
	r->y = (int)((terp(p->y, t->y, alpha) - terp(tc->py, tc->y, alpha)) * terp(tc->pscale, tc->scale, alpha) + 0.5f);
	r->w = (int)((terp(p->w, t->w, alpha) * terp(tc->pscale, tc->scale, alpha) + 0.5f));
	r->h = (int)((terp(p->h, t->h, alpha) * terp(tc->pscale, tc->scale, alpha) + 0.5f));
}

void project2(tcamera *tc, trect *t, SDL_Rect *r, float alpha){
	r->x = (int)((t->x - terp(tc->px, tc->x, alpha)) * terp(tc->pscale, tc->scale, alpha) + 0.5f);
	r->y = (int)((t->y - terp(tc->py, tc->y, alpha)) * terp(tc->pscale, tc->scale, alpha) + 0.5f);
	r->w = (int)(t->w * terp(tc->pscale, tc->scale, alpha) + 0.5f);
	r->h = (int)(t->h * terp(tc->pscale, tc->scale, alpha) + 0.5f);
}

void projecthud(tcamera *tc, SDL_Rect *r, float x, float y, float w, float h){
	r->x = (int)(x * tc->swidth + 0.5f);
	r->y = (int)(y * tc->swidth + 0.5f);
	r->w = (int)(w * tc->swidth + 0.5f);
	r->h = (int)(h * tc->swidth + 0.5f);
}

void tcamera_track(tcamera *tc, trect *a, trect *b){
	float height, width, x, y;
	tc->px = tc->x;
	tc->py = tc->y;
	tc->pscale = tc->scale;
	width = (a->x + a->w/2) - (b->x + b->w/2);
	if(width < 0){
		width = -width;
	}
	height = (a->y + a->h/2) - (b->y + b->h/2);
	if(height < 0){
		height = -height;
	}

	x = ((a->x + a->w/2) + (b->x + b->w/2)) / 2;
	y = ((a->y + a->h/2) + (b->y + b->h/2)) / 2;
	tc->scale = tc->swidth / (width > height ? width + 20 : height + 20);
	tc->cx = x;
	tc->cy = y;
	tc->x = tc->cx - tc->swidth/tc->scale/2;
	tc->y = tc->cy - tc->sheight/tc->scale/2;

	if(tc->scale > tc->bw/2){
		tc->scale = tc->bw/2;
	}
	if(tc->scale > tc->bh/2){
		tc->scale = tc->bh/2;
	}

	if(tc->x < tc->bx){
		tc->x = tc->bx;
		tc->cx = tc->x + tc->scale;
	}
	if(tc->y < tc->by){
		tc->y = tc->by;
		tc->cy = tc->y + tc->scale;
	}
	if(tc->cy + tc->scale > tc->bh){
		tc->cy = tc->bh - tc->scale;
		tc->y = tc->cy - tc->scale;
	}
	if(tc->cx + tc->scale > tc->bw){
		tc->cx = tc->bw - tc->scale;
		tc->x = tc->cx - tc->scale;
	}
}

tlevel *tlevel_new(int len){
	tlevel *tl = malloc(sizeof(tlevel));
	tl->blocks = malloc(sizeof(trect)*len);
	tl->len = len;
	return tl;
}

void tlevel_free(tlevel *tl){
	free(tl->blocks);
	free(tl);
	tl = NULL;
}

tfighter *tfighter_new(float x, float y, int red, int green, int blue, SDL_Keycode *keys, Uint32 *joybuttons, SDL_JoystickID joy, int joyxoffset, int joyyoffset, int imgindex){
	tfighter *ret = malloc(sizeof(tfighter));
	ret->imgindex = imgindex;
	ret->joyxoffset = joyxoffset;
	ret->joyyoffset = joyyoffset;
	ret->tick = 0;
	ret->damage = 0;
	ret->red = red;
	ret->green = green;
	ret->blue = blue;
	ret->state = 0;
	ret->pstate = 0;
	ret->keys = keys;
	ret->prect.x = x;
	ret->prect.y = y;
	ret->prect.w = 2;
	ret->prect.h = 2;
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 2;
	ret->jumpvel = 0.7f;
	ret->MAXJUMPS = 2;
	ret->jump = 2;
	ret->vx = 0;
	ret->vy = 0;
	ret->accel = 0.08f;
	ret->walk = 0.1f;
	ret->run = 0.3f;
	ret->speed = 0.3f;
	ret->gravity = 0.03f;
	ret->hitlag = 0;
	ret->id = gid;
	ret->joy = joy;
	ret->jbuttons = joybuttons;
	gid = gid << 1;

	ret->moves = malloc(sizeof(hitbox)*9);

	/* Over-Attack */
	ret->moves[0].rect.x = 0;
	ret->moves[0].rect.y = 0.7f;
	ret->moves[0].rect.w = 1.0f;
	ret->moves[0].rect.h = 1.4f;
	ret->moves[0].vx = 0.2f;
	ret->moves[0].vy = 0.0f;
	ret->moves[0].ax = 0.00f;
	ret->moves[0].ay = 0.00f;
	ret->moves[0].aw = 0.00f;
	ret->moves[0].ah = 0.00f;
	ret->moves[0].vw = 0.1f;
	ret->moves[0].vh = 0.0f;
	ret->moves[0].kb = 6.0f;
	ret->moves[0].kbgrowth = 0.0f;
	ret->moves[0].kbangle = -30.0;
	ret->moves[0].attack = 5;
	ret->moves[0].maxattack = 30;
	ret->moves[0].time = 15;
	ret->moves[0].type = ATTACK;
	ret->moves[0].owner = ret;
	ret->moves[0].tick = 0;
	ret->moves[0].mindelay = 10;
	ret->moves[0].maxdelay = 400;
	ret->moves[0].left = 1;
	ret->moves[0].hit = 0;
	ret->moves[0].endlag = 20;
	/* Up-Attack */
	ret->moves[1].rect.x = 0.0f;
	ret->moves[1].rect.y = 0.0f;
	ret->moves[1].rect.w = 2.0f;
	ret->moves[1].rect.h = 1.0f;
	ret->moves[1].vx = 0.0f;
	ret->moves[1].vy = -0.5f;
	ret->moves[1].ax = 0.00f;
	ret->moves[1].ay = 0.01f;
	ret->moves[1].aw = 0.00f;
	ret->moves[1].ah = 0.00f;
	ret->moves[1].vw = 0.1f;
	ret->moves[1].vh = 0.1f;
	ret->moves[1].kb = 2.0f;
	ret->moves[1].kbgrowth = 1.0f;
	ret->moves[1].kbangle = -90.0;
	ret->moves[1].attack = 9;
	ret->moves[1].maxattack = 30;
	ret->moves[1].time = 30;
	ret->moves[1].type = ATTACK;
	ret->moves[1].owner = ret;
	ret->moves[1].tick = 0;
	ret->moves[1].mindelay = 30;
	ret->moves[1].maxdelay = 150;
	ret->moves[1].left = 1;
	ret->moves[1].hit = 0;
	ret->moves[1].endlag = 30;

	/* Neutral-Attack */
	ret->moves[2].rect.x = 0.5f;
	ret->moves[2].rect.y = 0.0f;
	ret->moves[2].rect.w = 1.0f;
	ret->moves[2].rect.h = 1.0f;
	ret->moves[2].vx = 0.2f;
	ret->moves[2].vy = 0.0f;
	ret->moves[2].ax = -0.01f;
	ret->moves[2].ay = 0.00f;
	ret->moves[2].aw = 0.00f;
	ret->moves[2].ah = 0.00f;
	ret->moves[2].vw = 0.0f;
	ret->moves[2].vh = 0.0f;
	ret->moves[2].kb = 5.0f;
	ret->moves[2].kbgrowth = 1.5f;
	ret->moves[2].kbangle = -45.0;
	ret->moves[2].attack = 15;
	ret->moves[2].maxattack = 30;
	ret->moves[2].time = 15;
	ret->moves[2].type = ATTACK;
	ret->moves[2].owner = ret;
	ret->moves[2].tick = 0;
	ret->moves[2].mindelay = 4;
	ret->moves[2].maxdelay = 80;
	ret->moves[2].endlag = 10;
	ret->moves[2].left = 1;
	ret->moves[2].hit = 0;

	/* Down-Attack */
	ret->moves[3].rect.x = 2.0f;
	ret->moves[3].rect.y = 0.5f;
	ret->moves[3].rect.w = 1.0f;
	ret->moves[3].rect.h = 1.0f;
	ret->moves[3].vx = 0.3f;
	ret->moves[3].vy = 0.1f;
	ret->moves[3].ax = -0.1f;
	ret->moves[3].ay = 0.00f;
	ret->moves[3].aw = 0.00f;
	ret->moves[3].ah = 0.00f;
	ret->moves[3].vw = 0.0f;
	ret->moves[3].vh = 0.0f;
	ret->moves[3].kb = 0.1f;
	ret->moves[3].kbgrowth = 0.5f;
	ret->moves[3].kbangle = -80.0;
	ret->moves[3].attack = 6;
	ret->moves[3].maxattack = 30;
	ret->moves[3].time = 12;
	ret->moves[3].type = ATTACK;
	ret->moves[3].owner = ret;
	ret->moves[3].tick = 0;
	ret->moves[3].mindelay = 30;
	ret->moves[3].maxdelay = 90;
	ret->moves[3].left = 1;
	ret->moves[3].hit = 0;
	ret->moves[3].endlag = 15;

	/* Over-Special */
	ret->moves[4].rect.x = 2.0f;
	ret->moves[4].rect.y = 0.5f;
	ret->moves[4].rect.w = 1.0f;
	ret->moves[4].rect.h = 1.0f;
	ret->moves[4].vx = 0.3f;
	ret->moves[4].vy = -0.3f;
	ret->moves[4].ax = -0.1f;
	ret->moves[4].ay = 0.00f;
	ret->moves[4].aw = 0.00f;
	ret->moves[4].ah = 0.00f;
	ret->moves[4].vw = 0.0f;
	ret->moves[4].vh = 0.0f;
	ret->moves[4].kb = 0.1f;
	ret->moves[4].kbgrowth = 0.5f;
	ret->moves[4].kbangle = -80.0;
	ret->moves[4].attack = 6;
	ret->moves[4].maxattack = 10;
	ret->moves[4].time = 12;
	ret->moves[4].type = ATTACK | MOVEMENT;
	ret->moves[4].owner = ret;
	ret->moves[4].tick = 0;
	ret->moves[4].mindelay = 5;
	ret->moves[4].maxdelay = 10;
	ret->moves[4].left = 1;
	ret->moves[4].hit = 0;
	ret->moves[4].endlag = 15;


	/* Up-Special */
	ret->moves[5].rect.x = 2.0f;
	ret->moves[5].rect.y = 0.5f;
	ret->moves[5].rect.w = 1.0f;
	ret->moves[5].rect.h = 1.0f;
	ret->moves[5].vx = 0.3f;
	ret->moves[5].vy = -0.7f;
	ret->moves[5].ax = -0.1f;
	ret->moves[5].ay = 0.00f;
	ret->moves[5].aw = 0.00f;
	ret->moves[5].ah = 0.00f;
	ret->moves[5].vw = 0.0f;
	ret->moves[5].vh = 0.0f;
	ret->moves[5].kb = 0.1f;
	ret->moves[5].kbgrowth = 0.5f;
	ret->moves[5].kbangle = -80.0;
	ret->moves[5].attack = 6;
	ret->moves[5].maxattack = 40;
	ret->moves[5].time = 12;
	ret->moves[5].type = ATTACK | MOVEMENT | AIRONCE;
	ret->moves[5].owner = ret;
	ret->moves[5].tick = 0;
	ret->moves[5].mindelay = 5;
	ret->moves[5].maxdelay = 5;
	ret->moves[5].left = 1;
	ret->moves[5].hit = 0;
	ret->moves[5].endlag = 15;

	/* Neutral-Special */
	ret->moves[6].rect.x = 2.0f;
	ret->moves[6].rect.y = 0.5f;
	ret->moves[6].rect.w = 1.0f;
	ret->moves[6].rect.h = 1.0f;
	ret->moves[6].vx = 0.3f;
	ret->moves[6].vy = 0.1f;
	ret->moves[6].ax = 0.2f;
	ret->moves[6].ay = -0.10f;
	ret->moves[6].aw = 0.00f;
	ret->moves[6].ah = 0.00f;
	ret->moves[6].vw = 0.0f;
	ret->moves[6].vh = 0.0f;
	ret->moves[6].kb = 0.1f;
	ret->moves[6].kbgrowth = 0.5f;
	ret->moves[6].kbangle = -80.0;
	ret->moves[6].attack = 6;
	ret->moves[6].maxattack = 30;
	ret->moves[6].time = 12;
	ret->moves[6].type = ATTACK;
	ret->moves[6].owner = ret;
	ret->moves[6].tick = 0;
	ret->moves[6].mindelay = 30;
	ret->moves[6].maxdelay = 90;
	ret->moves[6].left = 1;
	ret->moves[6].hit = 0;
	ret->moves[6].endlag = 15;

	/* DOWN-SPECIAL */
	ret->moves[7].rect.x = 2.0f;
	ret->moves[7].rect.y = -1.5f;
	ret->moves[7].rect.w = 1.0f;
	ret->moves[7].rect.h = 1.0f;
	ret->moves[7].vx = 0.3f;
	ret->moves[7].vy = 0.3f;
	ret->moves[7].ax = 0.0f;
	ret->moves[7].ay = -0.02f;
	ret->moves[7].aw = 0.00f;
	ret->moves[7].ah = 0.00f;
	ret->moves[7].vw = 0.0f;
	ret->moves[7].vh = 0.0f;
	ret->moves[7].kb = 0.5f;
	ret->moves[7].kbgrowth = 0.5f;
	ret->moves[7].kbangle = -10.0;
	ret->moves[7].attack = 6;
	ret->moves[7].maxattack = 20;
	ret->moves[7].time = 12;
	ret->moves[7].type = ATTACK;
	ret->moves[7].owner = ret;
	ret->moves[7].tick = 0;
	ret->moves[7].mindelay = 30;
	ret->moves[7].maxdelay = 90;
	ret->moves[7].left = 1;
	ret->moves[7].hit = 0;
	ret->moves[7].endlag = 15;
	
	/* SHIELD */
	ret->moves[8].rect.x = 0.0f;
	ret->moves[8].rect.y = 0.0f;
	ret->moves[8].rect.w = 3.0f;
	ret->moves[8].rect.h = 3.0f;
	ret->moves[8].vx = 0.0f;
	ret->moves[8].vy = 0.0f;
	ret->moves[8].ax = 0.0f;
	ret->moves[8].ay = 0.00f;
	ret->moves[8].aw = -0.01f;
	ret->moves[8].ah = -0.01f;
	ret->moves[8].vw = 0.0f;
	ret->moves[8].vh = 0.0f;
	ret->moves[8].kb = 0.5f;
	ret->moves[8].kbgrowth = 0.0f;
	ret->moves[8].kbangle = 00.0;
	ret->moves[8].attack = 0;
	ret->moves[8].maxattack = 0;
	ret->moves[8].time = 12;
	ret->moves[8].type = SHIELD;
	ret->moves[8].owner = ret;
	ret->moves[8].tick = 0;
	ret->moves[8].mindelay = 5;
	ret->moves[8].maxdelay = 200;
	ret->moves[8].left = 1;
	ret->moves[8].hit = 0;
	ret->moves[8].endlag = 15;

	ret->left = 1;
	return ret;
}

void tfighter_free(tfighter *t){
	free(t->moves);
	free(t);
	t = NULL;
}

void hitbox_update(hitbox *h){
	if(!h->owner){
		return;
	}
	if(!(h->type & PROJECTILE) && (h->owner->state & HITSTUN)){
		h->owner = NULL;
		return;
	}
	if(h->hitlag > 0){
		--h->hitlag;
		return;
	}
	++h->tick;
	h->prect.x = h->rect.x;
	h->prect.y = h->rect.y;
	h->prect.w = h->rect.w;
	h->prect.h = h->rect.h;
	if(~h->type & PROJECTILE || h->tick < h->maxdelay){
		h->rect.x += h->owner->rect.x - h->owner->prect.x;
		h->rect.y += h->owner->rect.y - h->owner->prect.y;
	}
	if(h->tick < h->maxdelay){
		if(!(h->owner->state & CHARGING) && h->tick >= h->mindelay){
			h->attack = ((double)h->maxattack - h->attack) * (h->tick - h->mindelay) / (h->maxdelay - h->mindelay) + h->attack;
			h->tick = h->maxdelay + 1;
		}
		else{
			return;
		}
	}
	if(h->tick > h->time + h->maxdelay){
		/*h->owner->state &= ~ATTACKING;*/
		h->owner->tick = h->endlag;
		h->owner = NULL;
		return;
	}

	h->rect.w += (h->vw += h->aw);
	h->rect.h += (h->vh += h->ah);
	if(h->left){
		h->rect.x -= h->vw;
	}
	if(h->type & MOVEMENT){
		h->owner->vx = h->vx;
		h->owner->vy = h->vy;
	}
	else{
		h->vx += h->ax;
		h->vy += h->ay;
		if(h->left)
			h->rect.x += h->vx + h->vw / 2;
		else
			h->rect.x += h->vx - h->vw / 2;
		h->rect.y += h->vy + h->vh/2;
	}
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
	dest->maxattack = src->maxattack;
	dest->kb = src->kb;
	dest->kbangle = src->kbangle;
	dest->kbgrowth = src->kbgrowth;
	dest->owner = src->owner;
	dest->hit = 0;

	dest->maxdelay = src->maxdelay;
	dest->mindelay = src->mindelay;
	dest->time = src->time;
	dest->hitlag = 0;
	dest->endlag = src->endlag;

	dest->type = src->type;
	dest->tick = 0;
}

void tfighter_input(tfighter *t, tlevel *tl, SDL_Event *e){
	int i;
	if(e->type == SDL_KEYDOWN){
		for(i=0; i<7; ++i){
			if(e->key.keysym.sym == t->keys[i]){
				t->state = t->state | (1 << i);
			}
		}
	}
	else if(e->type == SDL_KEYUP){
		for(i=0; i<7; ++i){
			if(e->key.keysym.sym == t->keys[i]){
				if(((1 << i) != ATTACKING) && (1 << i) != SPECIAL){
					t->state &= ~(1 << i);
				}
				else{
					t->state &= ~CHARGING;
				}
			}
		}
	}
	else if(e->type == SDL_JOYAXISMOTION && e->jaxis.which == t->joy){
		if(e->jaxis.axis == 0){	
			if(e->jaxis.value + t->joyxoffset < -JOYDEADZONE){
				t->state &= ~RIGHT;
				t->state |= LEFT;
				if(e->jaxis.value > -JOYDEADZONE * 3){
					t->state |= WALKING;
				}
				else{
					t->state |= RUNNING;
				}
			}
			else if(e->jaxis.value + t->joyxoffset > JOYDEADZONE){
				t->state &= ~LEFT;
				t->state |= RIGHT;
			}
			else{
				t->state &= ~(LEFT | RIGHT);
			}
		}
		else if(e->jaxis.axis == 1){
			if(e->jaxis.value + t->joyyoffset < -JOYDEADZONE){
				t->state &= ~DOWN;
				t->state |= UP;
			}
			else if(e->jaxis.value + t->joyyoffset > JOYDEADZONE){
				t->state &= ~UP;
				t->state |= DOWN;
			}
			else{
				t->state &= ~(UP | DOWN);
			}
		}
	}
	else if((e->type == SDL_JOYBUTTONDOWN || e->type == SDL_JOYBUTTONUP) && e->jbutton.which == t->joy){
		for(i=0; i<5; ++i){
			if(e->jbutton.state == SDL_PRESSED){
				if(i == e->jbutton.button){
					t->state |= t->jbuttons[i];
				}
			}
			else{
				if(t->jbuttons[i] != ATTACKING && t->jbuttons[i] != SPECIAL){
					t->state &= ~(t->jbuttons[i]);
				}
				else{
					t->state &= ~CHARGING;
				}
			}
		}
	}
}

void tfighter_update(tfighter *t, tlevel *tl){
	int i;
	t->prect.x = t->rect.x;
	t->prect.y = t->rect.y;
	t->prect.w = t->rect.w;
	t->prect.h = t->rect.h;

	/* DEATH */
	if(t->rect.x > tl->rect.w + 1 || t->rect.x < -1 || t->rect.y < -4 || t->rect.y > tl->rect.h + 1){
		t->state = 0;
		t->rect.x = 50;
		t->rect.y = 15;
		t->vx = 0;
		t->vy = 0;
		t->damage = 0;
	}

	if((t->state & (SPECIAL | ATTACKING) && !(t->pstate & (SPECIAL | ATTACKING)))){
		t->state |= CHARGING;
	}

	/* Jumping */
	if((~t->pstate & JUMP) && (t->state & JUMP) && t->tick <= 0){
		if(t->jump < t->MAXJUMPS){
			t->vy = -t->jumpvel;
			++t->jump;
		}
	}
	/* Attacking */
	if(((~t->pstate & ATTACKING) && (t->state & ATTACKING) && (~t->pstate & CHARGING))
			|| ((~t->pstate & SPECIAL) && (t->state & SPECIAL) && (~t->pstate & CHARGING))){
		int x = NATTACK;
		if(t->state & UP)
			x = UATTACK;
		else if(t->state & DOWN)
			x = DATTACK;
		else if(t->state & (LEFT | RIGHT))
			x = OATTACK;
		t->state |= CHARGING;
		if(t->state & SPECIAL){
			x += 4;
		}
		tlevel_add_hitbox(tl, t, &t->moves[x]);
		t->tick = t->moves[x].maxdelay + t->moves[x].time + t->moves[x].endlag; 
	}
	if(!(t->pstate & SHIELDING) && (t->state & SHIELDING)){
		tlevel_add_hitbox(tl, t, &t->moves[8]);
	}
	/* Moving */
	if(!(t->pstate & WALKING) && !(t->state & (ATTACKING | HITSTUN | CHARGING | SPECIAL))){
		if((t->state & RIGHT) && (t->vx > t->walk/2))
			t->state |= RUNNING;
		else if((t->state & LEFT) && (t->vx < -t->walk/2))
			t->state |= RUNNING;
		else if(t->state & (LEFT | RIGHT))
			t->state |= WALKING;
	}


	/* Checking hitboxes */
	for(i=0; i<tl->MAX_BOXES; ++i){
		tfighter *owner = tl->boxes[i].owner;
		hitbox *box = &tl->boxes[i];
		if((owner != NULL) && owner != t && box->tick > box->maxdelay && !(box->hit & t->id) && intersects(&t->rect, &box->rect)){
			t->vy += (float)(0.1 * sin(PI * box->kbangle / 180.0) * (box->kb + (t->damage * box->kbgrowth * 0.05)));
			t->vx += (float)(0.1 * cos(PI * box->kbangle / 180.0) * (box->kb + (t->damage * box->kbgrowth * 0.05)) * (box->left ? -1 : 1));
			box->hit |= t->id;
			t->state &= ~(ATTACKING | HELPLESS | SPECIAL);
			t->state |= HITSTUN;
			t->tick = (int)(box->kb + box->kbgrowth * t->damage / 10);
			t->damage += box->attack;
			t->hitlag = box->attack*3/4;
			box->hitlag = box->attack/2;
			if(~box->type & PROJECTILE){
				box->owner->hitlag = box->attack/2;
			}
			t->jump = 0;
		}
	}

	if(t->hitlag <= 0){
		/* Hitstun */
		if(t->state & HITSTUN){
				--t->tick;
				if(t->tick <= 0){
					t->state -= HITSTUN;
				}
		}
		/* Attacking & Movement */
		else{
			if(t->state & RUNNING){
				t->speed = t->run;
			}
			else{
				t->speed = t->walk;
			}
			if(t->state & (ATTACKING | SPECIAL)){
				--t->tick;
				if(t->tick <= 0){
					t->tick = 0;
					t->state &= ~(ATTACKING | CHARGING | SPECIAL);
				}
			}
			else if(t->state & LEFT){
				t->vx -= t->accel;
				if(t->vx <= -t->speed)
					t->vx = -t->speed;
				t->left = 1;
			}
			else if(t->state & RIGHT){
				t->vx += t->accel;
				if(t->vx >= t->speed)
					t->vx = t->speed;
				t->left = 0;
			}
			else if(t->state & DOWN){
				t->vy += t->accel;
			}
		}
		t->vy += t->gravity;
		t->rect.y += t->vy;
		for(i=0; i < tl->len; ++i){
			if(intersects(&t->rect, &tl->blocks[i])){
				if(t->vy > 0 && (tl->blocks[i].h >= 0.9f || !(t->state & DOWN))){
					if(t->state & HITSTUN){
						t->vy *= -1;
					}
					else{
						t->vy = 0;
						t->jump = 0;
						t->state &= ~HELPLESS;
					}
					t->rect.y = tl->blocks[i].y - t->rect.h;
				}
				else if(tl->blocks[i].h >= 0.9f){
					if(t->state & HITSTUN){
						t->vy *= -1;
					}
					else{
						t->vy = 0;	
					}
					t->rect.y = tl->blocks[i].y + tl->blocks[i].h;
				}
			}
		}
		if(~t->state & HITSTUN){
			t->vx *= 0.94;
		}
		t->rect.x += t->vx;
		for(i=0; i < tl->len; ++i){
			if(tl->blocks[i].h >= 1.0f){
				if(intersects(&t->rect, &tl->blocks[i])){
					if(t->vx > 0){
						if(t->state & HITSTUN){
							t->vx *= -1;	
						}
						else{
							t->vx = 0;	
						}
						t->rect.x = tl->blocks[i].x - t->rect.w;
					}
					else{
						if(t->state & HITSTUN){
							t->vx *= -1;	
						}
						else{
							t->vx = 0;
						}
						t->rect.x = tl->blocks[i].x + tl->blocks[i].w;
					}
				}
			}
		}
	}
	else{
		--t->hitlag;
	}
	t->pstate = t->state;
	t->state &= ~(WALKING | RUNNING);
}

void tlevel_add_hitbox(tlevel *tl, tfighter *t, hitbox *h){
	if(t->state & HELPLESS){
		return;
	}
	hitbox_spawn(t, h, &tl->boxes[tl->cbox]);
	if(h->type & AIRONCE){
		t->jump = t->MAXJUMPS;	
		t->state |= HELPLESS;
	}
	tl->cbox = (tl->cbox + 1) % tl->MAX_BOXES;
	/*if(h->type & MOVEMENT){
		t->vy = h->vy;
		t->vx = h->vx;
	}*/
}
