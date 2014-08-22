#include "object.h"
#include "game/game.h"
#include "core/object.h"
#include "core/file.h"
#include "core/animationpack.h"
#include "core/animationpack.h"
#include "game/luastatecheck.h"
#include "core/textparser.h"

extern "C" {
#include "../../lua/lua.h"
#include "../../lua/lualib.h"
#include "../../lua/lauxlib.h"

#include "../../luabins/luabins.h"
#include "../../luabins/saveload.h"
}



using namespace Engine;


Engine::LuaObject_Callback *LuaObject_OnSetScript = NULL;


void Engine::LuaObject_callback_SetOnSetScript( LuaObject_Callback *callback ) {
  LuaObject_OnSetScript = callback;
}//LuaObject_callback_SetOnSetScript


int Engine::LuaObject_New( lua_State *lua ) {
  int argc = lua_gettop( lua );
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_New => argc[%d]", argc );

  if( argc < 1 ) {
    luaL_error( lua, "Engine::LuaObject_New => object name required" );
  }

  const char* nameStr = luaL_checkstring( lua, 1 );
  const char* parentName = NULL;
  bool isSaveable = true;

  if( argc > 1 ) {
    parentName = luaL_checkstring( lua, 2 );
    if( !parentName || !parentName[ 0 ] ) {
      parentName = NULL;
    }

    if( argc > 2 ) {
      isSaveable = lua_toboolean( lua, 3 ) ? true : false;
    }
  }

  //Object *object = new Object( name, parentName, isSaveable, setUserData );
  //object->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  std::string name = nameStr;
  long slashPos = name.find_last_of( "/" );
  Object *parentObj = NULL;
  Object *obj       = NULL;
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  bool notInGrid = false;

  if( slashPos > 0 ) {
    std::string fullPath = &name[ slashPos ];
    std::vector< std::string > path = tools::Explode( name, "/" );
    std::vector< std::string >::iterator iter, iterEnd = path.end();
    std::string currentPath = "";
    size_t
      pathLength = path.size(),
      num = 0;
    for( iter = path.begin(); iter != iterEnd; ++iter, ++num ) {
      //__log.PrintInfo( Filelevel_DEBUG, ". iter: '%s'", iter->c_str() );
      obj = lib->game->core->CreateObject( *iter, ( currentPath.length() ? lib->game->core->GetObject( currentPath ) : NULL ), num == pathLength - 1 );
      obj->SetSaveable( isSaveable );
      obj->SetPosition( Vec3Null );
      currentPath += ( currentPath.length() ? "/" : "" ) + *iter;
      if( !notInGrid && isSaveable ) {
        lib->game->world->AttachObjectToGrid( lib->game->world->GetGridPositionByObject( *obj ), obj );
        __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_New => object '%s' x%p attached to grid", obj->GetNameFull().c_str(), obj );
      }
    }
    //std::string nextLevel = &name[ slashPos ];
    //std::string currentLevel = name.substr( 0, slashPos );
    //parentObj = game->core->GetObject( currentLevel );
  } else {
    obj = lib->game->core->CreateObject( name, NULL, true );
    obj->SetSaveable( isSaveable );
    obj->SetPosition( Vec3Null );
    obj->Update( 0.0f );
    if( !notInGrid && isSaveable ) {
      World::Grid::Position gridPos = lib->game->world->GetGridPositionByObject( *obj );
      lib->game->world->AttachObjectToGrid( gridPos, obj );
      __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_New => object '%s' %p attached to grid", obj->GetNameFull().c_str(), obj );
    }
  }

  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, obj->GetLuaObjectId() );
  lua_getfield( lib->lua, -1, "fn" );
  lua_remove( lib->lua, -2 );

  return 1;
}//LuaObject_New


int Engine::LuaObject_Get( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    printf( "Engine::LuaObject_Get => Not enough parameters\n", argc );
    return 0;
  }

  const char* name = luaL_checkstring( lua, 1 );
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  Object *object = lib->game->core->GetObject( name );
  if( object ) {
    if( !object->GetLuaObjectId() ) {
      object->InitLuaUserData();
    }
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    //__log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_Get => id %d", object->GetLuaObjectId() );
    if( !lua_istable( lua, -1 ) ) {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Get => %d is not a table", object->GetLuaObjectId() );
    } else {
      lua_getfield( lua, -1, "fn" );
      lua_remove( lua, -2 );
    }

    //TODO: если объект не найден в таблице __gameobjects, то его нужно проинициализировать по аналогии с LuaObject.new
    //хотя, значит объект не скриптован, может и не надо, выводить ошибку
    //lua_getglobal( lua, "__sprites" );
    //lua_getfield( lua, -1, object->spriteName.c_str() );
    //lua_remove( lua, -2 );
  } else {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_Get => object '%s' not found", name );
    lua_pushnil( lua );
  }

  if( lua_gettop( lua ) != argc + 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Get => stack crashed" );
  }

  return 1;
}//LuaObject_Get


int Engine::LuaObject_GetByPoint( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_GetByPoint => use: Object.GetByPoint( type, x, y [,afterObjectName] )" );
    luaL_error( lua, "Engine::LuaObject_GetByPoint => use: Object.GetByPoint( type, x, y [,afterObjectName] )" );
    return 0;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  int type = lua_tointeger( lua, 1 );
  Vec2 point( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) );
  std::string afterObjectName;
  if( argc >= 4 ) {
    afterObjectName = lua_tostring( lua, 4 );
  }
  //objectName = LUAFUNC_GetObjectByPoint( type, point, objectName );
  //lua_pushstring( lua, objectName.c_str() );

  Object *object = NULL;

  switch( type ) {
    case 1: { //renderable
      object = lib->game->core->GetRenderableInPoint( point, afterObjectName );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
        __log.PrintInfo( Filelevel_DEBUG, ". found['%s']", object->GetNameFull().c_str() );
      } else {
        object = lib->game->core->GetRenderableInPoint( point, afterObjectName );
        while( object && !object->IsSaveable() ) {
          object = object->GetParent();
        }
        if( object ) {
          __log.PrintInfo( Filelevel_DEBUG, ". found['%s']", object->GetNameFull().c_str() );
        } else {
          __log.PrintInfo( Filelevel_DEBUG, ". not found" );
        }
      }
      break;
    }//renderable
    case 2: { //collision
      object = lib->game->core->GetCollisionInPoint( point, afterObjectName );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
      } else {
        if( !afterObjectName.empty() ) {
          object = lib->game->core->GetCollisionInPoint( point, "" );
          while( object && !object->IsSaveable() ) {
            object = object->GetParent();
          }
        }
      }
      break;
    }//collision
    case 3: { //trigger
      object = lib->game->core->GetTriggerInPoint( point, afterObjectName );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
      } else {
        if( !afterObjectName.empty() ) {
          object = lib->game->core->GetTriggerInPoint( point, "" );
          while( object && !object->IsSaveable() ) {
            object = object->GetParent();
          }
        }
      }
      break;
    }//trigger
  }//

  if( object ) {
    if( !object->GetLuaObjectId() ) {
      object->SetLuaObjectId( Engine::LuaObject::InitUserData( object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, false ) );
    }
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    lua_getfield( lua, -1, "fn" );
    lua_remove( lua, -2 );
    //__log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_GetByPoint => id %d", object->GetLuaObjectId() );
  } else {
    //__log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetByPoint => object not found" );
    lua_pushnil( lua );
  }

  if( lua_gettop( lua ) != argc + 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_GetByPoint => stack crashed" );
  }

  return 1;
}//LuaObject_GetByPoint


int Engine::LuaObject_GetByRect( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 5 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_GetByRect => use: Object.GetByRect( type, left, top, right, bottom )" );
    luaL_error( lua, "Engine::LuaObject_GetByRect => use: Object.GetByRect( type, left, top, right, bottom )" );
    return 0;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  int type = lua_tointeger( lua, 1 );
  Vec2
    leftTop( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) ),
    rightBottom( ( float ) lua_tonumber( lua, 4 ), ( float ) lua_tonumber( lua, 5 ) );
  std::string objectName;
  //objectName = LUAFUNC_GetObjectByRect( type, leftTop, rightBottom );
  //lua_pushstring( lua, objectName.c_str() );

  ObjectList objectList;
  lib->game->core->GetObjectsInRect( type, leftTop, rightBottom, objectList );

  lua_newtable( lua );
  int num = 1;
  char name[ 256 ];
  for( auto &object: objectList ) {
    if( !object->GetLuaObjectId() ) {
      object->SetLuaObjectId( Engine::LuaObject::InitUserData( object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, false ) );
    }
    sprintf_s( name, sizeof( name ), "object%d", object->GetLuaObjectId() );
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    lua_getfield( lua, -1, "fn" );
    lua_remove( lua, -2 );
    lua_setfield( lua, -2, name );
    ++num;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_GetByRect => %d objects", objectList.size() );

  if( lua_gettop( lua ) != argc + 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_GetByRect => stack crashed" );
  }

  return 1;
}//LuaObject_GetByRect


int Engine::LuaObject_Destroy( lua_State *lua ) {
  int argc = lua_gettop( lua );
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_Destroy => object is NULL" );
    luaL_error( lua, "error" );
  }
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  lib->game->OnRemoveObject( object->GetNameFull() );
  lib->game->core->RemoveObject( object->GetNameFull(), true );
  //delete object;
  Engine::LuaObject::Destroy( object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  if( lua_gettop( lua ) != argc ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_GetByRect => stack crashed" );
  }

  return 0;
}//Object_Destroy


int Engine::LuaObject_SetAnimation( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_SetAnimation => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_SetAnimation => object is NULL" );
  }

  int argc = lua_gettop( lua );

  const char *str;
  if( argc < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetAnimation => use: object.api:SetAnimation( animationTemplateName, animationName, animationAfterCompleteName )", argc );
    return 0;
  }
  str = lua_tostring( lua, 2 );
  std::string
    templateName = str,
    animation,
    animationAfterAnimationComplete;
  int luaCallbackId = 0;

  if( argc > 2 ) {
    str = lua_tostring( lua, 3 );
    animation = str;

    if( argc > 3 ) {
      if( lua_isfunction( lua, 4 ) ) {
        __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetAnimation => callback is function" );
        lua_pushvalue( lua, 4 );
        luaCallbackId = luaL_ref( lua, LUA_REGISTRYINDEX );
        __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetAnimation => function[%d]", luaCallbackId );
      } else {
        str = lua_tostring( lua, 4 );
        animationAfterAnimationComplete = str;
      }
    } else {
      animationAfterAnimationComplete = animation;
    }
  }

  Animation::AnimationSetAction actionAfterAnimation( Animation::ANIMATION_SET_ACTION_REPEAT, animationAfterAnimationComplete );
  if( luaCallbackId ) {
    actionAfterAnimation.callback = Engine::LuaObject_AnimationCallback;
    actionAfterAnimation.luaFunctionId = luaCallbackId;
    actionAfterAnimation.userData = object;
  }
  /*
  if( !actionAfterAnimationComplete.empty() ) {
    auto &searchResult = game->animationSetActionByName.find( actionAfterAnimationComplete );
    if( searchResult != game->animationSetActionByName.end() ) {
      actionAfterAnimation.action = searchResult->second;
    } else {
      __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectSetAnimation => unknown action['%s']", actionAfterAnimationComplete.c_str() );
    }
  }
  */
  //__log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetAnimation => luaFunctionId[%d]", actionAfterAnimation.luaFunctionId );

  Animation::AnimationPack *pack = object->ApplyAnimation( actionAfterAnimation, templateName, animation );
  if( pack ) {
    pack->SetEnabled( true );
  }

  return 0;
}//Object_SetAnimation


void Engine::LuaObject_AnimationCallback( Animation::AnimationSetAction *action ) {
  if( !action->luaFunctionId ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_AnimationCallback => luaFunctionId is NULL" );
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_AnimationCallback => do, functionId[%d]", action->luaFunctionId );
  Object *object = reinterpret_cast< Object* >( action->userData );
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_AnimationCallback => object['%s']", object->GetNameFull().c_str() );
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  if( object->GetLuaObjectId() ) {
    object->InitLuaUserData();
  }

  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, action->luaFunctionId );
  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
  lua_getfield( lib->lua, -1, "fn" );
  lua_remove( lib->lua, -2 );
  if( lua_pcall( lib->lua, 1, 0, 0 ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_AnimationCallback => failed to call function %d in object '%s': %s", action->luaFunctionId, object->GetNameFull().c_str(), lua_tostring( lib->lua, -1 ) );
    lua_pop( lib->lua, 1 );
  }
  luaL_unref( lib->lua, LUA_REGISTRYINDEX, action->luaFunctionId );
  action->luaFunctionId = 0;
  action->callback = NULL;
}//LuaObject_AnimationCallback


int Engine::LuaObject_GetName( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetName => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetName => object is NULL" );
  }

  lua_pushstring( lua, object->GetName().c_str() );

  return 1;
}//LuaObject_GetName


int Engine::LuaObject_GetNameFull( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetNameFull => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetNameFull => object is NULL" );
  }

  lua_pushstring( lua, object->GetNameFull().c_str() );

  return 1;
}//LuaObject_GetNameFull


int Engine::LuaObject_GetParentNameFull( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetParentNameFull => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetParentNameFull => object is NULL" );
  }

  lua_pushstring( lua, object->GetParentNameFull().c_str() );

  return 1;
}//LuaObject_GetParentNameFull


int Engine::LuaObject_GetPos( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetPos => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetPos => object is NULL" );
  }

  const Vec3 &pos = object->GetPosition();
  lua_pushnumber( lua, pos.x );
  lua_pushnumber( lua, pos.y );

  return 2;
}//LuaObject_GetPos


int Engine::LuaObject_SetPos( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_SetPos => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_SetPos => object is NULL" );
  }

  int argc = lua_gettop( lua );
  if( argc < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetPos => use: object.api:SetPos( x, y )" );
    luaL_error( lua, "Engine::LuaObject_SetPos => use: object.api:SetPos( x, y )" );
    return 0;
  }

  const Vec2 pos( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) );

  object->SetPosition2D( pos );
  if( argc >= 4 ) {
    object->SetZ( ( float ) lua_tonumber( lua, 4 ) );
  }

  object->Update( 0.0f );

  return 0;
}//LuaObject_SetPos



int Engine::LuaObject_AddTrigger( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_AddTrigger => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_AddTrigger => object is NULL" );
  }

  ObjectTrigger *trigger = object->GetTrigger();
  if( !trigger ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_AddTrigger => object '%s' don't have trigger", object->GetNameFull().c_str() );
    return 0;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONTRIGGER ) ) {
    if( object->IsTrigger() ) {
      lib->game->luaObjectOnTrigger.push_back( object->GetLuaObjectId() );
      object->GetTrigger()->AddHandler( Game::LuaObject_OnTrigger );
    } else {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_AddTrigger => function '%s' exists, but object '%s' is not trigger", GAME_OBJECT_HANDLER_ONTRIGGER.c_str(), object->GetNameFull().c_str() );
    }
  }

  return 0;
}//LuaObject_AddTrigger


int Engine::LuaObject_Select( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc != 1 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Select => use: Object.Select( objectsList )" );
    luaL_error( lua, "Engine::LuaObject_Select => use: Object.Select( objectsList )" );
    return 0;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  lib->game->core->debug.selectedObjects.clear();

  if( !lua_istable( lua, -1 ) ) {
    return 0;
  }

  lua_pushnil( lua );
  while( lua_next( lua, -2 ) ) {
    if( lua_istable( lua, -1 ) ) {
      //lua_getfield( lua, -1, "fn" );
      lua_getfield( lua, -1, "api" );
      luaL_checktype( lua, -1, LUA_TUSERDATA );
      Object *object = *static_cast<Object **>( luaL_checkudata( lua, -1, "Object" ) );
      lib->game->core->debug.selectedObjects.push_back( object );
      lua_pop( lua, 1 );
    } else {
      __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Select => not a table" );
    }
    lua_pop( lua, 1 );
  }
  lua_pop( lua, 1 );

  return 0;
}//LuaObject_Select


int Engine::LuaObject_GetSelected( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  lua_newtable( lua );
  char name[ 256 ];
  for( auto &object: lib->game->core->debug.selectedObjects ) {
    if( !object->GetLuaObjectId() ) {
      object->SetLuaObjectId( Engine::LuaObject::InitUserData( object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, false ) );
    }
    sprintf_s( name, sizeof( name ), "object%d", object->GetLuaObjectId() );
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
    lua_getfield( lua, -1, "fn" );
    lua_remove( lua, -2 );
    lua_setfield( lua, -2, name );
  }

  return 1;
}//LuaObject_GetSelected


int Engine::LuaObject_GetId( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetName => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetName => object is NULL" );
  }

  lua_pushinteger( lua, object->GetLuaObjectId() );

  return 1;
}//LuaObject_GetId


int Engine::LuaObject_SetScript( lua_State *lua ) {
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetScript ..." );
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_SetScript => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_SetScript => object is NULL" );
  }

  int argc = lua_gettop( lua );

  if( argc != 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetScript => use: object.api:SetScript( scriptFileName )" );
    luaL_error( lua, "Engine::LuaObject_SetScript => use: object.api:SetScript( scriptFileName )" );
    return 0;
  }

  const char *templateFileName = lua_tostring( lua, 2 );
  memory scriptData;
  if( __fileManager->GetFile( templateFileName, scriptData, true ) ) {
    Engine::LuaObject::BindTemplateToObject( object->GetLuaObjectId(), object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, scriptData.getData() );
    object->SetLuaScript( templateFileName );
    /*
    if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, "OnCreate" ) ) {
      Engine::LuaObject::CallFunction( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, "OnCreate" );
      __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetScript => call OnCreate on object '%s'", object->GetNameFull().c_str() );
    }
    */
    if( LuaObject_OnSetScript ) {
      LuaObject_OnSetScript( object );
    }
  } else {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetScript => file '%s' not found", templateFileName );
  }

  lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
  lua_getfield( lua, -1, "fn" );
  lua_remove( lua, -2 );

  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetScript done" );
  return 1;
}//LuaObject_SetScript


int Engine::LuaObject_AddTag( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_AddTag => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_AddTag => object is NULL" );
  }

  int argc = lua_gettop( lua );

  const char *tag;
  if( argc < 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_AddTag => use: object.api:AddTag( tagName )" );
    return 0;
  }
  tag = lua_tostring( lua, 2 );

  object->AddTag( tag );

  return 0;
}//LuaObject_AddTag


int Engine::LuaObject_RemoveTag( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_RemoveTag => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_RemoveTag => object is NULL" );
  }

  int argc = lua_gettop( lua );

  const char *tag;
  if( argc < 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_RemoveTag => use: object.api:RemoveTag( tagName )" );
    return 0;
  }
  tag = lua_tostring( lua, 2 );

  object->RemoveTag( tag );

  return 0;
}//LuaObject_RemoveTag


int Engine::LuaObject_HasTag( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_HasTag => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_HasTag => object is NULL" );
  }

  int argc = lua_gettop( lua );

  if( argc != 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_HasTag => use: object.api:HasTag( tagName )" );
    return 0;
  }

  bool hasTag = object->IsHasTag( lua_tostring( lua, 2 ) );
  lua_pushboolean( lua, hasTag );

  return 1;
}//LuaObject_HasTag


int Engine::LuaObject_GetParent( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetParent => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetParent => object is NULL" );
  }

  Object *parent = object->GetParent();
  if( !parent->GetParent() ) {
    lua_pushnil( lua );
    return 1;
  }

  if( !parent->GetLuaObjectId() ) {
    parent->InitLuaUserData();
  }
  lua_rawgeti( lua, LUA_REGISTRYINDEX, parent->GetLuaObjectId() );
  lua_getfield( lua, -1, "fn" );
  lua_remove( lua, -2 );

  return 1;
}//LuaObject_GetParent


int Engine::LuaObject_Attr( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_Attr => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_Attr => object is NULL" );
  }

  int argc = lua_gettop( lua );

  if( argc < 2 || !lua_istable( lua, 2 ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Attr => use: object.api:Attr( parametersTableList )" );
    return 0;
  }

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  VariableAttributesList setAttrs, getAttrs;
  VariableAttribute *attr = NULL;
  int returnValuesCount = 0;
  //__log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_Attr => is table[%d]", lua_istable( lua, -1 ) );
  lua_pushnil( lua );
  while( lua_next( lua, -2 ) ) {
    if( lua_isnumber( lua, -2 ) ) { //get: ключ = число == без ключа, возвращаем значения
      const std::string parameterName = luaL_checkstring( lua, -1 );
      attr = new VariableAttribute();
      attr->name = parameterName;
      getAttrs.push_back( attr );
    } else { //set
      const std::string parameterName = luaL_checkstring( lua, -2 );
      attr = NULL;
      if( lua_isboolean( lua, -1 ) ) {
        attr = new VariableAttribute();
        attr->value.SetBoolean( lua_toboolean( lua, -1 ) ? true : false );
      } else if( lua_isnil( lua, -1 ) || lua_isnone( lua, -1 ) ) {
        attr = new VariableAttribute();
        attr->value.SetNull();
      } else if( lua_isnumber( lua, -1 ) ) {
        attr = new VariableAttribute();
        attr->value.SetNumber( ( float ) lua_tonumber( lua, -1 ) );
      } else if( lua_isstring( lua, -1 ) ) {
        attr = new VariableAttribute();
        attr->value.SetString( lua_tostring( lua, -1 ) );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_Attr => parameter's  key '%s' has bad value", parameterName.c_str() );
      }
      if( attr ) {
        attr->name = parameterName;;
        if( attr->name == "collision" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_COLLISION ].push_back( attr );
        } else if( attr->name == "collisionSize" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_COLLISION_SIZE ].push_back( attr );
        } else if( attr->name == "lightBlockByCollision" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_LIGHTBLOCKBYCOLLISION ].push_back( attr );
        } else if( attr->name == "renderable" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_RENDERABLE ].push_back( attr );
        } else if( attr->name == "trigger" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_TRIGGER ].push_back( attr );
        } else if( attr->name == "lightPoint" ) {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_LIGHTPOINT ].push_back( attr );
        }
        else {
          Lua::attrsListByPrioritet[ LUA_OBJECT_ATTRS_PRIORITET_DEFAULT ].push_back( attr );
        }
        //__log.PrintInfo( Filelevel_DEBUG, ". '%s' => '%s'", attr->name.c_str(), attr->value.GetString().c_str() );
      }
    }
    lua_pop( lua, 1 );
  }//while next
  lua_pop( lua, 1 );

  for( auto &attrsList: Lua::attrsListByPrioritet ) {
    for( auto &attr: attrsList ) {
      setAttrs.push_back( attr );
    }
    attrsList.clear();
  }

  RenderableQuad  *renderable;
  Collision       *collision;
  ObjectTrigger   *trigger;

  // set
  for( auto &attr: setAttrs ) {
    if( attr->name == "renderable" ) {
      if( attr->value.GetBoolean() ) {
        object->EnableRenderable( RENDERABLE_TYPE_QUAD );
      } else {
        object->DisableRenderable();
      }
      continue;
    }
    if( attr->name == "collision" ) {
      if( attr->value.GetBoolean() ) {
        object->EnableCollision();
      } else {
        object->DisableCollision();
      }
      continue;
    }//collision
    if( attr->name == "trigger" ) {
      if( attr->value.GetBoolean() ) {
        ObjectTrigger *trigger = object->EnableTrigger();
        if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONTRIGGER ) ) {
          lib->game->luaObjectOnTrigger.push_back( object->GetLuaObjectId() );
          trigger->AddHandler( Game::LuaObject_OnTrigger );
        }
      } else {
        lib->game->EraseLuaHandler( object->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONTRIGGER, lib->game->luaObjectOnTrigger );
        object->DisableTrigger();
      }
      continue;
    }//trigger
    if( attr->name == "lightBlockByCollision" ) {
      if( attr->value.GetBoolean() ) {
        object->EnableLightBlockByCollision();
      } else {
        object->DisableLightBlockByCollision();
      }
      continue;
    }//lightBlockByCollision
    if( attr->name == "lightPoint" ) {
      if( attr->value.GetBoolean() ) {
        object->EnableLightPoint();
      } else {
        object->DisableLightPoint();
      }
      continue;
    }//lightPoint
    if( attr->name == "position" ) {
      Vec3 pos = object->GetPosition();
      sscanf_s( attr->value.GetString().c_str(), "%f %f", &pos.x, &pos.y );
      object->SetPosition( pos );
      continue;
    }//position

    //renderable
    if( attr->name == "textureName" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      std::string textureFileName = std::string( FileManager_DATADIRECTORY ) + "/" + attr->value.GetString();
      if( renderable && __fileManager->FileExists( textureFileName ) ) {
        renderable->SetTexture( textureFileName );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => texture '%s' not found or object not renderable", textureFileName.c_str() );
      }
      continue;
    }//textureName
    if( attr->name == "renderableSize" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec2 size( 1.0f, 1.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        renderable->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableSize: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableSize
    if( attr->name == "renderablePosition" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec3 pos( 0, 0, 0 );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &pos.x, &pos.y );
        if( res < 2 ) {
          pos.y = pos.x;
        }
        renderable->SetPosition( pos );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderablePosition: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderablePosition
    if( attr->name == "color" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f %f %f", &color.x, &color.y, &color.z, &color.w );
        if( res < 4 ) {
          color.y = color.z = color.w = color.x;
        }
        renderable->SetColor( color );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => color: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//color
    if( attr->name == "renderableScale" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec2 scale( 1.0f, 1.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &scale.x, &scale.y );
        if( res < 2 ) {
          scale.y = scale.x;
        }
        renderable->SetScale( scale );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableScale: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableScale
    if( attr->name == "renderableRotation" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        float angle = attr->value.GetNumber();
        renderable->SetRotation( DEG2RAD( angle ) );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableRotation: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableRotation

    //collision
    if( attr->name == "collisionSize" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 size( 1.0f, 1.0f, 0.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        collision->InitSquare( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionSize: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionSize
    if( attr->name == "collisionAcceleration" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec( 0.0f, 0.0f, 0.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetAcceleration( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionAcceleration: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionAcceleration
    if( attr->name == "collisionVelocity" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec( 0.0f, 0.0f, 0.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetVelocity( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionVelocity: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionVelocity
    if( attr->name == "collisionStatic" ) {
      collision = object->GetCollision();
      if( collision ) {
        collision->SetIsStatic( attr->value.GetBoolean( true ) );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionStatic: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionStatic
    if( attr->name == "collisionForce" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec = collision->GetForce();
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetForce( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionForce: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionForce

    //trigger
    if( attr->name == "triggerSize" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 size( 1.0f, 1.0f, 0.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        trigger->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => triggerSize: object '%s' not triger", object->GetNameFull().c_str() );
      }
      continue;
    }//triggerSize
    if( attr->name == "triggerPolygon" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementPolygon::PointList points;
        TextParser parser( attr->value.GetString().c_str(), attr->value.GetString().size() );
        TextParser::Result parserValue;
        int step = 0;
        Vec2 point;
        while( parser.GetNext( parserValue ) ) {
          if( parserValue.type != TPL_NUMBER ) {
            __log.PrintInfo( Filelevel_WARNING, "triggerPolygon => bad format of string '%s'", attr->value.GetString().c_str() );
            break;
          } else {
            if( step == 0 ) {
              point.x = parserValue.GetFloat();
            } else {
              point.y = parserValue.GetFloat();
              //__log.PrintInfo( Filelevel_DEBUG, "triggerPolygon => point[%3.3f; %3.3f]", point.x, point.y );
              points.push_back( point );
            }
          }
          step ^= 1;
        }
        trigger->SetPolygon( points );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => triggerPolygon: object '%s' not trigger", object->GetNameFull().c_str() );
      }
      continue;
    }//triggerPolygon
    if( attr->name == "triggerOffset" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 offset( 0.0f, 0.0f, 0.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &offset.x, &offset.y );
        trigger->SetOffset( offset );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => triggerOffset: object '%s' not triger", object->GetNameFull().c_str() );
      }
      continue;
    }//triggerSize
    if( attr->name == "lightPointSize" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        Vec2 size( 1.0f, 1.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f", &size.x, &size.y );
        __log.PrintInfo( Filelevel_DEBUG, "lightPointSize => [%3.3f; %3.3f]", size.x, size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        widget->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => lightPointSize: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointSize
    if( attr->name == "lightPointColor" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        Vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
        int res = sscanf_s( attr->value.GetString().c_str(), "%f %f %f %f", &color.x, &color.y, &color.z, &color.w );
        if( res < 4 ) {
          color.y = color.z = color.w = color.x;
        }
        __log.PrintInfo( Filelevel_DEBUG, "lightPointColor[%3.3f; %3.3f; %3.3f; %3.3f]", color.x, color.y, color.z, color.w );
        widget->SetColor( color );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => color: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointColor
    if( attr->name == "lightPointPenetration" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        float penetration;
        int res = sscanf_s( attr->value.GetString().c_str(), "%f", &penetration );
        __log.PrintInfo( Filelevel_DEBUG, "lightPointPenetration => %3.3f", penetration );
        widget->SetPenetration( penetration );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => lightPointPenetration: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointPenetration
    if( attr->name == "z" ) {
      object->SetZ( attr->value.GetNumber() );
      continue;
    }//z
    if( attr->name == "lockToDelete" ) {
      object->SetLockToDelete( attr->value.GetBoolean() );
      continue;
    }//z
  }//set

  //get
  for( auto &attr: getAttrs ) {
    if( attr->name == "renderable" ) {
      //attr->value.SetBoolean( object->IsRenderable() );
      lua_pushboolean( lua, object->IsRenderable() );
      ++returnValuesCount;
      continue;
    }//renderable
    if( attr->name == "collision" ) {
      //attr->value.SetBoolean( object->IsCollision() );
      lua_pushboolean( lua, object->IsCollision() );
      ++returnValuesCount;
      continue;
    }//collision
    if( attr->name == "trigger" ) {
      //attr->value.SetBoolean( object->IsTrigger() );
      lua_pushboolean( lua, object->IsTrigger() );
      ++returnValuesCount;
      continue;
    }//trigger
    if( attr->name == "lightBlockByCollision" ) {
      //attr->value.SetBoolean( object->IsLightBlockByCollision() );
      lua_pushboolean( lua, object->IsLightBlockByCollision() );
      ++returnValuesCount;
      continue;
    }//lightBlockByCollision
    if( attr->name == "lightPoint" ) {
      //attr->value.SetBoolean( object->IsLightPoint() );
      lua_pushboolean( lua, object->IsLightPoint() );
      ++returnValuesCount;
      continue;
    }//lightPoint
    if( attr->name == "position" ) {
      //attr->value.SetVec3( object->GetPosition() );
      const auto &pos = object->GetPosition();
      lua_pushnumber( lua, pos.x );
      lua_pushnumber( lua, pos.y );
      lua_pushnumber( lua, pos.z );
      returnValuesCount += 3;
      continue;
    }//position

    //renderable
    if( attr->name == "textureName" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const std::string &texture = renderable->GetTextureFileName();
        //attr->value.SetString( texture.empty() ? texture : &renderable->GetTextureFileName()[ strlen( FileManager_DATADIRECTORY ) + 1 ] );
        lua_pushstring( lua, ( texture.empty() ? texture : &renderable->GetTextureFileName()[ strlen( FileManager_DATADIRECTORY ) + 1 ] ).c_str() );
        ++returnValuesCount;
      } else {
        //attr->value.SetString( "" );
        lua_pushstring( lua, "" );
        ++returnValuesCount;
      }
      continue;
    }//textureName
    if( attr->name == "renderableSize" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        //attr->value.SetVec2( renderable->GetSize() );
        const auto &size = renderable->GetSize();
        lua_pushnumber( lua, size.x );
        lua_pushnumber( lua, size.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//renderableSize
    if( attr->name == "renderablePosition" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const Vec3 &pos = renderable->GetPosition();
        //attr->value.SetVec2( Vec2( pos.x, pos.y ) );
        lua_pushnumber( lua, pos.x );
        lua_pushnumber( lua, pos.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//renderablePosition
    if( attr->name == "color" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        //attr->value.SetVec4( renderable->GetColor() );
        const auto &color = renderable->GetColor();
        lua_pushnumber( lua, color.x );
        lua_pushnumber( lua, color.y );
        lua_pushnumber( lua, color.z );
        lua_pushnumber( lua, color.w );
        returnValuesCount += 4;
      } else {
        //attr->value.SetVec4( Vec4Null );
        lua_pushnumber( lua, 0.0f );
        lua_pushnumber( lua, 0.0f );
        lua_pushnumber( lua, 0.0f );
        lua_pushnumber( lua, 0.0f );
        returnValuesCount += 4;
      }
      continue;
    }//color
    if( attr->name == "renderableScale" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        //attr->value.SetVec2( renderable->GetScale() );
        const auto &scale = renderable->GetScale();
        lua_pushnumber( lua, scale.x );
        lua_pushnumber( lua, scale.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//renderableScale
    if( attr->name == "renderableRotation" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        //attr->value.SetNumber( renderable->GetRotation() );
        lua_pushnumber( lua, renderable->GetRotation() );
        ++returnValuesCount;
      } else {
        //attr->value.SetNumber( 0.0f );
        lua_pushnumber( lua, 0.0 );
        ++returnValuesCount;
      }
      continue;
    }//renderableRotation

    //collision
    if( attr->name == "collisionSize" ) {
      collision = object->GetCollision();
      if( collision ) {
        const Vec3 &size = collision->GetSize();
        //attr->value.SetVec2( Vec2( size.x, size.y ) );
        lua_pushnumber( lua, size.x );
        lua_pushnumber( lua, size.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//collisionSize
    if( attr->name == "collisionAcceleration" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 acceleration = collision->GetAcceleration();
        //attr->value.SetVec2( Vec2( acceleration.x, acceleration.y ) );
        lua_pushnumber( lua, acceleration.x );
        lua_pushnumber( lua, acceleration.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//collisionAcceleration
    if( attr->name == "collisionVelocity" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 velocity = collision->GetVelocity();
        //attr->value.SetVec2( Vec2( velocity.x, velocity.y ) );
        lua_pushnumber( lua, velocity.x );
        lua_pushnumber( lua, velocity.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//collisionVelocity
    if( attr->name == "collisionStatic" ) {
      collision = object->GetCollision();
      if( collision ) {
        //attr->value.SetBoolean( collision->IsStatic() );
        lua_pushboolean( lua, collision->IsStatic() );
        ++returnValuesCount;
      } else {
        //attr->value.SetBoolean( false );
        lua_pushboolean( lua, 0 );
        ++returnValuesCount;
      }
      continue;
    }//collisionStatic
    if( attr->name == "collisionForce" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 force = collision->GetForce();
        //attr->value.SetVec2( Vec2( force.x, force.y ) );
        lua_pushnumber( lua, force.x );
        lua_pushnumber( lua, force.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//collisionForce

    //trigger
    if( attr->name == "triggerType" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementType type = trigger->GetType();
        //std::string typeName = "";
        switch( type ) {
        case COLLISION_ELEMENT_TYPE_SQUARE:
          //typeName = "square";
          lua_pushstring( lua, "square" );
          ++returnValuesCount;
          break;
        case COLLISION_ELEMENT_TYPE_CIRCLE:
          //typeName = "circle";
          lua_pushstring( lua, "circle" );
          ++returnValuesCount;
          break;
        case COLLISION_ELEMENT_TYPE_POLYGON:
          //typeName = "polygon";
          lua_pushstring( lua, "polygon" );
          ++returnValuesCount;
          break;
        }
        //attr->value.SetString( typeName );
      } else {
        lua_pushstring( lua, "" );
        ++returnValuesCount;
        //attr->value.SetString( "" );
      }
      continue;
    }//triggerType
    if( attr->name == "triggerSize" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 size = trigger->GetSize();
        //attr->value.SetVec2( Vec2( size.x, size.y ) );
        lua_pushnumber( lua, size.x );
        lua_pushnumber( lua, size.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//triggerSize
    if( attr->name == "triggerOffset" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 offset = trigger->GetOffset();
        //attr->value.SetVec2( Vec2( size.x, size.y ) );
        lua_pushnumber( lua, offset.x );
        lua_pushnumber( lua, offset.y );
        returnValuesCount += 2;
      } else {
        //attr->value.SetVec2( Vec2Null );
        lua_pushnumber( lua, 0.0 );
        lua_pushnumber( lua, 0.0 );
        returnValuesCount += 2;
      }
      continue;
    }//triggerOffset
    if( attr->name == "triggerPolygon" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementPolygon::PointList pointList;
        if( trigger->GetPolygon( pointList ) ) {
          std::string result = "";
          char temp[ 1024 ];
          for( auto &point: pointList ) {
            sprintf_s( temp, sizeof( temp ), "%s%3.3f %3.3f", result.size() ? " " : "", point.x, point.y );
            result += temp;
          }
          //attr->value.SetString( result );
          lua_pushstring( lua, result.c_str() );
          ++returnValuesCount;
        } else {
          //attr->value.SetString( "" );
          lua_pushstring( lua, "" );
          ++returnValuesCount;
        }
      } else {
        //attr->value.SetString( "" );
        lua_pushstring( lua, "" );
        ++returnValuesCount;
      }
      continue;
    }//triggerPolygon
    if( attr->name == "z" ) {
      float z = object->GetPosition().z;
      //attr->value.SetNumber( z );
      lua_pushnumber( lua, z );
      ++returnValuesCount;
      continue;
    }//z
    if( attr->name == "lockToDelete" ) {
      lua_pushboolean( lua, object->GetLockToDelete() );
      ++returnValuesCount;
      continue;
    }//z
  }//get

  /*
  if( !getAttrs.empty() ) { //return parameters
    //__log.PrintInfo( Filelevel_DEBUG, ". getAttrs[%d]", getAttrs.size() );
    VariableAttributesList::iterator iter, iterEnd = getAttrs.end();
    VariableAttribute *var;
    for( iter = getAttrs.begin(); iter != iterEnd; ++iter ) {
      var = *iter;
      //__log.PrintInfo( Filelevel_DEBUG, ". type[%d]", var->value.GetType() );
      switch( ( *iter )->value.GetType() ) {
        case VariableType_NUMBER: {
          //__log.PrintInfo( Filelevel_DEBUG, ". number[%3.3f]", var->value.GetNumber() );
          lua_pushnumber( lua, var->value.GetNumber() );
          ++returnValuesCount;
          break;
        }
        case VariableType_STRING: {
          //__log.PrintInfo( Filelevel_DEBUG, ". string['%s']", var->value.GetString().c_str() );
          lua_pushstring( lua, var->value.GetString().c_str() );
          ++returnValuesCount;
          break;
        }
        case VariableType_BOOLEAN: {
          //__log.PrintInfo( Filelevel_DEBUG, ". bool[%d]", var->value.GetBoolean() );
          lua_pushboolean( lua, var->value.GetBoolean() );
          ++returnValuesCount;
          break;
        }
        case VariableType_NULL: {
          //__log.PrintInfo( Filelevel_DEBUG, ". null" );
          lua_pushnil( lua );
          ++returnValuesCount;
          break;
        }
        case VariableType_VEC2: {
          //__log.PrintInfo( Filelevel_DEBUG, ". vec2[%3.3f; %3.3f]", var->value.GetVec2().x, var->value.GetVec2().y );
          lua_pushnumber( lua, var->value.GetVec2().x );
          lua_pushnumber( lua, var->value.GetVec2().y );
          returnValuesCount += 2;
          break;
        }
        case VariableType_VEC3: {
          //__log.PrintInfo( Filelevel_DEBUG, ". vec3[%3.3f; %3.3f; %3.3f]", var->value.GetVec3().x, var->value.GetVec3().y, var->value.GetVec3().z );
          lua_pushnumber( lua, var->value.GetVec3().x );
          lua_pushnumber( lua, var->value.GetVec3().y );
          lua_pushnumber( lua, var->value.GetVec3().z );
          returnValuesCount += 3;
          break;
        }
        case VariableType_VEC4: {
          //__log.PrintInfo( Filelevel_DEBUG, ". vec4[%3.3f; %3.3f; %3.3f; %3.3f]", var->value.GetVec4().x, var->value.GetVec4().y, var->value.GetVec4().z, var->value.GetVec4().w );
          lua_pushnumber( lua, var->value.GetVec4().x );
          lua_pushnumber( lua, var->value.GetVec4().y );
          lua_pushnumber( lua, var->value.GetVec4().z );
          lua_pushnumber( lua, var->value.GetVec4().w );
          returnValuesCount += 4;
          break;
        }
      }//switch
    }//for
  }
  */

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

  return returnValuesCount;
}//LuaObject_Attr



int Engine::LuaObject_GetRoot( lua_State *lua ) {
  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  ObjectList objectsList;
  lib->game->core->GetRootObjects( objectsList );

  if( objectsList.empty() ) {
    lua_pushnil( lua );
  } else {
    lua_newtable( lua );
    char t[ 1024 ];
    int num = 1;
    for( auto &object: objectsList ) {
      sprintf_s( t, sizeof( t ), "%d", num );
      if( !object->GetLuaObjectId() ) {
        object->InitLuaUserData();
      }
      lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
      lua_getfield( lua, -1, "fn" );
      lua_remove( lua, -2 );
      lua_setfield( lua, -2, t );
      ++num;
    }
  }

  return 1;
}//LuaObject_GetRoot



int Engine::LuaObject_GetChilds( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetChilds => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetChilds => object is NULL" );
  }

  ObjectList objectsList;
  object->GetChilds( objectsList );

  if( objectsList.empty() ) {
    lua_pushnil( lua );
  } else {
    lua_newtable( lua );
    char t[ 1024 ];
    int num = 1;
    for( auto &object: objectsList ) {
      sprintf_s( t, sizeof( t ), "%d", num );
      if( !object->GetLuaObjectId() ) {
        object->InitLuaUserData();
      }
      lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );
      lua_getfield( lua, -1, "fn" );
      lua_remove( lua, -2 );
      lua_setfield( lua, -2, t );
      ++num;
    }
  }

  return 1;
}//LuaObject_GetChilds



int Engine::LuaObject_SetForce( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_SetForce => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_SetForce => object is NULL" );
  }

  int argc = lua_gettop( lua );

  if( argc != 4 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetForce => use: Object:SetForce( forceId, powerX, powerY )" );
    luaL_error( lua, "Engine::LuaObject_SetForce => use: Object:SetForce( forceId, powerX, powerY )" );
    return 0;
  }

  long forceId = lua_tointeger( lua, 2 );
  Vec3 force( ( float ) lua_tonumber( lua, 3 ), ( float ) lua_tonumber( lua, 4 ), 0.0f );

  object->SetForce( forceId, force );

  return 0;
}//LuaObject_SetForce



int Engine::LuaObject_RemoveForce( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_RemoveForce => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_RemoveForce => object is NULL" );
  }

  int argc = lua_gettop( lua );

  if( argc != 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_RemoveForce => use: Object:RemoveForce( forceId )" );
    luaL_error( lua, "Engine::LuaObject_RemoveForce => use: Object:RemoveForce( forceId )" );
    return 0;
  }

  long forceId = lua_tointeger( lua, 2 );

  object->RemoveForce( forceId );

  return 0;
}//LuaObject_RemoveForce


int Engine::LuaObject_StopAnimation( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_StopAnimation => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_StopAnimation => object is NULL" );
  }

  object->StopAnimation();

  return 0;
}//LuaObject_StopAnimation



/*
static int Object_gc( lua_State *lua ) {
  return Engine::LuaObject::GarbageCollector( lua, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
}//Object_gc
*/
