#include "scripting.h"
#include <stdarg.h>

int lsetcolor(lua_State *l){
	int len = lua_gettop(l);
	if(len > 3){
		len = 3;
	}
	switch(len){ /* Fall through is intended */
		case 3:
			if(lua_isnumber(l, 4)){
				cfighter->footcolor = lua_tonumber(l, 4) * 0x100 + 0xFF;
			}
		case 2:
			if(lua_isnumber(l, 3)){
				cfighter->armcolor = lua_tonumber(l, 3) * 0x100 + 0xFF;
			}
		case 1:
			if(lua_isnumber(l, 2)){
				cfighter->color = lua_tonumber(l, 2) * 0x100 + 0xFF;
			}
		default:
			if(lua_isnumber(l, 1)){
				cfighter->headcolor = lua_tonumber(l, 1) * 0x100 + 0xFF;
				return 0;
			}
	}
	return -1;
}

int lsetsize(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, 1) && lua_isnumber(l, 2)){
		cfighter->rect.h = lua_tonumber(l, 2);
		cfighter->rect.w = lua_tonumber(l, 1) * cfighter->rect.h;
		if(cfighter->rect.h < 2){
			cfighter->rect.h = 2;
		}
		if(cfighter->rect.w < 1.5f){
			cfighter->rect.w = 1.5f;
		}
		if(cfighter->rect.h > 4){
			cfighter->rect.h = 4;
		}
		if(cfighter->rect.w > cfighter->rect.h*0.8f){
			cfighter->rect.w = cfighter->rect.h*0.8f;
		}
		cfighter->launchresistance = cfighter->rect.w * cfighter->rect.h;
		return 0;
	}
	return -1;
}

int lrunscript(char *str){
	if(luaL_dofile(l, str)){
		error(l, "Cannot run lua script: %s", lua_tostring(l, -1));
		return -1;
	}
	return 0;
}

int lsetskin(lua_State *l){
	if(lua_gettop(l) >= 3 && lua_isnumber(l, 1) && lua_isnumber(l, 2) && lua_isnumber(l, 3) && lua_isnumber(l, 4) && cfighter != NULL){
		cfighter->skin[0] = lua_tonumber(l, 4);
		if(cfighter->skin[0] > MAXSKIN){
			cfighter->skin[0] = MAXSKIN;
		}
		cfighter->skin[1] = lua_tonumber(l, 3);
		if(cfighter->skin[1] > MAXSKIN){
			cfighter->skin[1] = MAXSKIN;
		}
		cfighter->skin[2] = lua_tonumber(l, 2);
		if(cfighter->skin[2] > MAXSKIN){
			cfighter->skin[2] = MAXSKIN;
		}
		cfighter->skin[3] = lua_tonumber(l, 1);
		if(cfighter->skin[3] > MAXSKIN){
			cfighter->skin[3] = MAXSKIN;
		}
		return 0;
	}
	return -1;
}


int lsetmove(lua_State *l){
	int i;
	if(lua_gettop(l) != 15){
		SDL_Log("Wrong number of arguments to setmove! Takes 15.");
		return -1;
	}
	if(cfighter == NULL){
		return -3;
	}
	for(i = 1; i <= 15; ++i){
		if(!lua_isnumber(l, i)){
			SDL_Log("Argument %d in setmove is not a number!", i);
			return -2;
		}
	}

	/*
	tfighter_setmove(cfighter, (int)lua_tonumber(l, -17), (int)lua_tonumber(l, -16), (int)lua_tonumber(l, -15), (int)lua_tonumber(l, -14), (int)lua_tonumber(l, -13), (int)lua_tonumber(l, -12), (int)lua_tonumber(l, -11), (int)lua_tonumber(l, -10), (int)lua_tonumber(l, -9), (int)lua_tonumber(l, -8), (int)lua_tonumber(l, -7), (int)lua_tonumber(l, -6), (int)lua_tonumber(l, -5), (int)lua_tonumber(l, -4), (int)lua_tonumber(l, -3), (int)lua_tonumber(l, -2), (int)lua_tonumber(l, -1));
	*/

	tfighter_balance_move(cfighter,
		lua_tonumber(l, 1),
		lua_tonumber(l, 2),
		lua_tonumber(l, 3),
		lua_tonumber(l, 4),
		lua_tonumber(l, 5),
		lua_tonumber(l, 6),
		lua_tonumber(l, 7),
		lua_tonumber(l, 8),
		lua_tonumber(l, 9),
		lua_tonumber(l, 10),
		lua_tonumber(l, 11),
		lua_tonumber(l, 12),
		lua_tonumber(l, 13),
		lua_tonumber(l, 14),
		lua_tonumber(l, 15)
	);
	/*
		int index,
		int attack,
		int kb,
		int chargetime,
		int duration,
		int endlag,
		int x,
		int y,
		int width,
		int height,
		int angle,
		int kbangle,
		int speed,
		int type,
		int img);
	*/

	return 0;
}

int lsetbruiserness(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, 1)){
		int temp = lua_tonumber(l, 1);
		if(temp > 100){
			return -2;
		}
		cfighter->run += (100 - temp)/30.0f;
		cfighter->strength += temp/100.0f;
		return 0;
	}
	return -1;
}

int lsetjump(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, 1)){
		int temp = lua_tonumber(l, 1);
		if(temp > 100){
			return -2;
		}
		cfighter->gravity += temp/2000.0f;
		cfighter->jumpvel += temp/200.0f;
		return 0;
	}
	return -1;
}

int laddblock(lua_State *l){
	if(lua_gettop(l) >= 3 && lua_isnumber(l, 1) && lua_isnumber(l, 2) && lua_isnumber(l, 3) && lua_isnumber(l, 4)){
		if(level.blocks == NULL){
			level.blocks = malloc(sizeof(trect) * level.len);
		}
		level.blocks[level.cbox].x = lua_tonumber(l, 1);
		level.blocks[level.cbox].y = lua_tonumber(l, 2);
		level.blocks[level.cbox].w = lua_tonumber(l, 3);
		level.blocks[level.cbox].h = lua_tonumber(l, 4);
		if(lua_gettop(l) >= 4 && lua_isnumber(l, 5)){
			level.blocks[level.cbox].type = lua_tonumber(l, 5);
		}
		else{
			level.blocks[level.cbox].type = RECT;
		}
		++level.cbox;
	}
	return 0;
}

int lsetscale(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, 1) && lua_isnumber(l, 2)){
		level.rect.w = lua_tonumber(l, 1);	
		level.rect.h = lua_tonumber(l, 2);	
		return 0;
	}
	return -1;
}

int lsetname(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isstring(l, 1)){
		int length;
		char *temp = lua_tostring(l, 1);
		length = strlen(temp);
		cfighter->name = malloc(length*(sizeof(char)));
		strcpy(cfighter->name, temp);
		return 0;
	}
	return -1;
}


void linit(){
	cfighter = NULL;
	l = luaL_newstate();
	luaL_openlibs(l);

	lua_pushnumber(l, ATTACK); lua_setglobal(l, "ATTACK");	
	lua_pushnumber(l, MOVEMENT); lua_setglobal(l, "MOVEMENT");	
	lua_pushnumber(l, REFLECT); lua_setglobal(l, "REFLECT");	
	lua_pushnumber(l, PROJECTILE); lua_setglobal(l, "PROJECTILE");	
	lua_pushnumber(l, AIRONCE); lua_setglobal(l, "AIRONCE");	

	lua_register(l, "setcolor", lsetcolor);
	lua_register(l, "setname", lsetname);
	lua_register(l, "setbruiserness", lsetbruiserness);
	lua_register(l, "setskin", lsetskin);
	lua_register(l, "setsize", lsetsize);
	lua_register(l, "setmove", lsetmove);

	lua_register(l, "addblock", laddblock);
	lua_register(l, "setscale", lsetscale);
	lua_register(l, "setjump", lsetjump);
}

void lclose(){
	lua_close(l);
}
