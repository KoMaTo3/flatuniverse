#include "tools.h"
#include "game/game.h"
#include <time.h>


using namespace Engine;


int Engine::LuaTools_GetRandomSeed( lua_State *lua ) {
  int seed = rand();
  lua_pushinteger( lua, seed );
  return 1;
}//LuaTools_GetRandomSeed


int Engine::LuaTools_GetTimeFormatted( lua_State *lua ) {
  int argc = lua_gettop( lua );
  if( argc < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaTools_GetTime => use: Tools.LuaTools_GetTimeFormatted( formatString )" );
    return 0;
  }
  const char *format = lua_tostring( lua, 1 );

  time_t currentTime = time( NULL );
  tm localTime;
  localtime_s( &localTime, &currentTime );
  char date[ 1024 ];
  strftime( date, sizeof( date ), format, &localTime );

  lua_pushstring( lua, date );

  return 1;
}//LuaTools_GetTimeFormatted


int Engine::LuaTools_GetClipboard( lua_State *lua ) {
  std::string text = "";

#ifdef WIN32
  OpenClipboard( NULL );
  HANDLE hClipboard = GetClipboardData( CF_TEXT );
  HANDLE hMem = GlobalLock( hClipboard );
  text = ( char* ) hClipboard;
  GlobalUnlock( hClipboard );
#endif

  lua_pushstring( lua, text.c_str() );

  return 1;
}//LuaTools_GetClipboard


int Engine::LuaTools_SetClipboard( lua_State *lua ) {
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaTools_SetClipboard => use: Tools.SetClipboard( string )" );
    return 0;
  }

  std::string text = lua_tostring( lua, 1 );

#ifdef WIN32
  const size_t length = text.size() + 1;
  HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE, length );
  memcpy( GlobalLock( hMem ), text.c_str(), length );
  GlobalUnlock( hMem );
  OpenClipboard( NULL );
  EmptyClipboard();
  SetClipboardData( CF_TEXT, hMem );
  CloseClipboard();
#endif

  return 0;
}//LuaTools_SetClipboard


int Engine::LuaTools_MakeScreenshot( lua_State *lua ) {
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaTools_MakeScreenshot => use: Tools.MakeScreenshot( fileName )" );
    return 0;
  }

  std::string fileName = lua_tostring( lua, 1 );
  bool beforeLight = false;
  if( parmsCount > 1 ) {
    beforeLight = lua_toboolean( lua, 2 ) ? true : false;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_TOOLS );

  lib->game->core->MakeScreenshot( fileName, beforeLight );

  return 0;
}//LuaTools_MakeScreenshot
