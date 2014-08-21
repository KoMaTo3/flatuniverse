#pragma once

#include "game/luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaScene_AddActiveObject( lua_State *lua );
int LuaScene_Clear( lua_State *lua );
int LuaScene_LoadWorld( lua_State *lua );
int LuaScene_SaveWorld( lua_State *lua );
int LuaScene_SetLightAmbient( lua_State *lua );
int LuaScene_SetPause( lua_State *lua );

  
};
