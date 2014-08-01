#pragma once

#include "../luaobject.h"


struct lua_State;


namespace Engine {

  
int LuaObject_Destroy( lua_State *lua );
int LuaObject_Get( lua_State *lua );
int LuaObject_GetName( lua_State *lua );
int LuaObject_GetNameFull( lua_State *lua );
int LuaObject_GetParentNameFull( lua_State *lua );
int LuaObject_New( lua_State *lua );
int LuaObject_SetAnimation( lua_State *lua );

  
};
