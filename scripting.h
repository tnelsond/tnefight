#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "tfighter.h"

tfighter *cfighter;
lua_State *l;

int lsetcolor(lua_State *l);
void linit();
void lclose();

#endif
