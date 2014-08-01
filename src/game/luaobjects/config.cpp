#include "config.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaConfig_GetGridSize( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CONFIG );

  float size = lib->game->world->GetGridSize();
  lua_pushnumber( lua, size );

  return 1;
}//LuaConfig_GetGridSize
