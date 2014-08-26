#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaTools_GetClipboard( lua_State *lua );
int LuaTools_GetRandomSeed( lua_State *lua );
int LuaTools_GetTimeFormatted( lua_State *lua );
int LuaTools_MakeScreenshot( lua_State *lua );
int LuaTools_SetClipboard( lua_State *lua );

  
};
