#include "camera.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaCamera_Get( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CAMERA );

  Object *object = lib->game->core->GetCamera();

  if( object ) {
    if( !object->GetLuaObjectId() ) {
      object->SetLuaObjectId( Engine::LuaObject::InitUserData( object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT ) );
      __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaCamera_Get => object new ref = %d", object->GetLuaObjectId() );
    } else {
      __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaCamera_Get => object old ref = %d", object->GetLuaObjectId() );
      lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    }
  } else {
    __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaCamera_Get => camera is null" );
    lua_pushnil( lua );
  }

  return 1;
}//LuaCamera_Get
