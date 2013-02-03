#include "lua.h"
#include "core/filemanager.h"
#include "windows.h"
#include "luastatecheck.h"
#include "core/timer.h"


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
  lua_register( this->luaState, "GuiAddTrigger",    Lua::LUA_GuiAddTrigger );
  lua_register( this->luaState, "SetCamera",        Lua::LUA_SetCamera );
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
// ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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



/*
=============
  LUA_GuiGetText
=============
*/
int Lua::LUA_GuiGetText( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  LUA_GuiGetChecked
=============
*/
int Lua::LUA_GuiGetChecked( lua_State *lua )
{
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
  int parmsCount = lua_gettop( lua ); //����� ����������
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
