#include "core.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaCore_SetTimer( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CORE );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCore_SetTimer => use: Core.SetTimer( seconds, function|functionName )" );
    return 0;
  }

  float time            = ( float ) lua_tonumber( lua, 1 );
  std::string funcName  = "";
  int luaFunctionId     = 0;
  if( lua_isfunction( lua, 2 ) ) {
    lua_pushvalue( lua, 2 );
    luaFunctionId = luaL_ref( lua, LUA_REGISTRYINDEX );
  } else if( lua_isstring( lua, 2 ) ) {
    funcName = lua_tostring( lua, 2 );
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCore_SetTimer => bad 2nd parameter: must be function or string" );
    return 0;
  }
  int dontPause = false;

  if( parmsCount >= 3 ) {
    dontPause = lua_toboolean( lua, 3 );
  }

  //int id = LUAFUNC_SetTimer( period, funcName, dontPause ? true : false, luaFunctionId );

  Dword id, count = lib->game->luaTimers.size();
  bool setted = false;
  for( Dword q = 0; q < count; ++q ) {
    if( !lib->game->luaTimers[ q ].active ) {
      id = q;
      setted = true;
      break;
    }
  }
  if( !setted ) {
    lib->game->luaTimers.push_back( Game::GameLuaTimer() );
    id = lib->game->luaTimers.size() - 1;
  }
  lib->game->luaTimers[ id ].active    = 1;
  lib->game->luaTimers[ id ].time      = time;
  lib->game->luaTimers[ id ].funcName  = funcName;
  lib->game->luaTimers[ id ].dontPause = dontPause ? true : false;
  lib->game->luaTimers[ id ].id = id;
  lib->game->luaTimers[ id ].luaFunctionId = luaFunctionId;

  lua_pushinteger( lua, id );

  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaCore_SetTimer => timer id[%d] function id[%d] name['%s']", id, luaFunctionId, funcName.c_str() );

  return 1;
}//LuaCore_SetTimer


int Engine::LuaCore_StopTimer( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CORE );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaCore_SetTimer => use: Core.StopTimer( timerId )" );
    return 0;
  }

  Dword id = ( Dword ) lua_tonumber( lua, 1 );
  if( id < lib->game->luaTimers.size() ) {
    lib->game->luaTimers[ id ].active = false;
    if( lib->game->luaTimers[ id ].luaFunctionId ) {
      luaL_unref( lua, LUA_REGISTRYINDEX, lib->game->luaTimers[ id ].luaFunctionId );
      lib->game->luaTimers[ id ].luaFunctionId = 0;
    }
  }

  return 1;
}//LuaCore_StopTimer
