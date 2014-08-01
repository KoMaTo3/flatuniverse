#include "keyboard.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaKeyboard_Listen( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CORE );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaKeyboard_Listen => use: Keyboard.Listen( functionName )" );
    return 0;
  }
  const std::string funcName = lua_tostring( lua, 1 );

  Game::luaKeyboardListenersList::iterator iter, iterEnd = lib->game->luaKeyboardListeners.end();
  for( iter = lib->game->luaKeyboardListeners.begin(); iter != iterEnd; ++iter ) {
    if( *iter == funcName ) {
      return 0;
    }
  }
  lib->game->luaKeyboardListeners.push_back( funcName );

  return 0;
}//LuaKeyboard_Listen
