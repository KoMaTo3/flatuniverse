#include "luastatecheck.h"
#include "core/file.h"
#include "lua.h"



LuaStateCheck::LuaStateCheck( lua_State *setLuaState )
:luaState( setLuaState )
{
  if( !setLuaState )
    __log.PrintInfo( Filelevel_WARNING, "LuaStateCheck => lua is NULL" );
  else
    this->parmsCount = lua_gettop( setLuaState );
}


LuaStateCheck::~LuaStateCheck()
{
  if( !this->luaState )
    __log.PrintInfo( Filelevel_WARNING, "~LuaStateCheck => lua is NULL" );
  else
    if( this->parmsCount != lua_gettop( this->luaState ) )
      __log.PrintInfo( Filelevel_WARNING, "~LuaStateCheck => stack are crashed" );
}