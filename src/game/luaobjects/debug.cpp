#include "debug.h"
#include "game/game.h"
#include "core/debugrenderer.h"


using namespace Engine;
extern DebugRenderer* __debugRender;


/*
  Alert: обычный виндовый MessageBox с остановкой игрового процесса
    1 string: заголовок окна
    2 string: текст сообщения
*/
int Engine::LuaDebug_Alert( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 2 ) {
    luaL_error( lua, "Engine::LuaDebug_Alert => use: Debug.Alert( title, text )" );
  }

  ::MessageBox( NULL, lua_tostring( lua, 2 ), lua_tostring( lua, 1 ), MB_OK );
  sTimer.Update();
  sTimer.Update();

  return 0;
}//LuaDebug_Alert


/*
  Log: вывод в logs/lua.txt
    1 string: текст
    [ 2 bool: добавлять ли символ новой строки в конец ]
*/
int Engine::LuaDebug_Log( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 1 ) {
    luaL_error( lua, "Engine::LuaDebug_Log => use: Debug.Log( text [, addNewLine=true ] )" );
  }

  const char *text = lua_tostring( lua, 1 );
  bool addNewLine = true;
  if( argc > 1 && lua_isboolean( lua, 2 ) ) {
    addNewLine = ( lua_toboolean( lua, 2 ) ? true : false );
  }

  Lua::log.Print( "%s%s", text, addNewLine ? "\n" : "" );

  return 0;
}//LuaDebug_Log


int Engine::LuaDebug_RenderState( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 1 ) {
    luaL_error( lua, "Engine::LuaDebug_RenderState => use: Debug.RenderState( stateFlags )" );
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_DEBUG );

  int flags = lua_tointeger( lua, 1 );
  lib->game->core->debug.renderRenderable  = ( ( flags&1 ) == 1 );
  lib->game->core->debug.renderCollision   = ( ( flags&2 ) == 2 );
  lib->game->core->debug.renderTrigger     = ( ( flags&4 ) == 4 );

  return 0;
}//LuaDebug_RenderState


int Engine::LuaDebug_Render( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_DEBUG );

  int parmsCount = lua_gettop( lua ); //число параметров
  if( parmsCount < 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters" );
    return 0;
  }
  std::string operation = lua_tostring( lua, 1 );
  int result = 0;

  if( operation == "clrscr" ) {
    __debugRender->Clrscr();
  } else if( operation == "line" ) { //x0, y0, z0, x1, y1, z1, color
    if( parmsCount < 8 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      Vec4 color;
      Lua::GetColor( lib->lua, 8, color );
      __debugRender->Line(
          Vec3( ( Float ) lua_tonumber( lua, 2 ), ( Float ) lua_tonumber( lua, 3 ), ( Float ) lua_tonumber( lua, 4 ) ),
          Vec3( ( Float ) lua_tonumber( lua, 5 ), ( Float ) lua_tonumber( lua, 6 ), ( Float ) lua_tonumber( lua, 7 ) ),
          color
          );
    }
  } else if( operation == "sprite" ) { //x0, y0, z0, x1, y1, z1, texture, color
    if( parmsCount < 8 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      Vec4 color( Vec4One );
      Lua::GetColor( lua, 9, color );
      __debugRender->Sprite(
          Vec3( ( Float ) lua_tonumber( lua, 2 ), ( Float ) lua_tonumber( lua, 3 ), ( Float ) lua_tonumber( lua, 4 ) ),
          Vec3( ( Float ) lua_tonumber( lua, 5 ), ( Float ) lua_tonumber( lua, 6 ), ( Float ) lua_tonumber( lua, 7 ) ),
          lua_tostring( lua, 8 ),
          color
          );
    }
  } else if( operation == "rect" ) { //x0, y0, z0, x1, y1, z1, color
    if( parmsCount < 8 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      Vec4 color( Vec4One );
      Lua::GetColor( lua, 8, color );
      __debugRender->Rect(
          Vec3( ( Float ) lua_tonumber( lua, 2 ), ( Float ) lua_tonumber( lua, 3 ), ( Float ) lua_tonumber( lua, 4 ) ),
          Vec3( ( Float ) lua_tonumber( lua, 5 ), ( Float ) lua_tonumber( lua, 6 ), ( Float ) lua_tonumber( lua, 7 ) ),
          color
          );
    }
  } else if( operation == "scissorEnable" ) { //x0, y0, x1, y1
    if( parmsCount < 5 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      __debugRender->SciccorEnable(
          Vec2( ( Float ) lua_tonumber( lua, 2 ), ( Float ) lua_tonumber( lua, 3 ) ),
          Vec2( ( Float ) lua_tonumber( lua, 4 ), ( Float ) lua_tonumber( lua, 5 ) )
          );
    }
  } else if( operation == "scissorDisable" ) {
    __debugRender->SciccorDisable();
  } else if( operation == "text" ) { //x, y, z, text, color
    if( parmsCount < 6 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      Vec4 color( Vec4One );
      Lua::GetColor( lua, 6, color );
      __debugRender->Text(
          Vec3( ( Float ) lua_tonumber( lua, 2 ), ( Float ) lua_tonumber( lua, 3 ), ( Float ) lua_tonumber( lua, 4 ) ),
          color,
          lua_tostring( lua, 5 )
          );
    }
  } else if( operation == "getTextWidth" ) { //text
    if( parmsCount < 2 ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => not enough parameters for '%s'", operation.c_str() );
    } else {
      float width = __debugRender->GetTextWidh( lua_tostring( lua, 2 ), DEBUG_RENDERER_DEFAULT_FONT_PROPORTIONS );
      lua_pushnumber( lua, width );
      result = 1;
    }
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaDebug_Render => unknown shape '%s'", operation.c_str() );
  }

  return result;
}//LuaDebug_Render
