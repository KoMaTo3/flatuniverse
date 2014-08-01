#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaTools_GetRandomSeed( lua_State *lua );
int LuaTools_GetTime( lua_State *lua );

  
};
