#include "object.h"
#include "../../game/game.h"
#include "../../core/object.h"
#include "../../core/file.h"
#include "../../core/animationpack.h"

extern "C" {
#include "../../lua/lua.h"
#include "../../lua/lualib.h"
#include "../../lua/lauxlib.h"

#include "../../luabins/luabins.h"
#include "../../luabins/saveload.h"
}


using namespace Engine;


int Engine::LuaObject_New( lua_State *lua ) {
  int argc = lua_gettop( lua );

  if( argc < 1 ) {
    luaL_error( lua, "Engine::LuaObject_New => object name required" );
  }

  const char* nameStr = luaL_checkstring( lua, 1 );
  const char* parentName = NULL;
  bool setUserData = false;
  //bool isSaveable = true;

  if( argc > 1 ) {
    parentName = luaL_checkstring( lua, 2 );
    if( !parentName[ 0 ] ) {
      parentName = NULL;
    }

    if( argc > 2 ) {
      setUserData = lua_toboolean( lua, 3 ) ? true : false;

      /*
      if( argc > 3 ) {
        isSaveable = lua_toboolean( lua, 4 ) ? true : false;
      }
      */
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
      obj->SetPosition( Vec3Null );
      currentPath += ( currentPath.length() ? "/" : "" ) + *iter;
      lib->game->world->AttachObjectToGrid( lib->game->world->GetGridPositionByObject( *obj ), obj );
    }
    //std::string nextLevel = &name[ slashPos ];
    //std::string currentLevel = name.substr( 0, slashPos );
    //parentObj = game->core->GetObject( currentLevel );
  } else {
    obj = lib->game->core->CreateObject( name, NULL, true );
    obj->SetPosition( Vec3Null );
    obj->Update( 0.0f );
    if( !notInGrid ) {
      World::Grid::Position gridPos = lib->game->world->GetGridPositionByObject( *obj );
      lib->game->world->AttachObjectToGrid( gridPos, obj );
    }
  }


  if( !setUserData ) {
    lua_pushnil( lua );
  }

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

  /*
  const Object *object = NULL;
  auto objectsList = Engine::LuaObject::GetObjectsList( Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
  auto
    iter = objectsList->begin(),
    iterEnd = objectsList->end();
  while( iter != iterEnd ) {
    if( *iter && ( static_cast< const Object* >( *iter ) )->get == name ) {
      object = static_cast< const Object* >( *iter );
      break;
    }
    ++iter;
  }
  */
  Object *object = lib->game->core->GetObject( name );
  if( object ) {
    lua_rawgeti( lua, LUA_REGISTRYINDEX, object->GetLuaObjectId() );

    //TODO: если объект не найден в таблице __gameobjects, то его нужно проинициализировать по аналогии с LuaObject.new
    //хотя, значит объект не скриптован, может и не надо, выводить ошибку
    //lua_getglobal( lua, "__sprites" );
    //lua_getfield( lua, -1, object->spriteName.c_str() );
    //lua_remove( lua, -2 );
  } else {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_Get => object '%s' not found", name );
    lua_pushnil( lua );
  }

  return 1;
}//LuaObject_Get


int Engine::LuaObject_Destroy( lua_State *lua ) {
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
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject_SetAnimation => not enough parameters", argc );
    return 0;
  }
  str = lua_tostring( lua, 2 );
  std::string
    templateName = str,
    animation,
    animationAfterAnimationComplete;

  if( argc > 2 ) {
    str = lua_tostring( lua, 3 );
    animation = str;

    if( argc > 3 ) {
      str = lua_tostring( lua, 4 );
      animationAfterAnimationComplete = str;
    } else {
      animationAfterAnimationComplete = animation;
    }
  }

  Animation::AnimationSetAction actionAfterAnimation( Animation::ANIMATION_SET_ACTION_REPEAT, animationAfterAnimationComplete );
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

  Animation::AnimationPack *pack = object->ApplyAnimation( actionAfterAnimation, templateName, animation );
  if( pack ) {
    pack->SetEnabled( true );
  }

  return 0;
}//Object_SetAnimation


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
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetName => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetName => object is NULL" );
  }

  lua_pushstring( lua, object->GetNameFull().c_str() );

  return 1;
}//LuaObject_GetNameFull


int Engine::LuaObject_GetParentNameFull( lua_State *lua ) {
  luaL_checktype( lua, 1, LUA_TUSERDATA );
  Object *object = *static_cast<Object **>( luaL_checkudata( lua, 1, "Object" ) );
  if( object == NULL ) {
    __log.PrintInfo( Filelevel_WARNING, "Engine::LuaObject_GetName => object is NULL" );
    luaL_error( lua, "Engine::LuaObject_GetName => object is NULL" );
  }

  lua_pushstring( lua, object->GetParentNameFull().c_str() );

  return 1;
}//LuaObject_GetParentNameFull



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
