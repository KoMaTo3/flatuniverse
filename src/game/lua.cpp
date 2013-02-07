#include "lua.h"
#include "core/filemanager.h"
#include "windows.h"
#include "luastatecheck.h"
#include "core/timer.h"
#include "core/tools.h"


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
LUAFUNCPROC_ListenMouseMove   *LUAFUNC_ListenMouseMove      = NULL;
LUAFUNCPROC_GameExit          *LUAFUNC_GameExit             = NULL;
LUAFUNCPROC_GetMousePos       *LUAFUNC_GetMousePos          = NULL;
LUAFUNCPROC_GetCameraPos      *LUAFUNC_GetCameraPos         = NULL;
LUAFUNCPROC_GetWindowSize     *LUAFUNC_GetWindowSize        = NULL;
LUAFUNCPROC_ObjectAddTrigger  *LUAFUNC_ObjectAddTrigger     = NULL;
LUAFUNCPROC_GuiAddTrigger     *LUAFUNC_GuiAddTrigger        = NULL;
LUAFUNCPROC_SetCamera         *LUAFUNC_SetCamera            = NULL;
LUAFUNCPROC_GetCamera         *LUAFUNC_GetCamera            = NULL;
LUAFUNCPROC_ClearScene        *LUAFUNC_ClearScene           = NULL;
LUAFUNCPROC_GetRandomSeed     *LUAFUNC_GetRandomSeed        = NULL;
LUAFUNCPROC_GuiGetText        *LUAFUNC_GuiGetText           = NULL;
LUAFUNCPROC_GuiSetText        *LUAFUNC_GuiSetText           = NULL;
LUAFUNCPROC_ObjectEnableRenderable  *LUAFUNC_ObjectEnableRenderable   = NULL;
LUAFUNCPROC_ObjectDisableRenderable *LUAFUNC_ObjectDisableRenderable  = NULL;
LUAFUNCPROC_ObjectEnableCollision   *LUAFUNC_ObjectEnableCollision    = NULL;
LUAFUNCPROC_ObjectDisableCollision  *LUAFUNC_ObjectDisableCollision   = NULL;
LUAFUNCPROC_ObjectEnableTrigger     *LUAFUNC_ObjectEnableTrigger      = NULL;
LUAFUNCPROC_ObjectDisableTrigger    *LUAFUNC_ObjectDisableTrigger     = NULL;
LUAFUNCPROC_GuiGetChecked     *LUAFUNC_GuiGetChecked        = NULL;
LUAFUNCPROC_GetCollisionStatic*LUAFUNC_GetCollisionStatic   = NULL;
LUAFUNCPROC_SetCollisionStatic*LUAFUNC_SetCollisionStatic   = NULL;
LUAFUNCPROC_DebugRender       *LUAFUNC_DebugRender          = NULL;
LUAFUNCPROC_GetObjectByPoint  *LUAFUNC_GetObjectByPoint     = NULL;
LUAFUNCPROC_SetGuiVisibility  *LUAFUNC_SetGuiVisibility     = NULL;
LUAFUNCPROC_SelectObject      *LUAFUNC_SelectObject         = NULL;
LUAFUNCPROC_GetSelectedObject *LUAFUNC_GetSelectedObject    = NULL;
LUAFUNCPROC_GuiAttr           *LUAFUNC_GuiAttr              = NULL;
LUAFUNCPROC_LoadScript        *LUAFUNC_LoadScript           = NULL;
LUAFUNCPROC_ObjectAttr        *LUAFUNC_ObjectAttr           = NULL;

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
  lua_register( this->luaState, "ListenMouseMove",  Lua::LUA_ListenMouseMove );
  lua_register( this->luaState, "GameExit",         Lua::LUA_GameExit );
  lua_register( this->luaState, "GetMousePos",      Lua::LUA_GetMousePos );
  lua_register( this->luaState, "GetCameraPos",     Lua::LUA_GetCameraPos );
  lua_register( this->luaState, "GetWindowSize",    Lua::LUA_GetWindowSize );
  lua_register( this->luaState, "ObjectAddTrigger", Lua::LUA_ObjectAddTrigger );
  lua_register( this->luaState, "GuiAddTrigger",    Lua::LUA_GuiAddTrigger );
  lua_register( this->luaState, "SetCamera",        Lua::LUA_SetCamera );
  lua_register( this->luaState, "SetGuiVisibility", Lua::LUA_SetGuiVisibility );
  lua_register( this->luaState, "GetCamera",        Lua::LUA_GetCamera );
  lua_register( this->luaState, "ClearScene",       Lua::LUA_ClearScene );
  lua_register( this->luaState, "GetRandomSeed",    Lua::LUA_GetRandomSeed );
  lua_register( this->luaState, "GuiGetText",       Lua::LUA_GuiGetText );
  lua_register( this->luaState, "GuiSetText",       Lua::LUA_GuiSetText );
  lua_register( this->luaState, "ObjectRenderable", Lua::LUA_ObjectRenderable );
  lua_register( this->luaState, "ObjectCollision",  Lua::LUA_ObjectCollision );
  lua_register( this->luaState, "ObjectTrigger",    Lua::LUA_ObjectTrigger );
  lua_register( this->luaState, "GuiGetChecked",    Lua::LUA_GuiGetChecked );
  lua_register( this->luaState, "CollisionGetStatic", Lua::LUA_GetCollisionStatic );
  lua_register( this->luaState, "CollisionSetStatic", Lua::LUA_SetCollisionStatic );
  lua_register( this->luaState, "DebugRender",      Lua::LUA_DebugRender );
  lua_register( this->luaState, "GetObjectByPoint", Lua::LUA_GetObjectByPoint );
  lua_register( this->luaState, "SelectObject",     Lua::LUA_SelectObject );
  lua_register( this->luaState, "GetSelectedObject",Lua::LUA_GetSelectedObject );
  lua_register( this->luaState, "GuiAttr",          Lua::LUA_GuiAttr );
  lua_register( this->luaState, "LoadScript",       Lua::LUA_LoadScript );
  lua_register( this->luaState, "ObjectAttr",       Lua::LUA_ObjectAttr );
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
  sTimer.Update();
  sTimer.Update();
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
  LUA_GetCollisionStatic
=============
*/
int Lua::LUA_GetCollisionStatic( lua_State *lua )
{
  std::string objectName = lua_tostring( lua, 1 );
  bool isStatic = LUAFUNC_GetCollisionStatic( objectName );
  lua_pushboolean( lua, isStatic );

  return 1;
}//LUA_GetCollisionStatic



/*
=============
  LUA_SetCollisionStatic
=============
*/
int Lua::LUA_SetCollisionStatic( lua_State *lua )
{
  std::string objectName = lua_tostring( lua, 1 );
  LUAFUNC_SetCollisionStatic( objectName, lua_toboolean( lua, 2 ) ? true : false );
  return 0;
}//LUA_SetCollisionStatic



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
  LUACALLBACK_ListenMouseMove
=============
*/
void LUACALLBACK_ListenMouseMove( Lua *lua, const std::string &funcName, const Vec2 &pos )
{
  Lua::LUACALLBACK_ListenMouseMove( lua, funcName, pos );
}//LUACALLBACK_ListenMouseMove



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
  LUACALLBACK_ListenMouseMove
=============
*/
void Lua::LUACALLBACK_ListenMouseMove( Lua *lua, const std::string &funcName, const Vec2 &pos )
{
  LuaStateCheck state( lua->luaState );
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenMouseMove => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  lua_pushnumber( lua->luaState, pos.x ); //stack: funcName x
  lua_pushnumber( lua->luaState, pos.y ); //stack: funcName x y
  if( lua_pcall( lua->luaState, 2, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_ListenMouseMove => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_ListenMouseMove





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
  if( parmsCount < 4 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_CreateObject => not enough parameters" );
    return 0;
  }
  std::string objectName = lua_tostring( lua, 1 );
  Vec3 pos( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ), ( float ) lua_tonumber( lua, 4 ) );
  LUAFUNC_CreateObject( objectName, pos );
  return 0;
}//LUA_CreateObject



/*
=============
  LUA_ObjectRenderable

  1:    bool    isEnable
  2:    string  objectName
  3:    string  texture
  4-5:  Vec2    size
  6-9:  Vec4    color
=============
*/
int Lua::LUA_ObjectRenderable( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectRenderable => not enough parameters" );
    return 0;
  }
  int enable = lua_toboolean( lua, 1 );
  std::string objectName = lua_tostring( lua, 2 );
  if( enable ) {  //enable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectRenderable => enable" );
    if( parmsCount < 2 + 3 ) {
      __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectRenderable => not enough parameters" );
      return 0;
    }
    std::string texture = lua_tostring( lua, 3 );
    Vec2 size( ( float ) lua_tonumber( lua, 4 ), ( float ) lua_tonumber( lua, 5 ) );
    Vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
    if( parmsCount >= 2 + 3 + 4 ) { //color
      color.Set( ( float ) lua_tonumber( lua, 6 ), ( float ) lua_tonumber( lua, 7 ), ( float ) lua_tonumber( lua, 8 ), ( float ) lua_tonumber( lua, 9 ) );
    }
    LUAFUNC_ObjectEnableRenderable( objectName, texture, size, color );
  } else {        //disable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectRenderable => disable" );
    LUAFUNC_ObjectDisableRenderable( objectName );
  }
  return 0;
}//LUA_ObjectRenderable



/*
=============
  LUA_ObjectCollision

  1:    bool    isEnable
  2:    string  objectName
  3:    bool    isStatic
  4-5:  Vec2    size
  6-7:  Vec2    velocity
  8-9:  Vec2    acceleration
=============
*/
int Lua::LUA_ObjectCollision( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectCollision => not enough parameters" );
    return 0;
  }
  int enable = lua_toboolean( lua, 1 );
  std::string objectName = lua_tostring( lua, 2 );
  if( enable ) {  //enable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectCollision => enable, params[%d]", parmsCount );

    bool isStatic = true;
    Vec3 size( -1.0f, -1.0f, -1.0f );
    Vec3 velocity( 0.0f, 0.0f, 0.0f );
    Vec3 acceleration( 0.0f, 0.0f, 0.0f );
    enum Parameters {
      SET_ACCELERATION  = 2 + 1 + 2 + 2 + 2,
      SET_VELOCITY      = 2 + 1 + 2 + 2,
      SET_SIZE          = 2 + 1 + 2,
      SET_ISSTATIC      = 2 + 1,
    };

    switch( parmsCount ) {
      case SET_ACCELERATION: {
        acceleration.Set( ( float ) lua_tonumber( lua, SET_ACCELERATION - 1 ), ( float ) lua_tonumber( lua, SET_ACCELERATION - 0 ), 0.0f );
        __log.PrintInfo( Filelevel_DEBUG, ". acceleration[%3.3f; %3.3f]", acceleration.x, acceleration.y );
      }
      case SET_VELOCITY: {
        velocity.Set( ( float ) lua_tonumber( lua, SET_VELOCITY - 1 ), ( float ) lua_tonumber( lua, SET_VELOCITY - 0 ), 0.0f );
        __log.PrintInfo( Filelevel_DEBUG, ". velocity[%3.3f; %3.3f]", velocity.x, velocity.y );
      }
      case SET_SIZE: {
        size.Set( ( float ) lua_tonumber( lua, SET_SIZE - 1 ), ( float ) lua_tonumber( lua, SET_SIZE - 0 ), 0.0f );
        __log.PrintInfo( Filelevel_DEBUG, ". size[%3.3f; %3.3f]", size.x, size.y );
      }
      case SET_ISSTATIC: {
        isStatic = ( lua_toboolean( lua, SET_ISSTATIC ) ? true : false );
        __log.PrintInfo( Filelevel_DEBUG, ". isStatic[%d]", isStatic );
      }
    }//switch parmsCount
    LUAFUNC_ObjectEnableCollision( objectName, isStatic, size, velocity, acceleration );
  } else {        //disable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectCollision => disable" );
    LUAFUNC_ObjectDisableCollision( objectName );
  }
  return 0;
}//LUA_ObjectCollision



/*
=============
  LUA_ObjectTrigger

  1:    bool    isEnable
  2:    string  objectName
  3-4:  Vec2    size
=============
*/
int Lua::LUA_ObjectTrigger( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectTrigger => not enough parameters" );
    return 0;
  }
  int enable = lua_toboolean( lua, 1 );
  std::string objectName = lua_tostring( lua, 2 );
  if( enable ) {  //enable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectTrigger => enable, params[%d]", parmsCount );

    Vec3 size( -1.0f, -1.0f, -1.0f );
    enum Parameters {
      SET_SIZE  = 2 + 2,
    };

    switch( parmsCount ) {
      case SET_SIZE: {
        size.Set( ( float ) lua_tonumber( lua, SET_SIZE - 1 ), ( float ) lua_tonumber( lua, SET_SIZE - 0 ), 0.0f );
        __log.PrintInfo( Filelevel_DEBUG, ". size[%3.3f; %3.3f]", size.x, size.y );
      }
    }//switch parmsCount
    LUAFUNC_ObjectEnableTrigger( objectName, size );
  } else {        //disable
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectTrigger => disable" );
    LUAFUNC_ObjectDisableTrigger( objectName );
  }
  return 0;
}//LUA_ObjectTrigger



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
  LUA_ListenMouseMove
=============
*/
int Lua::LUA_ListenMouseMove( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ListenMouseMove => not enough parameters" );
    return 0;
  }
  std::string funcName = lua_tostring( lua, 1 );
  LUAFUNC_ListenMouseMove( funcName );
  return 0;
}//LUA_ListenMouseMove



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
  LUA_GuiAddTrigger
=============
*/
int Lua::LUA_GuiAddTrigger( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiAddTrigger => not enough parameters" );
    return 0;
  }
  std::string objectName  = lua_tostring( lua, 1 );
  std::string funcName    = lua_tostring( lua, 2 );
  LUAFUNC_GuiAddTrigger( objectName, funcName );
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
  LUACALLBACK_GuiTrigger
=============
*/
void Lua::LUACALLBACK_GuiTrigger( Lua *lua, const std::string &funcName, const std::string &objectName )
{
  LuaStateCheck state( lua->luaState );
  lua_getglobal( lua->luaState, funcName.c_str() ); //stack: funcName
  if( !lua_isfunction( lua->luaState, -1 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_GuiTrigger => '%s' is not a function", funcName.c_str() );
    lua_pop( lua->luaState, 1 );  //stack:
  }

  __log.PrintInfo( Filelevel_DEBUG, "Lua::LUACALLBACK_GuiTrigger => funcName[%s] objectName[%s]", funcName.c_str(), objectName.c_str() );

  lua_pushstring  ( lua->luaState, objectName.c_str() );  //stack: funcName objectName

  if( lua_pcall( lua->luaState, 1, 0, 0 ) )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUACALLBACK_GuiTrigger => error by calling function '%s'", funcName.c_str() );
    return;
  }
}//LUACALLBACK_GuiTrigger




/*
=============
  LUACALLBACK_ObjectTrigger
=============
*/
void LUACALLBACK_ObjectTrigger( Lua *lua, const std::string &funcName, const std::string &triggerName, const std::string &objectName, bool isInTrigger )
{
  Lua::LUACALLBACK_ObjectTrigger( lua, funcName, triggerName, objectName, isInTrigger );
}//LUACALLBACK_ObjectTrigger




/*
=============
  LUACALLBACK_GuiTrigger
=============
*/
void LUACALLBACK_GuiTrigger( Lua *lua, const std::string &funcName, const std::string &objectName )
{
  Lua::LUACALLBACK_GuiTrigger( lua, funcName, objectName );
}//LUACALLBACK_GuiTrigger



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
  LUA_SetGuiVisibility
=============
*/
int Lua::LUA_SetGuiVisibility( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_SetGuiVisibility => not enough parameters" );
    return 0;
  }
  int show = lua_toboolean( lua, 1 );
  LUAFUNC_SetGuiVisibility( show );
  return 0;
}//LUA_SetGuiVisibility



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



/*
=============
  LUA_LoadScript
=============
*/
int Lua::LUA_LoadScript( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_LoadScript => not enough parameters" );
    return 0;
  }
  std::string fileName = lua_tostring( lua, 1 );
  LUAFUNC_LoadScript( fileName );
  return 0;
}//LUA_LoadScript



/*
=============
  LUA_GuiGetText
=============
*/
int Lua::LUA_GuiGetText( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiGetText => not enough parameters" );
    return 0;
  }
  std::string guiName = lua_tostring( lua, 1 );
  std::string text = LUAFUNC_GuiGetText( guiName );

  lua_pushstring( lua, text.c_str() );
  return 1;
}//LUA_GuiGetText



/*
=============
  LUA_GuiSetText
=============
*/
int Lua::LUA_GuiSetText( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiSetText => not enough parameters" );
    return 0;
  }
  std::string guiName = lua_tostring( lua, 1 );
  std::string text    = lua_tostring( lua, 2 );
  LUAFUNC_GuiSetText( guiName, text );

  return 0;
}//LUA_GuiSetText



/*
=============
  LUA_GuiAttr
=============
*/
int Lua::LUA_GuiAttr( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiAttr => not enough parameters" );
    return 0;
  }

  std::string guiName   = lua_tostring( lua, 1 );
  std::string parameter = lua_tostring( lua, 2 );
  Variable val;

  int res = 0;
  if( parmsCount == 2 ) { //get
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_GuiAttr => get..." );
    LUAFUNC_GuiAttr( guiName, parameter, val, false );
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_GuiAttr => value type = %d", val.GetType() );
    switch( val.GetType() ) {
      case VariableType_NUMBER: {
        lua_pushnumber( lua, val.GetNumber() );
        res = 1;
        break;
      }
      case VariableType_STRING: {
        lua_pushstring( lua, val.GetString().c_str() );
        res = 1;
        break;
      }
      case VariableType_BOOLEAN: {
        lua_pushboolean( lua, val.GetBoolean() );
        res = 1;
        break;
      }
      default: {
        res = 0;
      }
    }
  } else {  //set
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_GuiAttr => set..." );
    if( lua_isboolean( lua, 3 ) ) {
      val.SetBoolean( lua_toboolean( lua, 3 ) != 0 );
    } else if( lua_isnumber( lua, 3 ) ) {
      val.SetNumber( ( float ) lua_tonumber( lua, 3 ) );
    } else if( lua_isstring( lua, 3 ) ) {
      val.SetString( lua_tostring( lua, 3 ) );
    } else {
      __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiAttr => set, unknown type" );
    }
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_GuiAttr => value['%s']", val.GetString().c_str() );
    LUAFUNC_GuiAttr( guiName, parameter, val, true );
  }

  return res;
}//LUA_GuiAttr



/*
=============
  LUA_ObjectAttr
=============
*/
int Lua::LUA_ObjectAttr( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount != 2 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAttr => not enough parameters" );
    return 0;
  }

  std::string objectName = lua_tostring( lua, 1 );

  int res = 0;
  VariableAttributesList setAttrs, getAttrs;
  if( lua_istable( lua, 2 ) ) {
    __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectAttr => start:" );

    lua_pushvalue( lua, -1 );
    lua_pushnil( lua );
    int num = 0;
    VariableAttribute *attr = NULL;
    while( lua_next( lua, -2 ) ) {
      lua_pushvalue( lua, -2 );

      if( lua_isnumber( lua, -1 ) ) { //get parameter
        if( lua_isstring( lua, -2 ) ) {
          __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_ObjectAttr => get '%s'", lua_tostring( lua, -2 ) );
          attr = new VariableAttribute();
          attr->name = lua_tostring( lua, -2 );
          getAttrs.push_back( attr );
        } else {
          __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAttr => wrong get-parameter" );
        }
      } else {
        if( lua_isstring( lua, -1 ) ) {
          attr = NULL;
          if( lua_isboolean( lua, -2 ) ) {  //set parameters...
            attr = new VariableAttribute();
            attr->value.SetBoolean( lua_toboolean( lua, -2 ) ? true : false );
          } else if( lua_isnil( lua, -2 ) || lua_isnone( lua, -2 ) ) {
            attr = new VariableAttribute();
            attr->value.SetNull();
          } else if( lua_isnumber( lua, -2 ) ) {
            attr = new VariableAttribute();
            attr->value.SetNumber( ( float ) lua_tonumber( lua, -2 ) );
          } else if( lua_isstring( lua, -2 ) ) {
            attr = new VariableAttribute();
            attr->value.SetString( lua_tostring( lua, -2 ) );
          } else {
            __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAttr => parameter's  key '%s' has bad value", attr->name.c_str() );
          }
          if( attr ) {
            attr->name = lua_tostring( lua, -1 );
            if(
              attr->name == "renderable"  ||
              attr->name == "collision"   ||
              attr->name == "trigger"
              )
              setAttrs.push_front( attr );
            else
              setAttrs.push_back( attr );
            __log.PrintInfo( Filelevel_DEBUG, ". '%s' => '%s'", attr->name.c_str(), attr->value.GetString().c_str() );
          }

        } else {
          __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAttr => parameter's key №%d is not a string" );
        }
      }

      //__log.PrintInfo( Filelevel_DEBUG, ". '%s' = '%s'\n", lua_tostring( lua, -1 ), lua_tostring( lua, -2 ) );
      lua_pop( lua, 2 );
      ++num;
    }//while

    LUAFUNC_ObjectAttr( objectName, setAttrs, getAttrs );

    if( getAttrs.size() ) { //return parameters
      __log.PrintInfo( Filelevel_DEBUG, ". getAttrs[%d]", getAttrs.size() );
      VariableAttributesList::iterator iter, iterEnd = getAttrs.end();
      VariableAttribute *var;
      for( iter = getAttrs.begin(); iter != iterEnd; ++iter ) {
        var = *iter;
        __log.PrintInfo( Filelevel_DEBUG, ". type[%d]", var->value.GetType() );
        switch( ( *iter )->value.GetType() ) {
          case VariableType_NUMBER: {
            __log.PrintInfo( Filelevel_DEBUG, ". number[%3.3f]", var->value.GetNumber() );
            lua_pushnumber( lua, var->value.GetNumber() );
            ++res;
            break;
          }
          case VariableType_STRING: {
            __log.PrintInfo( Filelevel_DEBUG, ". string['%s']", var->value.GetString().c_str() );
            lua_pushstring( lua, var->value.GetString().c_str() );
            ++res;
            break;
          }
          case VariableType_BOOLEAN: {
            __log.PrintInfo( Filelevel_DEBUG, ". bool[%d]", var->value.GetBoolean() );
            lua_pushboolean( lua, var->value.GetBoolean() );
            ++res;
            break;
          }
          case VariableType_NULL: {
            __log.PrintInfo( Filelevel_DEBUG, ". null" );
            lua_pushnil( lua );
            ++res;
            break;
          }
          case VariableType_VEC2: {
            __log.PrintInfo( Filelevel_DEBUG, ". vec2[%3.3f; %3.3f]", var->value.GetVec2().x, var->value.GetVec2().y );
            lua_pushnumber( lua, var->value.GetVec2().x );
            lua_pushnumber( lua, var->value.GetVec2().y );
            res += 2;
            break;
          }
          case VariableType_VEC3: {
            __log.PrintInfo( Filelevel_DEBUG, ". vec3[%3.3f; %3.3f; %3.3f]", var->value.GetVec3().x, var->value.GetVec3().y, var->value.GetVec3().z );
            lua_pushnumber( lua, var->value.GetVec3().x );
            lua_pushnumber( lua, var->value.GetVec3().y );
            lua_pushnumber( lua, var->value.GetVec3().z );
            res += 3;
            break;
          }
          case VariableType_VEC4: {
            __log.PrintInfo( Filelevel_DEBUG, ". vec4[%3.3f; %3.3f; %3.3f; %3.3f]", var->value.GetVec4().x, var->value.GetVec4().y, var->value.GetVec4().z, var->value.GetVec4().w );
            lua_pushnumber( lua, var->value.GetVec4().x );
            lua_pushnumber( lua, var->value.GetVec4().y );
            lua_pushnumber( lua, var->value.GetVec4().z );
            lua_pushnumber( lua, var->value.GetVec4().w );
            res += 4;
            break;
          }
        }//switch
      }//for
    }

    VariableAttributesList::iterator iter, iterEnd = setAttrs.end();
    for( iter = setAttrs.begin(); iter != iterEnd; ++iter ) {
      delete *iter;
    }
    setAttrs.clear();

    iterEnd = getAttrs.end();
    for( iter = getAttrs.begin(); iter != iterEnd; ++iter ) {
      delete *iter;
    }
    getAttrs.clear();

  } else {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_ObjectAttr => parameter 2: need table" );
  }

  return res;
}//LUA_ObjectAttr



/*
=============
  LUA_GuiGetChecked
=============
*/
int Lua::LUA_GuiGetChecked( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GuiGetChecked => not enough parameters" );
    return 0;
  }
  std::string guiName = lua_tostring( lua, 1 );
  __log.PrintInfo( Filelevel_DEBUG, "Lua::LUA_GuiGetChecked => object['%s']", guiName.c_str() );
  int checked = LUAFUNC_GuiGetChecked( guiName );

  lua_pushboolean( lua, checked );
  return 1;
}//LUA_GuiGetChecked



/*
=============
  LUA_DebugRender
=============
*/
int Lua::LUA_DebugRender( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_DebugRender => not enough parameters" );
    return 0;
  }
  int flags = lua_tointeger( lua, 1 );
  LUAFUNC_DebugRender( flags );
  return 0;
}//LUA_DebugRender



/*
=============
  LUA_GetObjectByPoint
=============
*/
int Lua::LUA_GetObjectByPoint( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 3 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_GetObjectByPoint => not enough parameters" );
    return 0;
  }
  int type = lua_tointeger( lua, 1 );
  Vec2 point;
  point.Set( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) );
  std::string objectName;
  if( parmsCount >= 4 ) {
    objectName = lua_tostring( lua, 4 );
  }
  objectName = LUAFUNC_GetObjectByPoint( type, point, objectName );
  lua_pushstring( lua, objectName.c_str() );

  return 1;
}//LUA_GetObjectByPoint



/*
=============
  LUA_SelectObject
=============
*/
int Lua::LUA_SelectObject( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 )
  {
    __log.PrintInfo( Filelevel_ERROR, "Lua::LUA_SelectObject => not enough parameters" );
    return 0;
  }
  std::string name = lua_tostring( lua, 1 );
  LUAFUNC_SelectObject( name );
  return 0;
}//LUA_SelectObject



/*
=============
  LUA_GetSelectedObject
=============
*/
int Lua::LUA_GetSelectedObject( lua_State *lua )
{
  std::string name = LUAFUNC_GetSelectedObject();
  lua_pushstring( lua, name.c_str() );  //stack: name
  return 1;
}//LUA_GetSelectedObject
