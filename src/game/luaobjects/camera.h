#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaCamera_Get( lua_State *lua );
int LuaCamera_GetPos( lua_State *lua );
int LuaCamera_Set( lua_State *lua );

  
};
