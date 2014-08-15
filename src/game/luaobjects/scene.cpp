#include "scene.h"
#include "game/game.h"


using namespace Engine;


int Engine::LuaScene_Clear( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE );

  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => core" );
  lib->game->core->ClearScene();
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => world" );
  lib->game->world->ClearWorld();
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaScene_Clear => done" );

  return 0;
}//LuaScene_Clear


int Engine::LuaScene_SetPause( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE );

  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaScene_SetPause => use: Scene.SetPause( setIsPaused )" );
    luaL_error( lua, "Engine::LuaScene_SetPause => use: Scene.SetPause( setIsPaused )" );
    return 0;
  }

  lib->game->SetActive( lua_toboolean( lua, 1 ) ? true : false );

  return 0;
}//LuaScene_SetPause


int Engine::LuaScene_SetLightAmbient( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE );

  int argc = lua_gettop( lua );

  if( argc != 4 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaScene_SetPause => use: Scene.LuaScene_SetLightAmbient( R, G, B, A )" );
    luaL_error( lua, "Engine::LuaScene_SetPause => use: Scene.LuaScene_SetLightAmbient( R, G, B, A )" );
    return 0;
  }

  const Vec4 color( ( float ) lua_tonumber( lua, 1 ), ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ), ( float ) lua_tonumber( lua, 4 ) );

  lib->game->core->GetLightRenderer()->GetLightManager()->lightAmbient = color;

  return 0;
}//LuaScene_SetLightAmbient


int Engine::LuaScene_LoadWorld( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE );

  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaScene_LoadWorld => use: Scene.LoadWorld( fileName )" );
    luaL_error( lua, "Engine::LuaScene_LoadWorld => use: Scene.LoadWorld( fileName )" );
    return 0;
  }

  lib->game->world->ClearWorld();
  lib->game->core->ClearScene();
  lib->game->ClearLuaTimers();
  lib->game->core->Update();
  //lib->game->core->SetCamera( lib->game->core->GetObject( "player" ) ); //!!!
  lib->game->core->SetCamera( lib->game->core->GetObject( "defaults/camera" ) );
  lib->game->world->LoadFromFile( lua_tostring( lua, 1 ) );
  //lib->game->core->GetObject( "player" )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->GetCollision()->SetVelocity( Vec3( 0.0f, 0.0f, 0.0f ) );  //!!!
  lib->game->core->GetObject( "defaults/camera" )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->GetCollision()->SetVelocity( Vec3( 0.0f, 0.0f, 0.0f ) );

  sTimer.Update();
  lib->game->core->Update();
  lib->game->core->keyboard.ResetState();
  lib->game->core->mouse.ResetState();

  return 0;
}//LuaScene_LoadWorld


int Engine::LuaScene_SaveWorld( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SCENE );

  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaScene_SaveWorld => use: Scene.SaveWorld( fileName )" );
    luaL_error( lua, "Engine::LuaScene_SaveWorld => use: Scene.SaveWorld( fileName )" );
    return 0;
  }

  lib->game->world->SaveToFile( lua_tostring( lua, 1 ) );
  sTimer.Update();
  lib->game->core->Update();
  lib->game->core->keyboard.ResetState();
  lib->game->core->mouse.ResetState();

  return 0;
}//LuaScene_SaveWorld
