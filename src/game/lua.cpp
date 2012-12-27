#include "lua.h"
#include "core/file.h"
#include "core/filemanager.h"
#include "windows.h"


Lua *__LuaCurrentContext = NULL;

LUAFUNCPROC_RemoveObject *LUAFUNC_RemoveObject = NULL;
LUAFUNCPROC_GetObjectPos *LUAFUNC_GetObjectPos = NULL;



Lua::Lua()
:luaState( NULL )
{
}//constructor



Lua::~Lua()
{
  this->Destroy();
}//destructor






/*
=============
  Init
=============
*/
bool Lua::Init()
{
  if( this->luaState )
  {
    __log.PrintInfo( Filelevel_WARNING, "Lua::Init => already initialized" );
    return false;
  }

  this->luaState = lua_open();
  //luaL_openlibs( this->luaState );

  lua_register( this->luaState, "Alert", LUA_Alert );
  lua_register( this->luaState, "ObjectRemove", LUA_ObjectRemove );
  lua_register( this->luaState, "ObjectGetPos", LUA_GetObjectPos );
  lua_atpanic( this->luaState, ErrorHandler );

  __log.PrintInfo( Filelevel_DEBUG, "Lua::Init => initialized [x%X]", this->luaState );

  __LuaCurrentContext = this;

  return true;
}//Init




/*
=============
  Destroy
=============
*/
void Lua::Destroy()
{
  if( this->luaState )
  {
    lua_close( this->luaState );
    this->luaState = NULL;
    __log.PrintInfo( Filelevel_DEBUG, "Lua::Destroy => destroyed" );
  }

  if( __LuaCurrentContext == this )
    __LuaCurrentContext = NULL;
}//Destroy




/*
=============
  RunFile
=============
*/
bool Lua::RunFile( const std::string &fileName )
{
  if( !this->luaState )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunFile( '%s' ) => lua-state not initialized", fileName.c_str() );
    return false;
  }

  memory data;
  if( !__fileManager->GetFile( fileName, data, true ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunFile( '%s' ) => file not found", fileName.c_str() );
    return false;
  }

  if( luaL_dostring( this->luaState, data.getData() ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunFile( '%s' ) => luaL_dostring failed", fileName.c_str() );
    return false;
  }

  __log.PrintInfo( Filelevel_DEBUG, "Lua::RunFile( '%s' ) => done", fileName.c_str() );
  return true;
}//RunFile




/*
=============
  ShowError
=============
*/
void Lua::ShowError( const std::string &comment, const std::string &errorName )
{
  __log.PrintInfo( Filelevel_ERROR, "Lua [%s] %s", errorName.c_str(), comment.c_str() );
}//ShowError



/*
=============
  ErrorHandler
=============
*/
int Lua::ErrorHandler( lua_State *lua )
{
  __log.PrintInfo( Filelevel_ERROR, "Lua::ErrorHandler" );
  return 0;
}//ErrorHandler




/////////////////////
// Функционал
/////////////////////
int Lua::LUA_Alert( lua_State *lua )
{
  ::MessageBox( NULL, lua_tostring( lua, 2 ), lua_tostring( lua, 1 ), MB_OK );
  return 0;
}//LUA_Alert



/*
=============
  LUA_ObjectRemove
=============
*/
int Lua::LUA_ObjectRemove( lua_State *lua )
{
  std::string objectName = lua_tostring( lua, 1 );
  LUAFUNC_RemoveObject( objectName );
  return 0;
}//LUA_ObjectRemove



/*
=============
  LUA_GetObjectPos
=============
*/
int Lua::LUA_GetObjectPos( lua_State *lua )
{
  std::string objectName = lua_tostring( lua, 1 );
  Vec2 pos = LUAFUNC_GetObjectPos( objectName );
  char t[ 1024 ];

  sprintf_s( t, 1024, "%3.3f", pos.x );
  lua_pushstring( lua, t );

  sprintf_s( t, 1024, "%3.3f", pos.y );
  lua_pushstring( lua, t );

  return 2;
}//LUA_ObjectRemove
