#include "lua.h"
#include "core/filemanager.h"
#include "windows.h"
#include "luastatecheck.h"


File Lua::log;
Lua *__LuaCurrentContext = NULL;

LUAFUNCPROC_RemoveObject      *LUAFUNC_RemoveObject         = NULL;
LUAFUNCPROC_GetObjectPos      *LUAFUNC_GetObjectPos         = NULL;
LUAFUNCPROC_SetObjectPos      *LUAFUNC_SetObjectPos         = NULL;
LUAFUNCPROC_SetTimer          *LUAFUNC_SetTimer             = NULL;
LUAFUNCPROC_LogWrite          *LUAFUNC_LogWrite             = NULL;
LUAFUNCPROC_CreateObject      *LUAFUNC_CreateObject         = NULL;
LUAFUNCPROC_ListenKeyboard    *LUAFUNC_ListenKeyboard       = NULL;
LUAFUNCPROC_ListenMouseKey    *LUAFUNC_ListenMouseKey       = NULL;
LUAFUNCPROC_GameExit          *LUAFUNC_GameExit             = NULL;
LUAFUNCPROC_GetMousePos       *LUAFUNC_GetMousePos          = NULL;
LUAFUNCPROC_GetCameraPos      *LUAFUNC_GetCameraPos         = NULL;
LUAFUNCPROC_GetWindowSize     *LUAFUNC_GetWindowSize        = NULL;
LUAFUNCPROC_ObjectAddTrigger  *LUAFUNC_ObjectAddTrigger     = NULL;
LUAFUNCPROC_SetCamera         *LUAFUNC_SetCamera            = NULL;
LUAFUNCPROC_GetCamera         *LUAFUNC_GetCamera            = NULL;
LUAFUNCPROC_ClearScene        *LUAFUNC_ClearScene           = NULL;
LUAFUNCPROC_GetRandomSeed     *LUAFUNC_GetRandomSeed        = NULL;

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

  lua_register( this->luaState, "Alert",            Lua::LUA_Alert );
  lua_register( this->luaState, "ObjectRemove",     Lua::LUA_ObjectRemove );
  lua_register( this->luaState, "ObjectGetPos",     Lua::LUA_GetObjectPos );
  lua_register( this->luaState, "ObjectSetPos",     Lua::LUA_SetObjectPos );
  lua_register( this->luaState, "SetTimer",         Lua::LUA_SetTimer );
  lua_register( this->luaState, "LogWrite",         Lua::LUA_LogWrite );
  lua_register( this->luaState, "ObjectCreate",     Lua::LUA_CreateObject );
  lua_register( this->luaState, "ListenKeyboard",   Lua::LUA_ListenKeyboard );
  lua_register( this->luaState, "ListenMouseKey",   Lua::LUA_ListenMouseKey );
  lua_register( this->luaState, "GameExit",         Lua::LUA_GameExit );
  lua_register( this->luaState, "GetMousePos",      Lua::LUA_GetMousePos );
  lua_register( this->luaState, "GetCameraPos",     Lua::LUA_GetCameraPos );
  lua_register( this->luaState, "GetWindowSize",    Lua::LUA_GetWindowSize );
  lua_register( this->luaState, "ObjectAddTrigger", Lua::LUA_ObjectAddTrigger );
  lua_register( this->luaState, "SetCamera",        Lua::LUA_SetCamera );
  lua_register( this->luaState, "GetCamera",        Lua::LUA_GetCamera );
  lua_register( this->luaState, "ClearScene",       Lua::LUA_ClearScene );
  lua_register( this->luaState, "GetRandomSeed",    Lua::LUA_GetRandomSeed );
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
  LuaStateCheck state( this->luaState );
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
  LuaStateCheck state( lua->luaState );
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
  LUACALLBACK_ListenKeyboard
=============
*/
void LUACALLBACK_ListenKeyboard( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed )
{
  Lua::LUACALLBACK_ListenKeyboard( lua, funcName, keyId, isPressed );
}//LUACALLBACK_ListenKeyboard



/*
=============
  LUACALLBACK_ListenMouseKey
=============
*/
void LUACALLBACK_ListenMouseKey( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed )
{
  Lua::LUACALLBACK_ListenMouseKey( lua, funcName, keyId, isPressed );
}//LUACALLBACK_ListenMouseKey



/*
=============
  LUACALLBACK_ListenKeyboard
=============
*/
void Lua::LUACALLBACK_ListenKeyboard( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed )
{
  LuaStateCheck state( lua->luaState );
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenKeyboard => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  lua_pushinteger( lua->luaState, keyId ); //stack: funcName keyId
  lua_pushboolean( lua->luaState, isPressed ); //stack: funcName keyId isPressed
  if( lua_pcall( lua->luaState, 2, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenKeyboard => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_ListenKeyboard



/*
=============
  LUACALLBACK_ListenMouseKey
=============
*/
void Lua::LUACALLBACK_ListenMouseKey( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed )
{
  LuaStateCheck state( lua->luaState );
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenMouseKey => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  lua_pushinteger( lua->luaState, keyId ); //stack: funcName keyId
  lua_pushboolean( lua->luaState, isPressed ); //stack: funcName keyId isPressed
  if( lua_pcall( lua->luaState, 2, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenMouseKey => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_ListenMouseKey





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



/*
=============
  LUA_ListenKeyboard
=============
*/
int Lua::LUA_ListenKeyboard( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ListenKeyboard => not enough parameters" );
    return 0;
  }
  std::string funcName = lua_tostring( lua, 1 );
  LUAFUNC_ListenKeyboard( funcName );
  return 0;
}//LUA_ListenKeyboard



/*
=============
  LUA_ListenMouseKey
=============
*/
int Lua::LUA_ListenMouseKey( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ListenMouseKey => not enough parameters" );
    return 0;
  }
  std::string funcName = lua_tostring( lua, 1 );
  LUAFUNC_ListenMouseKey( funcName );
  return 0;
}//LUA_ListenMouseKey



/*
=============
  LUA_GameExit
=============
*/
int Lua::LUA_GameExit( lua_State *lua )
{
  LUAFUNC_GameExit();
  return 0;
}//LUA_GameExit



/*
=============
  LUA_GetMousePos
=============
*/
int Lua::LUA_GetMousePos( lua_State *lua )
{
  Vec2 pos = LUAFUNC_GetMousePos();
  lua_pushinteger( lua, ( int ) pos.x );
  lua_pushinteger( lua, ( int ) pos.y );

  return 2;
}//LUA_GetMousePos



/*
=============
  LUA_GetCameraPos
=============
*/
int Lua::LUA_GetCameraPos( lua_State *lua )
{
  Vec2 pos = LUAFUNC_GetCameraPos();
  lua_pushnumber( lua, pos.x );
  lua_pushnumber( lua, pos.y );

  return 2;
}//LUA_GetCameraPos



/*
=============
  LUA_GetWindowSize
=============
*/
int Lua::LUA_GetWindowSize( lua_State *lua )
{
  Size size = LUAFUNC_GetWindowSize();
  lua_pushinteger( lua, size.width );
  lua_pushinteger( lua, size.height );

  return 2;
}//LUA_GetWindowSize



/*
=============
  LUA_ObjectAddTrigger
=============
*/
int Lua::LUA_ObjectAddTrigger( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAddTrigger => not enough parameters" );
    return 0;
  }
  std::string triggerName = lua_tostring( lua, 1 );
  std::string funcName    = lua_tostring( lua, 2 );
  LUAFUNC_ObjectAddTrigger( triggerName, funcName );
  return 0;
}//LUA_ObjectAddTrigger



/*
=============
  LUACALLBACK_ObjectTrigger
=============
*/
void Lua::LUACALLBACK_ObjectTrigger( Lua *lua, const std::string &funcName, const std::string &triggerName, const std::string &objectName, bool isInTrigger )
{
  LuaStateCheck state( lua->luaState );
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ObjectTrigger => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  __log.PrintInfo( Filelevel_DEBUG, "Lua::LUACALLBACK_ObjectTrigger => funcName[%s] triggerName[%s] objectName[%s] isInTrigger[%d]", funcName.c_str(), triggerName.c_str(), objectName.c_str(), isInTrigger );

  lua_pushstring  ( lua->luaState, triggerName.c_str() ); //stack: funcName triggerName
  lua_pushstring  ( lua->luaState, objectName.c_str() );  //stack: funcName triggerName objectName
  lua_pushboolean ( lua->luaState, isInTrigger );         //stack: funcName triggerName objectName isInTrigger

  if( lua_pcall( lua->luaState, 3, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ObjectTrigger => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_ObjectTrigger




/*
=============
  LUACALLBACK_ObjectTrigger
=============
*/
void LUACALLBACK_ObjectTrigger( Lua *lua, const std::string &funcName, const std::string &triggerName, const std::string &objectName, bool isInTrigger )
{
  Lua::LUACALLBACK_ObjectTrigger( lua, funcName, triggerName, objectName, isInTrigger );
}//LUACALLBACK_Timer



/*
=============
  LUA_SetCamera
=============
*/
int Lua::LUA_SetCamera( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_SetCamera => not enough parameters" );
    return 0;
  }
  std::string name = lua_tostring( lua, 1 );
  LUAFUNC_SetCamera( name );
  return 0;
}//LUA_SetCamera



/*
=============
  LUA_GetCamera
=============
*/
int Lua::LUA_GetCamera( lua_State *lua )
{
  std::string name = LUAFUNC_GetCamera();
  lua_pushstring( lua, name.c_str() );
  return 1;
}//LUA_GetCamera



/*
=============
  LUA_ClearScene
=============
*/
int Lua::LUA_ClearScene( lua_State *lua )
{
  LUAFUNC_ClearScene();
  return 0;
}//LUA_ClearScene



/*
=============
  LUA_GetRandomSeed
=============
*/
int Lua::LUA_GetRandomSeed( lua_State *lua )
{
  int seed = rand();
  lua_pushinteger( lua, seed );
  return 1;
}//LUA_GetRandomSeed
