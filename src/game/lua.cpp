#include "lua.h"
#include "core/filemanager.h"
#include "windows.h"
#include "luastatecheck.h"
#include "core/timer.h"
#include "core/tools.h"
#include "core/debugrenderer.h"
#include "core/tools.h"
#include "luaobjects/object.h"
#include "luaobjects/debug.h"
#include "luaobjects/core.h"
#include "luaobjects/keyboard.h"
#include "luaobjects/mouse.h"
#include "luaobjects/camera.h"
#include "luaobjects/config.h"
#include "luaobjects/scene.h"
#include "luaobjects/tools.h"


File Lua::log;
Lua *__LuaCurrentContext = NULL;
DebugRenderer* __debugRender = NULL;
VariableAttributesListByPrioritet Lua::attrsListByPrioritet;

//LUAFUNCPROC_RemoveObject      *LUAFUNC_RemoveObject         = NULL;



Lua::Lua()
:luaState( NULL )
{
  __debugRender = new DebugRenderer();
}//constructor



Lua::~Lua()
{
  DEF_DELETE( __debugRender );
  this->Destroy();
}//destructor






/*
=============
  Init
=============
*/
bool Lua::Init( Game *game )
{
  if( this->luaState )
  {
    __log.PrintInfo( Filelevel_WARNING, "Lua::Init => already initialized" );
    return false;
  }

  this->luaState = luaL_newstate();
  this->LoadLibs();

  //lua_register( this->luaState, "Alert",            Lua::LUA_Alert );

  //external libraries
  static const luaL_Reg none_functions[] = {
    { NULL, NULL }
  };
  //Object
  static const luaL_Reg object_functions[] = {
    { "AddTag", &Engine::LuaObject_AddTag },
    { "AddTrigger", &Engine::LuaObject_AddTrigger },
    { "Attr", &Engine::LuaObject_Attr },
    { "Destroy", &Engine::LuaObject_Destroy },
    { "GetChilds", &Engine::LuaObject_GetChilds },
    { "GetId", &Engine::LuaObject_GetId },
    { "GetName", &Engine::LuaObject_GetName },
    { "GetNameFull", &Engine::LuaObject_GetNameFull },
    { "GetParent", &Engine::LuaObject_GetParent },
    { "GetParentNameFull", &Engine::LuaObject_GetParentNameFull },
    { "GetPos", &Engine::LuaObject_GetPos },
    { "HasTag", &Engine::LuaObject_HasTag },
    { "RemoveForce", &Engine::LuaObject_RemoveForce },
    { "RemoveTag", &Engine::LuaObject_RemoveTag },
    { "SetAnimation", &Engine::LuaObject_SetAnimation },
    { "SetForce", &Engine::LuaObject_SetForce },
    { "SetPos", &Engine::LuaObject_SetPos },
    { "SetScript", &Engine::LuaObject_SetScript },
    { "StopAnimation", &Engine::LuaObject_StopAnimation },
    { NULL, NULL }
  };

  static const luaL_Reg object_library_functions[] = {
    { "Get", &Engine::LuaObject_Get },
    { "GetByPoint", &Engine::LuaObject_GetByPoint },
    { "GetByRect", &Engine::LuaObject_GetByRect },
    { "GetRoot", &Engine::LuaObject_GetRoot },
    { "GetSelected", &Engine::LuaObject_GetSelected },
    { "New", &Engine::LuaObject_New },
    { "Select", &Engine::LuaObject_Select },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, this->luaState, "Object", "", object_functions, object_library_functions /*, Sprite_gc */ );

  //Debug
  static const luaL_Reg debug_library_functions[] = {
    { "Alert", &Engine::LuaDebug_Alert },
    { "Log", &Engine::LuaDebug_Log },
    { "Render", &Engine::LuaDebug_Render },
    { "RenderState", &Engine::LuaDebug_RenderState },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_DEBUG, this->luaState, "Debug", "", none_functions, debug_library_functions );

  //Core
  static const luaL_Reg core_library_functions[] = {
    { "GameExit", &Engine::LuaCore_GameExit },
    { "GetTime", &Engine::LuaCore_GetTime },
    { "LoadScript", &Engine::LuaCore_LoadScript },
    { "SetTimer", &Engine::LuaCore_SetTimer },
    { "StopTimer", &Engine::LuaCore_StopTimer },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CORE, this->luaState, "Core", "", none_functions, core_library_functions );

  //Keyboard
  static const luaL_Reg keyboard_library_functions[] = {
    { "Listen", &Engine::LuaKeyboard_Listen },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_KEYBOARD, this->luaState, "Keyboard", "", none_functions, keyboard_library_functions );

  //Mouse
  static const luaL_Reg mouse_library_functions[] = {
    { "GetPos", &Engine::LuaMouse_GetPos },
    { "ListenKey", &Engine::LuaMouse_ListenKey },
    { "ListenMove", &Engine::LuaMouse_ListenMove },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_MOUSE, this->luaState, "Mouse", "", none_functions, mouse_library_functions );

  //Camera
  static const luaL_Reg camera_functions[] = {
    //{ "Get", &Engine::LuaCamera_Get },
    //{ "Set", &Engine::LuaCamera_Set },
    { NULL, NULL }
  };

  static const luaL_Reg camera_library_functions[] = {
    { "Get", &Engine::LuaCamera_Get },
    { "GetPos", &Engine::LuaCamera_GetPos },
    { "Set", &Engine::LuaCamera_Set },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CAMERA, this->luaState, "Camera", "", camera_functions, camera_library_functions );
  
  //Config
  static const luaL_Reg config_library_functions[] = {
    { "GetGridSize", &Engine::LuaConfig_GetGridSize },
    { "GetWindowSize", &Engine::LuaConfig_GetWindowSize },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_CONFIG, this->luaState, "Config", "", none_functions, config_library_functions );
  
  //Scene
  static const luaL_Reg scene_library_functions[] = {
    { "Clear", &Engine::LuaScene_Clear },
    { "LoadWorld", &Engine::LuaScene_LoadWorld },
    { "SaveWorld", &Engine::LuaScene_SaveWorld },
    { "SetLightAmbient", &Engine::LuaScene_SetLightAmbient },
    { "SetPause", &Engine::LuaScene_SetPause },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE, this->luaState, "Scene", "", none_functions, scene_library_functions );
  
  //Tools
  static const luaL_Reg tools_library_functions[] = {
    { "GetClipboard", &Engine::LuaTools_GetClipboard },
    { "GetRandomSeed", &Engine::LuaTools_GetRandomSeed },
    { "GetTimeFormatted", &Engine::LuaTools_GetTimeFormatted },
    { "SetClipboard", &Engine::LuaTools_SetClipboard },
    { NULL, NULL }
  };
  Engine::LuaObject::InitLibrary( game, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_TOOLS, this->luaState, "Tools", "", none_functions, tools_library_functions );

  //__log.PrintInfo( Filelevel_DEBUG, "Lua::Init => initialized [x%X]", this->luaState );

  lua_atpanic( this->luaState, ErrorHandler );
  __LuaCurrentContext = this;

  this->log.Open( "logs/lua.txt", File_mode_WRITE );
  this->log.EnableAutoFlush( true );

  if( Lua::attrsListByPrioritet.empty() ) {
    Lua::attrsListByPrioritet.resize( __LUA_OBJECT_ATTRS_PRIORITET_COUNT );
  }

  __log.PrintInfo( Filelevel_DEBUG, "Lua::Init => lua stack initial value %d", lua_gettop( this->luaState ) );

  return true;
}//Init




/*
=============
  LoadLibs
=============
*/
void Lua::LoadLibs() {
  if( !this->luaState ) {
    return;
  }

  static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    //{LUA_LOADLIBNAME, luaopen_package},
    //{LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    //{LUA_IOLIBNAME, luaopen_io},
    //{LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_BITLIBNAME, luaopen_bit32},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_DBLIBNAME, luaopen_debug},
    {NULL, NULL}
  };

  //luaL_openlibs
  const luaL_Reg *lib;
  for( lib = loadedlibs; lib->func; lib++ ) {
    luaL_requiref( this->luaState, lib->name, lib->func, 1 );
    lua_pop( this->luaState, 1 );
  }
}//LoadLibs



/*
=============
  Unref
=============
*/
void Lua::Unref( const int referenceId ) {
  luaL_unref( this->luaState, LUA_REGISTRYINDEX, referenceId );
}//Unref



/*
=============
  GetBoolean
=============
*/
bool Lua::GetBoolean( const int stackIndex, const bool removeFromStack ) {
  bool result = false;
  if( lua_isboolean( this->luaState, stackIndex ) ) {
    result = lua_toboolean( this->luaState, stackIndex ) ? true : false;
    if( removeFromStack ) {
      lua_remove( this->luaState, stackIndex );
    }
  }
  return result;
}//GetBoolean



void Lua::DumpStack() {
  int count = lua_gettop( this->luaState );
  __log.PrintInfo( Filelevel_DEBUG, "Lua::DumpStack => size[%d]:", count );
  char t[ 1024 ];
  for( int q = 1; q <= count; ++q ) {
    int type = lua_type( this->luaState, q );
    t[ 0 ] = 0;
    switch( type ) {
    case LUA_TBOOLEAN:
      sprintf_s( t, sizeof( t ), "%s", lua_toboolean( this->luaState, q ) ? "true" : "false" );
      break;
    case LUA_TNUMBER:
      sprintf_s( t, sizeof( t ), "%3.3f", lua_tonumber( this->luaState, q ) );
      break;
    case LUA_TSTRING:
      sprintf_s( t, sizeof( t ), "'%s'", lua_tostring( this->luaState, q ) );
      break;
    }
    __log.PrintInfo( Filelevel_DEBUG, ". [%d/%d] %s = %s", q, count, lua_typename( this->luaState, type ), t );
  }
}//DumpStack



void Lua::Pop( const int count ) {
  lua_pop( this->luaState, count );
}//Pop



/*
=============
  Destroy
=============
*/
void Lua::Destroy()
{
  if( this->luaState )
  {
    __log.PrintInfo( Filelevel_DEBUG, "Lua::Destroy => stack = %d", lua_gettop( this->luaState ) );
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

  __log.PrintInfo( Filelevel_DEBUG, "Lua::RunFile => file '%s'...", fileName.c_str() );
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
  RunScript
=============
*/
bool Lua::RunScript( const std::string &script )
{
  if( !this->luaState ) {
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunScript( '%s' ) => lua-state not initialized", script.c_str() );
    return false;
  }

  if( luaL_dostring( this->luaState, script.c_str() ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Lua::RunScript( '%s' ) => luaL_dostring failed:\n%s", script.c_str(), lua_tostring( this->luaState, -1 ) );
    return false;
  }

  return true;
}//RunScript




/*
=============
  CallFunction
=============
*/
bool Lua::CallFunction( const std::string &funcName )
{
  __log.PrintInfo( Filelevel_DEBUG, "Lua::CallFunction => funcName['%s']", funcName.c_str() );
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
    lua_pop( this->luaState, 1 );
    return false;
  }

  return true;
}//CallFunction




/*
=============
  CallTableTableFunction
  Вызывает lua-функцию из таблицы, находящейся в таблице:
  table[ key ].function()
=============
*/
bool Lua::CallTableTableFunction( const std::string &table, const std::string &key, const std::string &function ) {
  __log.PrintInfo( Filelevel_DEBUG, "CallTableTableFunction => %s[%s].%s()", table.c_str(), key.c_str(), function.c_str() );
  LuaStateCheck state( this->luaState );

  lua_getglobal( this->luaState, table.c_str() );  //stack: table
  if( !lua_istable( this->luaState, -1 ) ) {
    __log.PrintInfo( Filelevel_ERROR, "CallTableTableFunction => '%s' is not a table", table.c_str() );
    lua_pop( this->luaState, 1 );
    return false;
  }
  lua_pushstring( this->luaState, key.c_str() );  //stack: table key
  lua_gettable( this->luaState, -2 ); //stack: table[key]
  if( !lua_istable( this->luaState, -1 ) ) {
    __log.PrintInfo( Filelevel_ERROR, "CallTableTableFunction => '%s[%s]' is not a table", table.c_str(), key.c_str() );
    lua_pop( this->luaState, 2 );
    return false;
  }
  lua_pushstring( this->luaState, function.c_str() ); //stack: table[key] function
  lua_gettable( this->luaState, -2 ); //stack: table[key][function]
  if( !lua_isfunction( this->luaState, -1 ) ) {
    __log.PrintInfo( Filelevel_ERROR, "CallTableTableFunction => %s[%s].'%s' is not a function", table.c_str(), key.c_str(), function.c_str() );
    lua_pop( this->luaState, 1 );
    return false;
  }
  lua_pcall( this->luaState, 0, 0, 0 );
  lua_pop( this->luaState, 2 ); //pop function and result

  return true;
}//CallTableTableFunction




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
  __log.PrintInfo( Filelevel_ERROR, "Lua::ErrorHandler => %s", lua_tostring( lua, -1 ) );
  return 0;
}//ErrorHandler




/*
=============
  GetStackParmsCount
=============
*/
int Lua::GetStackParmsCount() {
  return lua_gettop( this->luaState );
}//GetStackParmsCount



/*
=============
  GetColor
=============
*/
int Lua::GetColor( lua_State *lua, int stackIndex, FU_OUT Vec4& color ) {
  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < stackIndex ) {
    __log.PrintInfo( Filelevel_ERROR, "Lua::GetColor => not enough parameters" );
    return 0;
  }

  if( lua_istable( lua, stackIndex ) ) {  //{R,G,B,A}
    __log.PrintInfo( Filelevel_DEBUG, "Lua::GetColor => by table" );
    lua_pushvalue( lua, -1 );
    lua_pushnil( lua );
    float f[ 4 ];
    int num = 0;

    for( int num = 0; num < 4; ++num ) {
      lua_next( lua, -2 );
      lua_pushvalue( lua, -2 );
      f[ num ] = ( float ) lua_tonumber( lua, -2 );
      lua_pop( lua, 2 );
    }
    color.Set( f[ 0 ], f[ 1 ], f[ 2 ], f[ 3 ] );
    return 1;
  } else if( lua_isstring( lua, stackIndex ) ) {  //"RRGGBBAA" in hex
    //__log.PrintInfo( Filelevel_DEBUG, "Lua::GetColor => by string" );
    color = tools::StringToColor( lua_tostring( lua, stackIndex ) );
    return 1;
  } else if( lua_isnumber( lua, stackIndex ) ) {  //R,G,B,A
    if( parmsCount < stackIndex + 3 ) {
      __log.PrintInfo( Filelevel_ERROR, "Lua::GetColor => by 4 float => not enough parameters" );
      return 0;
    }
    __log.PrintInfo( Filelevel_DEBUG, "Lua::GetColor => by 4 float" );
    color.Set(
      ( float ) lua_tonumber( lua, stackIndex ),
      ( float ) lua_tonumber( lua, stackIndex + 1 ),
      ( float ) lua_tonumber( lua, stackIndex + 2 ),
      ( float ) lua_tonumber( lua, stackIndex + 3 )
    );
    return 4;
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Lua::GetColor => unknown type" );
    return 0;
  }

  __log.PrintInfo( Filelevel_ERROR, "Lua::GetColor => unknown error" );
  return 0;
}//GetColor
