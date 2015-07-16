#include <stdio.h>
#include "scripting.h"

int lsetcolor(lua_State *l){
	if(lua_gettop(l) >= 2 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && lua_isnumber(l, -3) && cfighter != NULL){
		cfighter->red = lua_tonumber(l, -3);
		cfighter->green = lua_tonumber(l, -2);
		cfighter->blue = lua_tonumber(l, -1);
		printf("Inside");
	}
	printf("Outside");
	return 0;
}

int lsetskin(lua_State *l){
	if(lua_gettop(l) >= 1 && lua_isnumber(l, -1) && lua_isnumber(l, -2) && cfighter != NULL){
		cfighter->skin[1] = lua_tonumber(l, -1);
		cfighter->skin[0] = lua_tonumber(l, -2);
	}
	return 0;
}


void linit(){
	cfighter = NULL;
	l = luaL_newstate();
	luaL_openlibs(l);

	lua_register(l, "setcolor", lsetcolor);
	lua_register(l, "setskin", lsetskin);
}

void lclose(){
	lua_close(l);
}
