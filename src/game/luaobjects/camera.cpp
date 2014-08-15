#include "camera.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaCamera_Get( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CAMERA );

  Object *object = lib->game->core->GetCamera();

  if( object ) {
    if( !object->GetLuaObjectId() ) {
      object->InitLuaUserData();
    }
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    lua_getfield( lua, -1, "fn" );
    lua_remove( lua, -2 );
  } else {
    lua_pushnil( lua );
  }

  return 1;
}//LuaCamera_Get


int Engine::LuaCamera_Set( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CAMERA );

  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
    luaL_error( lua, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
    return 0;
  }

  Object *object = NULL;
  if( lua_isstring( lua, 1 ) ) {
    object = lib->game->core->GetObject( lua_tostring( lua, 1 ) );
  } else if( lua_istable( lua, 1 ) ) {
    lua_getfield( lua, 1, "api" );
    if( lua_isuserdata( lua, -1 ) ) {
      luaL_checktype( lua, -1, LUA_TUSERDATA );
      object = *static_cast<Object **>( luaL_checkudata( lua, -1, "Object" ) );
    } else {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
      luaL_error( lua, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
    }
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
    luaL_error( lua, "Engine::LuaCamera_Set => use: Camera.Set( objectName|object )" );
    return 0;
  }

  if( object ) {
    lib->game->core->SetCamera( object );
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCamera_Set => object not found" );
  }

  return 0;
}//LuaCamera_Set


int Engine::LuaCamera_GetPos( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CAMERA );

  Object *object = lib->game->core->GetCamera();

  if( !object ) {
    lua_pushnil( lua );
    return 1;
  }

  const Vec3 pos = object->GetPosition();
  lua_pushnumber( lua, pos.x );
  lua_pushnumber( lua, pos.y );

  return 2;
}//LuaCamera_GetPos
