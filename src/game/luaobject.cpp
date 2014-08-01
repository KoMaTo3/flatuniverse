﻿#include <vector>
#include "luaobject.h"
#include "core/file.h"

extern "C" {
  #include "luabins/saveload.h"
  #include "luabins/luabins.h"
}


using namespace Engine;


LuaObject::_LuaObjectLibraryLoader LuaObject::_luaObjectLibraryLoader;
std::vector< LuaObject::LuaObjectLibrary > LuaObjectLibraryList;
#define LUAOBJECT_TEMPLATE_FIELD_NAME "fn"
#define LUAOBJECT_DATA_FIELD_NAME "data"
#define LUAOBJECT_FUNCTION_ONCREATE "OnCreate"


LuaObject::LuaObject() {
}//


LuaObject::~LuaObject() {
}//~


void LuaObject::InitLibrary( Game *setGame, const LUAOBJECT_LIBRARIESLIST libraryType, lua_State *luaState, const std::string& setLibraryName, const std::string& setLuaObjectsTableName, const luaL_Reg *object_lib, const luaL_Reg *lua_lib, lua_CFunction gcFunction ) {
  if( LuaObjectLibraryList.empty() ) {
    LuaObjectLibraryList.resize( LUAOBJECT_LIBRARY_FFFF - 1 );
  }

  const size_t type = libraryType - 1;
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ type ];
  if( lib->lua ) {
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "LuaObject::InitLibrary => name['%s']", setLibraryName.c_str() );

  lib->lua = luaState;
  lib->name = setLibraryName;
  lib->luaObjectsTableName = setLuaObjectsTableName;
  lib->game = setGame;

  LuaObject::_luaObjectLibraryLoader.object_lib = object_lib;
  LuaObject::_luaObjectLibraryLoader.lua_lib = lua_lib;
  LuaObject::_luaObjectLibraryLoader.gcFunction = gcFunction;
  LuaObject::_luaObjectLibraryLoader.lib = lib;

  luaL_requiref( lib->lua, lib->name.c_str(), LuaObject::__LoadLibrary, 1 );
  lua_pop( lib->lua, 1 );

  LuaObject::_luaObjectLibraryLoader.object_lib = NULL;
  LuaObject::_luaObjectLibraryLoader.lua_lib = NULL;
  LuaObject::_luaObjectLibraryLoader.gcFunction = NULL;
  LuaObject::_luaObjectLibraryLoader.lib = NULL;

  __log.PrintInfo( Filelevel_DEBUG, "LuaObject::InitLibrary => library '%s' loaded", lib->name.c_str() );
}//InitLibrary


int LuaObject::__LoadLibrary( lua_State *lua ) {
  luaL_newlib( lua, LuaObject::_luaObjectLibraryLoader.lua_lib );

  // Stack: MyLib
  luaL_newmetatable( lua, LuaObject::_luaObjectLibraryLoader.lib->name.c_str() );
  luaL_newlib( lua, LuaObject::_luaObjectLibraryLoader.object_lib );
  lua_setfield( lua, -2, "__index" );

  if( LuaObject::_luaObjectLibraryLoader.gcFunction ) {
    lua_pushstring( lua, "__gc" );
    lua_pushcfunction( lua, LuaObject::_luaObjectLibraryLoader.gcFunction );
    lua_settable( lua, -3 );
  }
  lua_pop( lua, 1 );

  if( !LuaObject::_luaObjectLibraryLoader.lib->luaObjectsTableName.empty() ) {
    lua_pushglobaltable( lua );
    lua_newtable( lua );
    lua_setfield( lua, -2, LuaObject::_luaObjectLibraryLoader.lib->luaObjectsTableName.c_str() );
    lua_pop( lua, 1 );
  }

  return 1;
}//__LoadLibrary


/*
int LuaObject::GarbageCollector( lua_State *lua, LUAOBJECT_LIBRARIESLIST libraryType ) {
  Engine::LuaObject::LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];
  //printf( ". library '%s'\n", lib->name.c_str() );

  Engine::LuaObject **pobject = reinterpret_cast<Engine::LuaObject **>( lua_touserdata( lua, 1 ) );
  printf( "LuaObject::GarbageCollector of type %d, object x%p\n", libraryType, *pobject );
  if( *pobject ) {
    luaL_argcheck( lua, *pobject != NULL, 1, "Error foo" );
    LuaObject::STLEraseByPointer( lib->objectsList, pobject );
    //printf( ". erased\n" );
  }
  //printf( ". gc done\n" );
  return 0;
}//GarbageCollector
*/


const LuaObjectType* LuaObject::Push( lua_State *lua, const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const bool insertToObjectsList ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  const LuaObjectType **pobject = static_cast< const LuaObjectType ** >( lua_newuserdata( lua, sizeof( LuaObjectType** ) ) );
  *pobject = object;

  luaL_getmetatable( lua, lib->name.c_str() );
  lua_setmetatable( lua, -2 );

  if( insertToObjectsList ) {
    lib->objectsList.push_back( object );
  }

  return object;
}//Push


/* LuaObject** LuaObject::New( lua_State *lua, LuaObject *object, LUAOBJECT_LIBRARIESLIST libraryType ) {
  LuaObject **pobject = LuaObject::Push( lua, object, &LuaObjectLibraryList[ libraryType - 1 ] );
  return pobject;
}//New
*/


const LuaObjectPointerList* LuaObject::GetObjectsList( LUAOBJECT_LIBRARIESLIST libraryType ) {
  return &LuaObjectLibraryList[ libraryType - 1 ].objectsList;
}//GetObjectsList


LuaObjectType* LuaObject::Check( LUAOBJECT_LIBRARIESLIST libraryType ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  LuaObjectType *object;
  luaL_checktype( lib->lua, 1, LUA_TUSERDATA );
  object = *static_cast<LuaObjectType **>( luaL_checkudata( lib->lua, 1, lib->name.c_str() ) );
  if( object == NULL ) {
    luaL_error( lib->lua, "error" );
  }

  return object;
}//Check


void LuaObject::Destroy( LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  auto
    iter = lib->objectsList.begin(),
    iterEnd = lib->objectsList.end();
  while( iter != iterEnd ) {
    if( *iter == object ) {
      __log.PrintInfo( Filelevel_DEBUG, "LuaObject::Destroy..." );
      lib->objectsList.erase( iter );
      //iter = lib->objectsList.begin();
      //iterEnd = lib->objectsList.end();
      //continue;
      break;
    }
    ++iter;
  }

  lua_pushnil( lib->lua );
  lua_setmetatable( lib->lua, 1 );
}//Destroy


void LuaObject::RemoveFromLuaTable( const std::string& objectName, LUAOBJECT_LIBRARIESLIST libraryType ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  if( !lib->luaObjectsTableName.empty() ) {
    lua_getglobal( lib->lua, lib->luaObjectsTableName.c_str() );
    lua_pushnil( lib->lua );
    lua_setfield( lib->lua, -2, objectName.c_str() );
    lua_pop( lib->lua, 1 );
  }
}//RemoveFromLuaTable


void LuaObject::RemoveFromLuaTable( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType ) {
  if( luaObjectId ) {
    LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];
    luaL_unref( lib->lua, LUA_REGISTRYINDEX, luaObjectId );
  }
}//RemoveFromLuaTable


/*
  ВНИМАНИЕ: поле data затирается шаблоном
*/
void LuaObject::BindTemplateToObject( const int luaObjectId, const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const std::string& luaTemplateTable ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  //lua_getglobal( lib->lua, lib->luaObjectsTableName.c_str() );
  //lua_getfield( lib->lua, -1, objectName.c_str() );
  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );

  int result = 0;

  if( luaTemplateTable.empty() ) {
    lua_newtable( lib->lua );
  } else {
    result = luaL_dostring( lib->lua, luaTemplateTable.c_str() );
  }

  if( result ) {
    __log.PrintInfo( Filelevel_ERROR, "LuaObject::BindTemplateToObject failed: %s", lua_tostring( lib->lua, -1 ) );
  } else {
    lua_newtable( lib->lua );
    lua_setfield( lib->lua, -2, LUAOBJECT_DATA_FIELD_NAME );

    LuaObject::Push( lib->lua, object, libraryType, false );
    lua_setfield( lib->lua, -2, "api" );

    lua_setfield( lib->lua, -2, LUAOBJECT_TEMPLATE_FIELD_NAME );
  }

  lua_pop( lib->lua, 1 );
}//BindTemplateToObject


void LuaObject::LoadObjectDataFromDump( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, const unsigned char* data, size_t dataSize ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  //lua_getglobal( lib->lua, lib->luaObjectsTableName.c_str() );
  //lua_getfield( lib->lua, -1, objectName.c_str() );
  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );

  lua_getfield( lib->lua, -1, LUAOBJECT_TEMPLATE_FIELD_NAME );

  int count = 0;
  int result = luabins_load( lib->lua, data, dataSize, &count );
  if( result ) {
    __log.PrintInfo( Filelevel_ERROR, "LuaObject::LoadObjectDataFromDump => failed: %s", lua_tostring( lib->lua, -1 ) );
  } else {
    lua_setfield( lib->lua, -2, LUAOBJECT_DATA_FIELD_NAME );

    /*
    lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );
    lua_getfield( lib->lua, -1, LUAOBJECT_TEMPLATE_FIELD_NAME );
    lua_getfield( lib->lua, -1, LUAOBJECT_DATA_FIELD_NAME );
    lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );
    lua_setfield( lib->lua, -2, "api" );
    lua_pop( lib->lua, 3 );
    */
  }

  lua_pop( lib->lua, 2 );
}//LoadObjectDataFromDump


bool LuaObject::SaveObjectDataToDump( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, char *&outData, size_t &dataSize ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];
  bool saveResult = false;

  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId ); //1

  lua_getfield( lib->lua, -1, LUAOBJECT_TEMPLATE_FIELD_NAME ); //2
  lua_getfield( lib->lua, -1, LUAOBJECT_DATA_FIELD_NAME ); //3
  int stack = lua_gettop( lib->lua );
  int result = luabins_save( lib->lua, stack, stack ); //3/4
  if( result == 0 ) { //4
    outData = const_cast< char* >( lua_tolstring( lib->lua, -1, &dataSize ) );
    lua_pop( lib->lua, 1 ); //3
    saveResult = true;
  }
  lua_pop( lib->lua, 3 );

  return saveResult;
}//SaveObjectDataToDump


/*
void LuaObject::InitUserData( const LuaObjectType *object, const std::string& objectName, LUAOBJECT_LIBRARIESLIST libraryType, const bool returnLuaObjectToStack ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  lua_getglobal( lib->lua, lib->luaObjectsTableName.c_str() ); //__gameobjects
  lua_createtable( lib->lua, 0, 1 ); //__gameobjects table
  //LuaObject **pobject = LuaObject_push( lua, new LuaObject( name, true ) );
  //LuaObject **pobject = reinterpret_cast< LuaObject** >( Engine::LuaObject::New( lua, new LuaObject( name, true ), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_LUAOBJECT ) );
  LuaObject::Push( lib->lua, object, libraryType ); //__gameobjects table userdata

  lua_setfield( lib->lua, -2, "api" ); //__gameobjects table
  lua_newtable( lib->lua ); //__gameobjects table table
  lua_setfield( lib->lua, -2, "fn" ); //__gameobjects table
  lua_getfield( lib->lua, -1, "fn" ); //__gameobjects table fn
  lua_newtable( lib->lua ); //__gameobjects table fn table
  lua_setfield( lib->lua, -2, "data" ); //__gameobjects table fn
  lua_pop( lib->lua, 1 ); //__gameobjects table

  lua_setfield( lib->lua, -2, objectName.c_str() ); //__gameobjects

  if( returnLuaObjectToStack ) {
    lua_getfield( lib->lua, -1, objectName.c_str() ); //__gameobjects object
    lua_remove( lib->lua, -2 ); //object
  } else {
    lua_pop( lib->lua, 1 );
  }
}//InitUserData
*/


int LuaObject::InitUserData( const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const bool returnLuaObjectToStack ) {
  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  lua_createtable( lib->lua, 0, 1 ); //1=object

  lua_newtable( lib->lua ); //2=fn

  LuaObject::Push( lib->lua, object, libraryType, false ); //3=api - false:незачем содержать очередной список объектов, всЄ есть в Core
  lua_setfield( lib->lua, -2, "api" ); //2

  lua_newtable( lib->lua ); //3=data
  lua_setfield( lib->lua, -2, "data" ); //2

  lua_setfield( lib->lua, -2, LUAOBJECT_TEMPLATE_FIELD_NAME ); //1

  /*
  LuaObject::Push( lib->lua, object, libraryType ); //__gameobjects table userdata
  lua_setfield( lib->lua, -2, "api" ); //__gameobjects table
  lua_newtable( lib->lua ); //__gameobjects table table
  lua_setfield( lib->lua, -2, "fn" ); //__gameobjects table
  lua_getfield( lib->lua, -1, "fn" ); //__gameobjects table fn
  lua_newtable( lib->lua ); //__gameobjects table fn table
  lua_setfield( lib->lua, -2, "data" ); //__gameobjects table fn
  lua_pop( lib->lua, 1 ); //__gameobjects table
  */

  int refId = luaL_ref( lib->lua, LUA_REGISTRYINDEX );

  if( returnLuaObjectToStack ) {
    lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, refId );
  }

  return refId;
}//InitUserData


bool LuaObject::LuaCheckStackParametersCount( LUAOBJECT_LIBRARIESLIST libraryType, int minCount ) {
  return
    lua_gettop( LuaObjectLibraryList[ libraryType - 1 ].lua ) >= minCount;
}//LuaCheckStackParametersCount


bool LuaObject::LuaCheckStackParametersCountExact( LUAOBJECT_LIBRARIESLIST libraryType, int count ) {
  return
    lua_gettop( LuaObjectLibraryList[ libraryType - 1 ].lua ) == count;
}//LuaCheckStackParametersCount


const char* LuaObject::LuaGetString( LUAOBJECT_LIBRARIESLIST libraryType, int n ) {
  return luaL_checkstring( LuaObjectLibraryList[ libraryType - 1 ].lua, n );
}//LuaGetString


const bool LuaObject::LuaGetBoolean( LUAOBJECT_LIBRARIESLIST libraryType, int n ) {
  return lua_toboolean( LuaObjectLibraryList[ libraryType - 1 ].lua, n ) ? true : false;
}//LuaGetBoolean


void LuaObject::LuaSetNil( LUAOBJECT_LIBRARIESLIST libraryType ) {
  lua_pushnil( LuaObjectLibraryList[ libraryType - 1 ].lua );
}//LuaSetNil


const LuaObject::LuaObjectLibrary* LuaObject::GetLibrary( LUAOBJECT_LIBRARIESLIST libraryType ) {
  return &LuaObjectLibraryList[ libraryType - 1 ];
}//GetLibrary


void LuaObject::CallFunction( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, const std::string& functionName ) {
  if( !luaObjectId ) {
    __log.PrintInfo( Filelevel_ERROR, "LuaObject::CallFunction => luaObjectId is NULL" );
    return;
  }

  LuaObjectLibrary *lib = &LuaObjectLibraryList[ libraryType - 1 ];

  lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );
  lua_getfield( lib->lua, -1, LUAOBJECT_TEMPLATE_FIELD_NAME );
  lua_getfield( lib->lua, -1, functionName.c_str() );
  if( !lua_isfunction( lib->lua, -1 ) ) {
    __log.PrintInfo( Filelevel_WARNING, "LuaObject::CallFunction => function '%s' not exists in object luaObjectId[%d]", functionName.c_str(), luaObjectId );
    lua_pop( lib->lua, 3 );
  } else {
    lua_rawgeti( lib->lua, LUA_REGISTRYINDEX, luaObjectId );
    lua_getfield( lib->lua, -1, LUAOBJECT_TEMPLATE_FIELD_NAME );
    lua_remove( lib->lua, -2 );
    if( lua_pcall( lib->lua, 1, 0, 0 ) ) {
      __log.PrintInfo( Filelevel_ERROR, "LuaObject::CallFunction => failed to call function '%s' in object %d: %s", functionName.c_str(), luaObjectId, lua_tostring( lib->lua, -1 ) );
      lua_pop( lib->lua, 1 );
    }
    lua_pop( lib->lua, 3 );
  }
}//CallFunction
