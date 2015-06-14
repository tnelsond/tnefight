#include <SDL2/SDL.h>

#include "tfighter.h"

tfighter tfighter_new(tfighterconf conf);

void tfighter_draw(SDL_Surface *surf, tfighter *t){
	SDL_FillRect(surf, &t->rect, 0x005500);
	if(t->box)
		SDL_FillRect(surf, &t->box->rect, 0x000044);
}

void tfighter_update(tfighter *t){
	hitbox *b = t->box;
	while(b){
		b->rect.x += b->vx += b->ax;
		b->rect.y += b->vy += b->ay;
		b = b->next;
	}
}
