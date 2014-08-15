#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaConfig_GetGridSize( lua_State *lua );
int LuaConfig_GetWindowSize( lua_State *lua );

  
};
