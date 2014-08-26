#include "mouse.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaMouse_ListenKey( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_MOUSE );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaMouse_ListenKey => use: Mouse.ListenKey( functionName )" );
    return 0;
  }
  std::string funcName = lua_tostring( lua, 1 );

  Game::luaKeyboardListenersList::iterator iter, iterEnd = lib->game->luaMouseKeyListeners.end();
  for( iter = lib->game->luaMouseKeyListeners.begin(); iter != iterEnd; ++iter ) {
    if( *iter == funcName ) {
      return 0;
    }
  }
  lib->game->luaMouseKeyListeners.push_back( funcName );

  return 0;
}//LuaMouse_ListenKey


int Engine::LuaMouse_ListenMove( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_MOUSE );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaMouse_ListenMove => use: Mouse.ListenMove( functionName )" );
    return 0;
  }
  std::string funcName = lua_tostring( lua, 1 );

  Game::luaKeyboardListenersList::iterator iter, iterEnd = lib->game->luaMouseMoveListeners.end();
  for( iter = lib->game->luaMouseMoveListeners.begin(); iter != iterEnd; ++iter ) {
    if( *iter == funcName ) {
      return 0;
    }
  }
  lib->game->luaMouseMoveListeners.push_back( funcName );

  return 0;
}//LuaMouse_ListenMove


int Engine::LuaMouse_GetPos( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_MOUSE );

  const Vec2 pos = lib->game->core->mouse.GetCursorPosition();
  lua_pushinteger( lua, ( int ) pos.x );
  lua_pushinteger( lua, ( int ) pos.y );

  return 2;
}//LuaMouse_GetPos
