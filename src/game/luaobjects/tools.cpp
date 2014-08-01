#include "tools.h"
#include "game/game.h"
#include <time.h>


using namespace Engine;


int Engine::LuaTools_GetRandomSeed( lua_State *lua ) {
  int seed = rand();
  lua_pushinteger( lua, seed );
  return 1;
}//LuaScene_Clear


int Engine::LuaTools_GetTime( lua_State *lua ) {
  int argc = lua_gettop( lua );
  if( argc < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaTools_GetTime => use: Tools.GetTime( formatString )" );
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
}//LuaScene_Clear
