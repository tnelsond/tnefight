#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "tfighter.h"

int gid = 1;

int intersects(trect *r, trect *o, float xintersect, float yintersect){
	if(r->type == RECT && o->type == RECT){
		return r->x + r->w - yintersect > o->x && r->x + yintersect < o->x + o->w 
			&& r->y + r->h - xintersect > o->y && r->y + xintersect < o->y + o->h;
	}
	else if(r->type == RECT && o->type == TRIUL){
		return (r->x + r->w - yintersect > o->x && r->x + yintersect < o->x + o->w && r->y + r->h - xintersect > o->y + o->h - (r->x + r->w - yintersect - o->x) * o->h/o->w && r->y + xintersect < o->y + o->h);
	}
}

void tparticle_set(tparticle *part, float x, float y, float vx, float vy, float size, int ttime, Uint32 color){
	part->px = x;
	part->py = y;
	part->x = part->px + vx;
	part->y = part->py + vy;
	part->size = size;
	part->time = ttime;
	part->color = color;
}

void tparticle_update(tparticle *part){
	float tempx, tempy;
	tempx = part->x;
	tempy = part->y;
	part->x += tempx - part->px;
	part->y += tempy - part->py;
	part->px = tempx;
	part->py = tempy;
	--part->time;
}

void tcamera_interpolate(tcamera *tc, float alpha){
	tc->ix = terp(tc->px, tc->x, alpha);
	tc->iy = terp(tc->py, tc->y, alpha);
}

void tcamera_track(tcamera *tc, tlevel *tl, tfighter **t, int len){
	float minx, miny, maxx, maxy, ratio;
	minx = miny = maxx = maxy = ratio = 0;
	int i;

	tc->px = tc->x;
	tc->py = tc->y;

	for(i = 0; i < len; ++i){
		if(i == 0){
			maxx = t[i]->rect.x + t[i]->rect.w;
			minx = t[i]->rect.x;
			maxy = t[i]->rect.y; 
			miny = t[i]->rect.y - t[i]->rect.h;
		}
		else{
			if(t[i]->rect.x < minx){
				minx = t[i]->rect.x;
			}
			if(t[i]->rect.x + t[i]->rect.w > maxx){
				maxx = t[i]->rect.x + t[i]->rect.w;
			}
			if(t[i]->rect.y - t[i]->rect.h < miny){
				miny = t[i]->rect.y - t[i]->rect.h;
			}
			if(t[i]->rect.y > maxy){
				maxy = t[i]->rect.y;
			}
		}
	}

	minx -= CAMERABORDER;
	miny -= CAMERABORDER;
	maxx += CAMERABORDER;
	maxy += CAMERABORDER;

	if(minx < tl->rect.x)
		minx = tl->rect.x;
	else if(minx >= tl->rect.x + tl->rect.w)
		minx = tl->rect.x + tl->rect.w - CAMERABORDER;
	if(maxx <= tl->rect.x)
		maxx = tl->rect.x + CAMERABORDER;
	else if(maxx > tl->rect.x + tl->rect.w)
		maxx = tl->rect.x + tl->rect.w;

	if(miny < tl->rect.y)
		miny = tl->rect.y;
	else if(miny >= tl->rect.y + tl->rect.h)
		miny = tl->rect.y + tl->rect.h - CAMERABORDER;
	if(maxy <= tl->rect.y)
		maxy = tl->rect.y + CAMERABORDER;
	else if(maxy > tl->rect.y + tl->rect.h)
		maxy = tl->rect.y + tl->rect.h;

	tc->width = maxx - minx;
	tc->height = maxy - miny;
	ratio = ((float)tc->swidth)/tc->sheight;
	if(tc->width/tc->height > ratio){
		tc->height = tc->width/ratio;
	}
	else{
		tc->width = ratio * tc->height;
	}
	tc->x = minx;
	tc->y = miny;
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

void tfighter_setmove(tfighter *t, int index, int attack, int kb, int kbgrowth, int mindelay, int chargetime, int duration, int endlag, int x, int y, int width, int height, int angle, int kbangle, int speed, int type, int img){
	SDL_Log("setmove start");
	float tempangle;
	float tempspeed;
	hitbox *h = &t->moves[clamp(index, 0, 8)];
	h->attack = clamp(attack, 0, 100);
	h->kb = clamp(kb, 0, 100) / 2.0f;
	h->kbgrowth = clamp(kbgrowth, 0, 100) / 50.0f;
	h->time = clamp(duration, 1, 500);
	h->endlag = clamp(endlag, 1, 100);
	h->rect.x = clamp(x, -100, 100) / 50.0f;
	h->rect.y = clamp(y, -100, 100) / 50.0f;
	h->rect.w = .5f + clamp(width, 1, 100) / 10.0f;
	h->rect.h = .5f + clamp(height, 1, 100) / 10.0f;
	tempangle = (angle % 360) * PI / 180.0f;
	h->kbangle = kbangle % 360;
	tempspeed = clamp(speed, 0, 100) / 50.0f;
	h->vx = tempspeed * cos(tempangle);
	h->vy = -tempspeed * sin(tempangle);
	h->type = type;
	h->image = clamp(img, 0, 13);
	h->mindelay = clamp(mindelay, 1, 200);
	h->maxdelay = h->mindelay + clamp(chargetime, 1, 200);
	SDL_Log("%f, %f, %f, %f, %f, %f, %d", h->rect.x, h->rect.y, h->rect.w, h->rect.h, h->vx, h->vy, h->time);
	SDL_Log("setmove end");
}

void tfighter_balance_move(tfighter *t, int index, int attack, int kb, int chargetime, int duration, int endlag, int x, int y, int width, int height, int angle, int kbangle, int speed, int type, int img){
	tfighter_setmove(t, index, 
			(attack - (width + height) / 2) / (type & PROJECTILE ? 3 : 1), /* Attack */
			kb, /* KnockBack */
			(100 - (speed + kb / 2)), /* KnockBack Growth */
			(int)((200 - endlag) * (attack + kb/2.0)/100.0 + duration * 0.03), /* Minimum Delay */ 
			chargetime, /* Max Delay */
			duration, /* Duration */
			(int)(endlag * attack/100.0 + duration * 0.03),
			x, y, width, height,
			angle,
			kbangle,
			speed / (!(type & PROJECTILE) ? 3 : 1),
			type,
			img);
}

tfighter *tfighter_new(float x, float y, Uint32 color, SDL_Keycode *keys, Uint32 *joybuttons, SDL_JoystickID joy, int joyxoffset, int joyyoffset, Uint8 *skin){
	int i;
	tfighter *ret = malloc(sizeof(tfighter));
	ret->skin = skin;
	ret->joyxoffset = joyxoffset;
	ret->joyyoffset = joyyoffset;
	ret->tick = 0;
	ret->damage = 0;
	ret->color = color;
	ret->state = 0;
	ret->pstate = 0;
	ret->keys = keys;
	ret->rect.type = RECT;
	ret->prect.x = x;
	ret->prect.y = y;
	ret->prect.w = 2;
	ret->prect.h = 2;
	ret->rect.x = x;
	ret->rect.y = y;
	ret->rect.w = 2;
	ret->rect.h = 3;
	ret->fallspeed = 1.5f;
	ret->driftspeed = 0.2f;
	ret->launchresistance = 3.0f;
	ret->jumpvel = 0.7f;
	ret->MAXJUMPS = 2;
	ret->jump = 2;
	ret->vx = 0;
	ret->vy = 0;
	ret->accel = 0.08f;
	ret->walk = 0.03f;
	ret->run = 0.05f;
	ret->speed = 0.0f;
	ret->gravity = 0.015f;
	ret->hitlag = 0;
	ret->id = gid;
	ret->joy = joy;
	ret->jbuttons = joybuttons;
	ret->name = NULL;
	ret->strength = 0.5f;
	gid = gid << 1;

	ret->moves = malloc(sizeof(hitbox)*MAXMOVES);
	for(i = 0; i<MAXMOVES; ++i){
		ret->moves[i].rect.x = 0;
		ret->moves[i].rect.y = 0.0f;
		ret->moves[i].rect.w = 0.0f;
		ret->moves[i].rect.h = 0.0f;
		ret->moves[i].rect.type = RECT;
		ret->moves[i].vx = 0.0f;
		ret->moves[i].vy = 0.0f;
		ret->moves[i].ax = 0.00f;
		ret->moves[i].ay = 0.00f;
		ret->moves[i].aw = 0.00f;
		ret->moves[i].ah = 0.00f;
		ret->moves[i].vw = 0.0f;
		ret->moves[i].vh = 0.0f;
		ret->moves[i].kb = 0.0f;
		ret->moves[i].kbgrowth = 0.0f;
		ret->moves[i].kbangle = 45.0f;
		ret->moves[i].attack = 0;
		ret->moves[i].attackmultiply = 1;
		ret->moves[i].time = 0;
		ret->moves[i].type = 0;
		ret->moves[i].owner = ret;
		ret->moves[i].tick = 0;
		ret->moves[i].mindelay = 0;
		ret->moves[i].maxdelay = 0;
		ret->moves[i].left = 0;
		ret->moves[i].hit = 0;
		ret->moves[i].endlag = 0;
		ret->moves[i].image = 12;
		tfighter_setmove(ret, i, 50, 50, 50, 20, 70, 10, 5, 10, 0, 20, 20, 0, 45, 10, ATTACK, 12);
	}

	ret->left = 1;
	return ret;
}

void tfighter_free(tfighter *t){
	if(t->name){
		free(t->name);
	}
	free(t->moves);
	free(t);
	t = NULL;
}

void hitbox_update(hitbox *h){
	int i;
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
			if(h->mindelay < h->maxdelay){
				h->attackmultiply = (float)(0.4f * (h->tick - h->mindelay) / (h->maxdelay - h->mindelay) + 1);
			}
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
	for(i = 0; i < level.MAX_BOXES; ++i){
		if((level.boxes[i].owner != NULL) && (level.boxes[i].owner != h->owner) && (&level.boxes[i] != h) && intersects(&level.boxes[i].rect, &h->rect, 0, 0)){
			if(level.boxes[i].type & REFLECT){
				h->vx *= -1;
				h->ax *= -1;
				h->left = !h->left;
				if(~h->type & PROJECTILE){
					h->owner->vx *= -1;
				}
				if(h->owner->tick > 5){
					h->owner->tick = 5;
				}
				h->owner = level.boxes[i].owner;
			}
			else if(level.boxes[i].tick > level.boxes[i].maxdelay && h->tick > h->maxdelay){
				int diff = h->attack - level.boxes[i].attack;
				if(diff > ATTACKPRECEDENCE){
					level.boxes[i].owner->tick = 5;
					level.boxes[i].owner = NULL;
					return;
				}
				else if(diff < -ATTACKPRECEDENCE){
					h->owner->tick = 5;
					h->owner = NULL;
					return;
				}
				else{
					level.boxes[i].owner->tick = 5;
					h->owner->tick = 5;
					level.boxes[i].owner = NULL;
					h->owner = NULL;
					return;
				}
			}
		}
	}
}

void hitbox_spawn(tfighter *t, hitbox *src, hitbox *dest){
	trect *offset = &t->rect;

	dest->rect.w = src->rect.w * t->rect.w / 2;
	dest->rect.h = src->rect.h * t->rect.h / 2;

	if(t->left){
		dest->rect.x = -src->rect.x - dest->rect.w/2 + offset->x + offset->w/2;
	}
	else{
		dest->rect.x = src->rect.x + offset->x - dest->rect.w/2 + offset->w/2;
	}

	dest->rect.y = src->rect.y + offset->y  - dest->rect.h/2;

	dest->rect.type = src->rect.type;

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
	dest->attackmultiply = src->attackmultiply;
	dest->kb = src->kb * t->strength;
	dest->kbangle = src->kbangle;
	if(dest->left){
		if(dest->kbangle < 90){
			dest->kbangle = (90 - dest->kbangle) + 90;
		}
		else if(dest->kbangle < 180){
			dest->kbangle = (180 - dest->kbangle);
		}
		else if(dest->kbangle < 270){
			dest->kbangle = 360 - (dest->kbangle - 180);
		}
		else{
			dest->kbangle = 180 + (360 - dest->kbangle);
		}
	}
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
	dest->image = src->image;
	SDL_Log("Spawning move");
}

void tfighter_input(tfighter *t, tlevel *tl, SDL_Event *e){
	int i;
	if(t->keys){
		if(e->type == SDL_KEYDOWN){
			for(i=0; i<7; ++i){
				if(e->key.keysym.sym == t->keys[i]){
					if(!(t->state & (ATTACKING | SPECIAL)) || !(t->keys[i] & (ATTACKING | SPECIAL))){
						t->state = t->state | (1 << i);
					}
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
	}
	if(e->type == SDL_JOYAXISMOTION && e->jaxis.which == t->joy){
		if(e->jaxis.axis == 0){	
			if(e->jaxis.value + t->joyxoffset < -JOYDEADZONE){
				t->state &= ~RIGHT;
				t->state |= LEFT;
				t->state |= RUNNING;
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
					if(!(t->state & (ATTACKING | SPECIAL)) || !(t->jbuttons[i] & (ATTACKING | SPECIAL))){
						t->state |= t->jbuttons[i];
					}
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
	if(t->state & HITSTUN && t->vx != 0 && t->vy < 0){
		float a = t->vx, b = t->vy;
		double angle = atan2((double)t->vy, (double)t->vx);
		t->state &= ~(ATTACKING | CHARGING | SPECIAL);
		t->vx -= (float)(cos(angle)) * KBRESISTANCE;
		t->vy -= (float)(sin(angle)) * KBRESISTANCE;
		if((t->vx > 0 && a < 0) || (t->vx < 0 && a > 0)){
			t->vx = 0;
		}
		if((t->vy > 0 && b < 0) || (t->vy < 0 && b > 0)){
			t->vy = 0;
		}
	}
	t->prect.x = t->rect.x;
	t->prect.y = t->rect.y;
	t->prect.w = t->rect.w;
	t->prect.h = t->rect.h;

	/* DEATH */
	if(t->rect.x > tl->rect.x + tl->rect.w + 1 || t->rect.x + t->rect.w < tl->rect.x || t->rect.y + t->rect.h < tl->rect.y - 4 || t->rect.y > tl->rect.y + tl->rect.h + 1){
		t->state = 0;
		t->hitlag = 0;
		t->pstate = 0;
		t->jump = 0;
		t->rect.x = tl->rect.x + tl->spawnx - t->rect.w / 2;
		t->rect.y = tl->rect.y + tl->spawny;
		t->vx = 0;
		t->vy = 0;
		t->tick = 0;
		t->damage = 0;
		for(i=0; i<tl->MAX_BOXES; ++i){
			hitbox *box = &tl->boxes[i];
			if(box->owner == t){
				box->owner = NULL;
			}
		}
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
		if((owner) && owner != t && box->tick > box->maxdelay && !(box->hit & t->id) && intersects(&t->rect, &box->rect, 0, 0)){
			t->damage += box->attack * box->attackmultiply;
			t->vy += (float)(-0.04 * sin(PI * box->kbangle / 180.0) * (box->kb + (t->damage * box->kbgrowth))/t->launchresistance);
			t->vx += (float)(0.04 * cos(PI * box->kbangle / 180.0) * (box->kb + (t->damage * box->kbgrowth))/t->launchresistance);
			SDL_Log("attack: %d, attackmultiply: %f, kb: %f, kbgrowth: %f, kbangle: %f\n", box->attack, box->attackmultiply, box->kb, box->kbgrowth, box->kbangle);
			box->hit |= t->id;
			t->state &= ~(ATTACKING | HELPLESS | SPECIAL | WALKING | RUNNING | JUMP | CHARGING);
			t->state |= HITSTUN;
			t->tick = (int)((box->kb + box->kbgrowth * t->damage) / 3);
			t->hitlag = box->attack*0.1f;
			box->hitlag = box->attack*0.1f;
			if(~box->type & PROJECTILE){
				box->owner->hitlag = box->attack*0.1f;
			}
			t->jump = 0;
		}
	}

	if(t->hitlag <= 0){
		--t->tick; 
		if(t->tick < 0){
			t->tick = 0;
		}
		/* Hitstun */
		if(t->state & HITSTUN){
				if(t->tick <= 0){
					t->state &= (LEFT | RIGHT | UP | DOWN);
				}
		}
		/* Attacking & Movement */
		else{
			if(t->pstate & GROUND){
				if(t->state & RUNNING){
					t->speed = t->run;
				}
				else{
					t->speed = t->walk;
				}
			}
			else{
				t->speed = t->driftspeed;
			}
			if(t->state & (ATTACKING | SPECIAL)){
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
		if(t->vy > t->fallspeed){
			t->vy = t->fallspeed;	
		}
		t->rect.y += t->vy;
		for(i=0; i < tl->len; ++i){
			if(intersects(&t->rect, &tl->blocks[i], 0, INTERSECT_TOLERANCE)){
				if(t->vy > 0 && (tl->blocks[i].h >= 0.9f || t->state ^ DOWN)){
					if(t->state & HITSTUN){
						t->hitlag = 2;
						t->vy *= -DAMPENING;
					}
					else{
						t->vy = 0;
						t->jump = 0;
						t->state &= ~HELPLESS;
						t->state |= GROUND;
					}
					if(t->rect.type == RECT && tl->blocks[i].type == TRIUL){
						if(t->rect.x + t->rect.w > tl->blocks[i].x + tl->blocks[i].w){
							t->rect.y = tl->blocks[i].y - t->rect.h;
						}
						else{
							t->rect.y = tl->blocks[i].y + tl->blocks[i].h - (t->rect.x + t->rect.w - tl->blocks[i].x) * tl->blocks[i].h/tl->blocks[i].w  - t->rect.h;
						}
					}
					else{
						t->rect.y = tl->blocks[i].y - t->rect.h;
					}
				}
				else if(tl->blocks[i].h >= 0.9f){
					if(t->state & HITSTUN){
						t->hitlag = 2;
						t->vy *= -DAMPENING;
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
				if(intersects(&t->rect, &tl->blocks[i], INTERSECT_TOLERANCE, 0)){
					if(t->vx > 0){
						if(t->state & HITSTUN){
							t->hitlag = 10;
							t->vx *= -DAMPENING;
							t->vy *= DAMPENING;
						}
						if(t->rect.type == RECT && tl->blocks[i].type == TRIUL){
							if(t->rect.x + t->rect.w > tl->blocks[i].x + tl->blocks[i].w){
								t->rect.y = tl->blocks[i].y - t->rect.h;
							}
							else{
								t->rect.y = tl->blocks[i].y + tl->blocks[i].h - (t->rect.x + t->rect.w - tl->blocks[i].x) * tl->blocks[i].h/tl->blocks[i].w  - t->rect.h;
							}
						}
						else{
							t->rect.x = tl->blocks[i].x - t->rect.w; 
							if(t->state ^ HITSTUN){
								t->vx = 0;
							}
						}
					}
					else{
						if(t->state & HITSTUN){
							t->hitlag = 10;
							t->vx *= -DAMPENING;	
							t->vy *= DAMPENING;
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
	t->state &= ~(WALKING | RUNNING | GROUND);
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
