#include "config.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaConfig_GetGridSize( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CONFIG );

  float size = lib->game->world->GetGridSize();
  lua_pushnumber( lua, size );

  return 1;
}//LuaConfig_GetGridSize


int Engine::LuaConfig_GetWindowSize( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CONFIG );

  const Size &size = lib->game->core->GetWindowSize();
  lua_pushinteger( lua, size.width );
  lua_pushinteger( lua, size.height );

  return 2;
}//LuaConfig_GetWindowSize


int Engine::LuaConfig_GetUpdateInterval( lua_State *lua ) {
  lua_pushnumber( lua, COLLISION_UPDATE_FREQUENCY );

  return 1;
}//LuaConfig_GetUpdateInterval
