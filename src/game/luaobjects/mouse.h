#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaMouse_GetPos( lua_State *lua );
int LuaMouse_ListenKey( lua_State *lua );
int LuaMouse_ListenMove( lua_State *lua );

  
};
