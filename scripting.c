#include "scripting.h"

int lsetcolor(lua_State *l){
	if(lua_gettop(l) >= 2 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && lua_isnumber(l, -3) && cfighter != NULL){
		cfighter->red = lua_tonumber(l, -3);
		cfighter->green = lua_tonumber(l, -2);
		cfighter->blue = lua_tonumber(l, -1);
	}
	else
		return -1;
	return 0;
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
	}
	else
		return -1;
	return 0;
}

int lsetbruiserness(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isnumber(l, -1)){
		int temp = lua_tonumber(l, -1);
		cfighter->gravity = (temp + 20.0f) / 500;
		cfighter->speed = 0.9f - (temp + 20.0f) / 10;
	}
	else
		return -1;
	return 0;
}

int lsetname(lua_State *l){
	if(lua_gettop(l) >= 0 && lua_isstring(l, -1)){
		int length;
		char *temp = lua_tostring(l, -1);
		length = strlen(temp);
		cfighter->name = malloc(length);
		memcpy(cfighter->name, temp, length + 1);
	}
	else
		return -1;
	return 0;
}


void linit(){
	cfighter = NULL;
	l = luaL_newstate();
	luaL_openlibs(l);

	lua_register(l, "setcolor", lsetcolor);
	lua_register(l, "setname", lsetname);
	lua_register(l, "setbruiserness", lsetbruiserness);
	lua_register(l, "setskin", lsetskin);
}

void lclose(){
	lua_close(l);
}
