#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaCore_GameExit( lua_State *lua );
int LuaCore_GetTime( lua_State *lua );
int LuaCore_LoadScript( lua_State *lua );
int LuaCore_SetTimer( lua_State *lua );
int LuaCore_StopTimer( lua_State *lua );

  
};
