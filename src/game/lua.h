#pragma once

extern "C" {

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

}

#include "core/klib.h"
#include <string>

typedef void LUAFUNCPROC_RemoveObject( const std::string &name );
typedef Vec2 LUAFUNCPROC_GetObjectPos( const std::string &name );

extern LUAFUNCPROC_RemoveObject *LUAFUNC_RemoveObject;
extern LUAFUNCPROC_GetObjectPos *LUAFUNC_GetObjectPos;



class Lua
{
private:
  lua_State *luaState;

public:
  Lua();
  virtual ~Lua();

  bool  Init    ();
  void  Destroy ();
  bool  RunFile ( const std::string &fileName );

  void  ShowError( const std::string &comment, const std::string &errorName );

  static int ErrorHandler     ( lua_State *lua );

  //Lua-функции
  static int LUA_Alert        ( lua_State *lua );
  static int LUA_ObjectRemove ( lua_State *lua );
  static int LUA_GetObjectPos ( lua_State *lua );
};
