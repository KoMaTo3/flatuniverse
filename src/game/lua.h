#pragma once

extern "C" {

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

}

#include "core/klib.h"
#include "core/file.h"
#include <string>

class Lua;

typedef bool  LUAFUNCPROC_RemoveObject      ( const std::string &name );
typedef Vec2  LUAFUNCPROC_GetObjectPos      ( const std::string &name );
typedef void  LUAFUNCPROC_SetObjectPos      ( const std::string &name, const Vec2 &pos );
typedef Dword LUAFUNCPROC_SetTimer          ( float time, const std::string &funcName );
typedef void  LUAFUNCPROC_LogWrite          ( const std::string &text );
typedef void  LUAFUNCPROC_CreateObject      ( const std::string &name, const std::string &parentName, const Vec3 &pos, const Vec2 &size, const std::string &textureName, const Vec4 &color );

extern LUAFUNCPROC_RemoveObject   *LUAFUNC_RemoveObject;
extern LUAFUNCPROC_GetObjectPos   *LUAFUNC_GetObjectPos;
extern LUAFUNCPROC_SetObjectPos   *LUAFUNC_SetObjectPos;
extern LUAFUNCPROC_SetTimer       *LUAFUNC_SetTimer;
extern LUAFUNCPROC_LogWrite       *LUAFUNC_LogWrite;
extern LUAFUNCPROC_CreateObject   *LUAFUNC_CreateObject;


//callbacks
void LUACALLBACK_Timer  ( Lua *lua, Dword id, const std::string &funcName );



class Lua
{
private:
  lua_State *luaState;
  static File log;

public:
  Lua();
  virtual ~Lua();

  bool  Init          ();
  void  Destroy       ();
  bool  RunFile       ( const std::string &fileName );
  bool  CallFunction  ( const std::string &funcName );

  void  ShowError     ( const std::string &comment, const std::string &errorName );

  static int ErrorHandler       ( lua_State *lua );

  //Lua-функции
  static int LUA_Alert          ( lua_State *lua );
  static int LUA_ObjectRemove   ( lua_State *lua );
  static int LUA_GetObjectPos   ( lua_State *lua );
  static int LUA_SetObjectPos   ( lua_State *lua );
  static int LUA_SetTimer       ( lua_State *lua );
  static int LUA_LogWrite       ( lua_State *lua );
  static int LUA_CreateObject   ( lua_State *lua );

  static void LUACALLBACK_Timer ( Lua *lua, Dword id, const std::string &funcName );
};
