#include "scripting.h"

int lsetcolor(lua_State *l){
	if(lua_gettop(l) >= 2 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && lua_isnumber(l, -3) && cfighter != NULL){
		cfighter->red = lua_tonumber(l, -3);
		cfighter->green = lua_tonumber(l, -2);
		cfighter->blue = lua_tonumber(l, -1);
		return 0;
	}
	return -1;
}

int lsetsize(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, -1) && lua_isnumber(l, -2)){
		cfighter->rect.w = lua_tonumber(l, -2);
		cfighter->rect.h = lua_tonumber(l, -1);
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
	if(lua_gettop(l) >= 2 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && lua_isnumber(l, -3) && cfighter != NULL){
		cfighter->skin[0] = lua_tonumber(l, -1);
		if(cfighter->skin[0] > MAXSKIN){
			cfighter->skin[0] = MAXSKIN;
		}
		cfighter->skin[1] = lua_tonumber(l, -2);
		if(cfighter->skin[1] > MAXSKIN){
			cfighter->skin[1] = MAXSKIN;
		}
		cfighter->skin[2] = lua_tonumber(l, -3);
		if(cfighter->skin[2] > MAXSKIN){
			cfighter->skin[2] = MAXSKIN;
		}
		return 0;
	}
	return -1;
}


int lsetmove(lua_State *l){
	if(lua_gettop(l) == 11 && cfighter != NULL){
		int i;
		for(i = -10; i <= -1; ++i){
			if(!lua_isnumber(l, i)){
				return -2;
			}
		}
		tfighter_setmove(cfighter, (int)lua_tonumber(l, -11), (int)lua_tonumber(l, -10), (int)lua_tonumber(l, -9), (int)lua_tonumber(l, -8),(int)lua_tonumber(l, -7),(int)lua_tonumber(l, -6),(int)lua_tonumber(l, -5),(float)lua_tonumber(l, -4),(float)lua_tonumber(l, -3),(int)lua_tonumber(l, -2),(int)lua_tonumber(l, -1));

		return 0;
	}
	return -1;
}

int lsetbruiserness(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, -1)){
		int temp = lua_tonumber(l, -1);
		if(temp > 100){
			return -2;
		}
		cfighter->run += (100 - temp)/300.0f;
		cfighter->strength += temp/100.0f;
		return 0;
	}
	return -1;
}

int lsetjump(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, -1)){
		int temp = lua_tonumber(l, -1);
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
	if(lua_gettop(l) >= 3 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && lua_isnumber(l, -3) && lua_isnumber(l, -4)){
		if(level.blocks == NULL){
			level.blocks = malloc(sizeof(trect) * level.len);
		}
		level.blocks[level.cbox].x = lua_tonumber(l, -4);
		level.blocks[level.cbox].y = lua_tonumber(l, -3);
		level.blocks[level.cbox].w = lua_tonumber(l, -2);
		level.blocks[level.cbox].h = lua_tonumber(l, -1);
		++level.cbox;
	}
	return 0;
}

int lsetscale(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, -1) && lua_isnumber(l, -2)){
		level.rect.w = lua_tonumber(l, -2);	
		level.rect.h = lua_tonumber(l, -1);	
		return 0;
	}
	return -1;
}

int lsetname(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isstring(l, -1)){
		int length;
		char *temp = lua_tostring(l, -1);
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
