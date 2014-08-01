#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaDebug_Alert( lua_State *lua );
int LuaDebug_Log( lua_State *lua );
int LuaDebug_RenderState( lua_State *lua );
int LuaDebug_Render( lua_State *lua );

  
};
