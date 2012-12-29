#include "lua.h"
#include "core/filemanager.h"
#include "windows.h"


File Lua::log;
Lua *__LuaCurrentContext = NULL;

LUAFUNCPROC_RemoveObject  *LUAFUNC_RemoveObject         = NULL;
LUAFUNCPROC_GetObjectPos  *LUAFUNC_GetObjectPos         = NULL;
LUAFUNCPROC_SetObjectPos  *LUAFUNC_SetObjectPos         = NULL;
LUAFUNCPROC_SetTimer      *LUAFUNC_SetTimer             = NULL;
LUAFUNCPROC_LogWrite      *LUAFUNC_LogWrite             = NULL;
LUAFUNCPROC_CreateObject  *LUAFUNC_CreateObject         = NULL;

//LUACALLBACKPROC_Timer     *LUACALLBACK_Timer            = NULL;



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
  //luaopen_math( this->luaState );
  lua_pushcfunction( this->luaState, luaopen_math );
  lua_pcall( this->luaState, 0, 0, 0 );
  lua_pushcfunction( this->luaState, luaopen_string );
  lua_pcall( this->luaState, 0, 0, 0 );

  lua_register( this->luaState, "Alert", Lua::LUA_Alert );
  lua_register( this->luaState, "ObjectRemove", Lua::LUA_ObjectRemove );
  lua_register( this->luaState, "ObjectGetPos", Lua::LUA_GetObjectPos );
  lua_register( this->luaState, "ObjectSetPos", Lua::LUA_SetObjectPos );
  lua_register( this->luaState, "SetTimer",     Lua::LUA_SetTimer );
  lua_register( this->luaState, "LogWrite",     Lua::LUA_LogWrite );
  lua_register( this->luaState, "ObjectCreate", Lua::LUA_CreateObject );
  lua_atpanic( this->luaState, ErrorHandler );

  __log.PrintInfo( Filelevel_DEBUG, "Lua::Init => initialized [x%X]", this->luaState );

  __LuaCurrentContext = this;

  this->log.Open( "logs/lua.txt", File_mode_WRITE );
  this->log.EnableAutoFlush( true );

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
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunFile( '%s' ) => luaL_dostring failed:\n%s", fileName.c_str(), lua_tostring( this->luaState, -1 ) );
    return false;
  }

  __log.PrintInfo( Filelevel_DEBUG, "Lua::RunFile( '%s' ) => done", fileName.c_str() );
  return true;
}//RunFile




/*
=============
  CallFunction
=============
*/
bool Lua::CallFunction( const std::string &funcName )
{
  lua_getglobal( this->luaState, funcName.c_str() );  //stack: funcName
  if( !lua_isfunction( this->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::CallFunction => '%s' is not a function", funcName.c_str() );
    lua_pop( this->luaState, 1 ); //stack:
    return false;
  }

  if( lua_pcall( this->luaState, 0, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::CallFunction => error by calling function '%s': \n%s", funcName.c_str(), lua_tostring( this->luaState, -1 ) );
    return false;
  }

  return true;
}//CallFunction





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
  bool result = LUAFUNC_RemoveObject( objectName );
  lua_pushboolean( lua, result );

  return 1;
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
  lua_pushnumber( lua, pos.x );
  lua_pushnumber( lua, pos.y );

  return 2;
}//LUA_ObjectRemove



/*
=============
  LUA_SetObjectPos
=============
*/
int Lua::LUA_SetObjectPos( lua_State *lua )
{
  std::string objectName = lua_tostring( lua, 1 );
  LUAFUNC_SetObjectPos( objectName, Vec2( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) ) );
  return 0;
}//LUA_SetObjectPos



/*
=============
  LUA_LogWrite
=============
*/
int Lua::LUA_LogWrite( lua_State *lua )
{
  std::string text = lua_tostring( lua, 1 );

  Lua::log.PrintInfo( Filelevel_DEBUG, "Lua: %s", text.c_str() );

  return 0;
}//LUA_LogWrite



/*
=============
  LUA_SetTimer
=============
*/
int Lua::LUA_SetTimer( lua_State *lua )
{
  float period          = ( float ) lua_tonumber( lua, 1 );
  std::string funcName  = lua_tostring( lua, 2 );

  int id = LUAFUNC_SetTimer( period, funcName );
  lua_pushinteger( lua, id );

  return 1;
}//LUA_SetTimer



/*
=============
  LUACALLBACK_Timer
=============
*/
void Lua::LUACALLBACK_Timer( Lua *lua, Dword id, const std::string &funcName )
{
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_Timer => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  lua_pushinteger( lua->luaState, id ); //stack: funcName id
  if( lua_pcall( lua->luaState, 1, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_Timer => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_Timer



/*
=============
  LUACALLBACK_Timer
=============
*/
void LUACALLBACK_Timer( Lua *lua, Dword id, const std::string &funcName )
{
  Lua::LUACALLBACK_Timer( lua, id, funcName );
}//LUACALLBACK_Timer



/*
=============
  LUA_CreateObject
=============
*/
int Lua::LUA_CreateObject( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 8 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_CreateObject => not enough parameters" );
    return 0;
  }
  std::string objectName = lua_tostring( lua, 1 );
  std::string parentName = lua_tostring( lua, 2 );
  Vec3 pos( ( float ) lua_tonumber( lua, 3 ), ( float ) lua_tonumber( lua, 4 ), ( float ) lua_tonumber( lua, 5 ) );
  Vec2 size( ( float ) lua_tonumber( lua, 6 ), ( float ) lua_tonumber( lua, 7 ) );
  std::string textureName = lua_tostring( lua, 8 );
  //Vec4 color;
  //if( parmsCount >= 8 )
  LUAFUNC_CreateObject( objectName, parentName, pos, size, textureName, Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  return 0;
}//LUA_CreateObject
