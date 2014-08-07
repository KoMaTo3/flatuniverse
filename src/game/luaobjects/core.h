#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaCore_SetTimer( lua_State *lua );
int LuaCore_StopTimer( lua_State *lua );

  
};
