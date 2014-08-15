#pragma once

extern "C" {

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

}

#include "core/klib.h"
#include "core/variable.h"
#include "core/file.h"
#include <string>

#define DEBU

class Lua;
class Game;

//typedef bool  LUAFUNCPROC_RemoveObject      ( const std::string &name );

//extern LUAFUNCPROC_RemoveObject     *LUAFUNC_RemoveObject;

//callbacks
//void LUACALLBACK_Timer            ( Lua *lua, Dword id, const std::string &funcName, const int luaFunctionId );


//очерёдность применения атрибутов при вызове ObjectAttr({...})
enum {
  LUA_OBJECT_ATTRS_PRIORITET_RENDERABLE = 0,
  LUA_OBJECT_ATTRS_PRIORITET_COLLISION,
  LUA_OBJECT_ATTRS_PRIORITET_COLLISION_SIZE,
  LUA_OBJECT_ATTRS_PRIORITET_LIGHTBLOCKBYCOLLISION,
  LUA_OBJECT_ATTRS_PRIORITET_TRIGGER,
  LUA_OBJECT_ATTRS_PRIORITET_LIGHTPOINT,
  LUA_OBJECT_ATTRS_PRIORITET_DEFAULT,
  __LUA_OBJECT_ATTRS_PRIORITET_COUNT
};


class Lua
{
private:
  lua_State *luaState;

public:
  static File log;

  Lua();
  virtual ~Lua();

  bool  Init          ( Game *game );
  void  Destroy       ();
  bool  RunFile       ( const std::string &fileName );
  bool  RunScript     ( const std::string &script );
  bool  CallFunction  ( const std::string &funcName );
  bool  CallTableTableFunction    ( const std::string &table, const std::string &key, const std::string &function );
  int   GetStackParmsCount();
  void  Unref         ( const int referenceId );
  static int  GetColor( lua_State *lua, int stackIndex, FU_OUT Vec4& color );
  bool  GetBoolean    ( const int stackIndex, const bool removeFromStack = false );
  void  Pop           ( const int count );
  void  DumpStack     ();

  void  ShowError     ( const std::string &comment, const std::string &errorName );

  static int ErrorHandler       ( lua_State *lua );

  //Lua-функции
  //static int LUA_Alert            ( lua_State *lua );

  //callbacks
  //static void LUACALLBACK_Timer           ( Lua *lua, Dword id, const std::string &funcName, const int luaFunctionId );

  static VariableAttributesListByPrioritet attrsListByPrioritet;

private:
  void        LoadLibs();
};
