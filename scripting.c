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

int lrunscript(char *str){
	if(luaL_dofile(l, str)){
		error(l, "Cannot run lua script: %s", lua_tostring(l, -1));
		return -1;
	}
	return 0;
}

int lsetskin(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && cfighter != NULL){
		cfighter->skin[1] = lua_tonumber(l, -1);
		cfighter->skin[0] = lua_tonumber(l, -2);
		return 0;
	}
	return -1;
}

int lsetbruiserness(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, -1)){
		int temp = lua_tonumber(l, -1);
		cfighter->gravity = (temp + 20.0f) / 500;
		cfighter->run = 1.7f - (temp + 20.0f) / 20;
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

int lsetsize(lua_State *l){
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
		cfighter->name = malloc(length);
		memcpy(cfighter->name, temp, length + 1);
		return 0;
	}
	return -1;
}


void linit(){
	cfighter = NULL;
	l = luaL_newstate();
	luaL_openlibs(l);

	lua_register(l, "setcolor", lsetcolor);
	lua_register(l, "setname", lsetname);
	lua_register(l, "setbruiserness", lsetbruiserness);
	lua_register(l, "setskin", lsetskin);
	lua_register(l, "addblock", laddblock);
	lua_register(l, "setsize", lsetsize);
}

void lclose(){
	lua_close(l);
}
