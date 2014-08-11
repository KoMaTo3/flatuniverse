#pragma once

#include "../luaobject.h"


struct lua_State;
class Object;

namespace Animation {
  class AnimationSetAction;
}

namespace Engine {

  
int LuaObject_AddTag( lua_State *lua );
int LuaObject_AddTrigger( lua_State *lua );
int LuaObject_Destroy( lua_State *lua );
int LuaObject_Get( lua_State *lua );
int LuaObject_GetId( lua_State *lua );
int LuaObject_GetByPoint( lua_State *lua );
int LuaObject_GetByRect( lua_State *lua );
int LuaObject_GetName( lua_State *lua );
int LuaObject_GetNameFull( lua_State *lua );
int LuaObject_GetParentNameFull( lua_State *lua );
int LuaObject_GetPos( lua_State *lua );
int LuaObject_GetSelected( lua_State *lua );
int LuaObject_New( lua_State *lua );
int LuaObject_RemoveTag( lua_State *lua );
int LuaObject_Select( lua_State *lua );
int LuaObject_SetAnimation( lua_State *lua );
int LuaObject_SetPos( lua_State *lua );
int LuaObject_SetScript( lua_State *lua );

typedef void LuaObject_Callback( Object *object );
void LuaObject_callback_SetOnSetScript( LuaObject_Callback *callback );
void LuaObject_AnimationCallback( Animation::AnimationSetAction *action );

  
};
