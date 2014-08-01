#include "scene.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaScene_Clear( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CONFIG );

  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => core" );
  lib->game->core->ClearScene();
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => world" );
  lib->game->world->ClearWorld();
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => done" );

  return 0;
}//LuaScene_Clear
