#include "object.h"
#include "game/game.h"
#include "core/object.h"
#include "core/file.h"
#include "core/animationpack.h"
#include "core/animationpack.h"
#include "game/luastatecheck.h"

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
    __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_Get => id %d", object->GetLuaObjectId() );
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
  __log.PrintInfo( Filelevel_DEBUG, "Engine::LuaObject_SetAnimation => luaFunctionId[%d]", actionAfterAnimation.luaFunctionId );

  Animation::AnimationPack *pack = object->ApplyAnimation( actionAfterAnimation, templateName, animation );
  if( pack ) {
    pack->SetEnabled( true );
  }

  return 0;
}//Object_SetAnimation


void Engine::LuaObject_AnimationCallback( Animation::AnimationSetAction *action ) {
  if( !action->luaFunctionId ) {
    return;
  }
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
  lua_pcall( lib->lua, 1, 0, 0 );
  luaL_unref( lib->lua, LUA_REGISTRYINDEX, action->luaFunctionId );
  action->luaFunctionId = 0;
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
  if( argc != 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetPos => use: object.api:SetPos( x, y )" );
    luaL_error( lua, "Engine::LuaObject_SetPos => use: object.api:SetPos( x, y )" );
    return 0;
  }

  const Vec2 pos( ( float ) lua_tonumber( lua, 2 ), ( float ) lua_tonumber( lua, 3 ) );

  object->SetPosition2D( pos );
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

  int argc = lua_gettop( lua );
  if( argc != 2 ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_AddTrigger => use: object.api:AddTrigger( functionName )" );
    luaL_error( lua, "Engine::LuaObject_AddTrigger => use: object.api:AddTrigger( functionName )" );
    return 0;
  }

  ObjectTrigger *trigger = object->GetTrigger();
  if( !trigger ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAddTrigger => object '%s' don't have trigger", object->GetNameFull().c_str() );
    return 0;
  }

  const char *funcName = lua_tostring( lua, 2 );
  trigger->AddHandler( Game::LUA_ObjectTrigger_Handler );

  auto lib = Engine::LuaObject::GetLibrary( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );

  lib->game->objectTriggers.push_back( Game::GameObjectTrigger() );
  Game::GameObjectTrigger *objectTrigger = &( *lib->game->objectTriggers.rbegin() );
  objectTrigger->funcName = funcName;
  objectTrigger->trigger  = trigger;

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



/*
static int Object_gc( lua_State *lua ) {
  return Engine::LuaObject::GarbageCollector( lua, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
}//Object_gc
*/


/*
static int Sprite_Callback( lua_State *lua ) {
  Sprite *object = static_cast< Sprite* >( Engine::LuaObject::Check( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE ) );
  luaL_argcheck( lua, object != NULL, 1, "Error object" );

  int callbackReference = 0;

  if( lua_isfunction( lua, 2 ) ) {
    lua_pushvalue( lua, 2 );
    callbackReference = luaL_ref( lua, LUA_REGISTRYINDEX );
    lua_pop( lua, 1 );
  }
  printf( "callbackReference = %d\n", callbackReference );
  object->callbackFunction = callbackReference;

  return 0;
}//Sprite_Callback


static int Sprite_CallbackDo( lua_State *lua ) {
  Sprite *object = static_cast< Sprite* >( Engine::LuaObject::Check( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE ) );
  luaL_argcheck( lua, object != NULL, 1, "Error object" );

  int num = lua_tointeger( lua, 2 );

  if( object->callbackFunction ) {
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->callbackFunction );
    //luaL_unref( lua, LUA_REGISTRYINDEX, object->callbackFunction );
    lua_pushnumber( lua, num );
    lua_pcall( lua, 1, 0, 0 );
  }

  return 0;
}//Sprite_CallbackDo


static int Sprite_CallbackRemove( lua_State *lua ) {
  Sprite *object = static_cast< Sprite* >( Engine::LuaObject::Check( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE ) );
  luaL_argcheck( lua, object != NULL, 1, "Error object" );

  if( object->callbackFunction ) {
    luaL_unref( lua, LUA_REGISTRYINDEX, object->callbackFunction );
  }

  return 0;
}//Sprite_CallbackRemove


static int Sprite_Save( lua_State *lua ) {
  Sprite *object = static_cast< Sprite* >( Engine::LuaObject::Check( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE ) );
  luaL_argcheck( lua, object != NULL, 1, "Error object" );

  const char* dumpFileName = luaL_checkstring( lua, 2 );
  char *str;
  size_t size = 0;
  Engine::LuaObject::SaveObjectDataToDump( object->luaObjectId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE, str, size );

  FILE *f = NULL;
  fopen_s( &f, dumpFileName, "wb+" );
  fwrite( str, size, 1, f );
  fclose( f );

  return 0;
}//Sprite_Save


static int Sprite_Load( lua_State *lua ) {
  Sprite *object = static_cast< Sprite* >( Engine::LuaObject::Check( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE ) );
  luaL_argcheck( lua, object != NULL, 1, "Error object" );

  const char* dumpFileName = luaL_checkstring( lua, 2 );
  const char* scriptFileName = luaL_checkstring( lua, 3 );

  FILE *fscripts = NULL;
  fopen_s( &fscripts, scriptFileName, "rb+" );
  fseek( fscripts, 0, SEEK_END );
  size_t size = ftell( fscripts );
  char *script = new char[ size + 1 ];
  script[ size ] = 0;
  fseek( fscripts, 0, SEEK_SET );
  fread( script, size, 1, fscripts );
  fclose( fscripts );

  FILE *f = NULL;
  fopen_s( &f, dumpFileName, "rb+" );
  fseek( f, 0, SEEK_END );
  size = ftell( f );
  unsigned char *str = new unsigned char[ size + 1 ];
  str[ size ] = 0;
  fseek( f, 0, SEEK_SET );
  fread( str, size, 1, f );
  fclose( f );

  Engine::LuaObject::BindTemplateToObject( object->luaObjectId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE, script );
  Engine::LuaObject::LoadObjectDataFromDump( object->luaObjectId, object, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_SPRITE, str, size );

  delete [] str;

  return 0;
}//Sprite_Load
*/
