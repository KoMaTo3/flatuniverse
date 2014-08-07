#pragma once


#include <string>
#include <deque>


extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}



class Game;
class Lua;

namespace Engine {


enum LUAOBJECT_LIBRARIESLIST {
  LUAOBJECT_LIBRARY_0000 = 0,

  LUAOBJECT_LIBRARY_OBJECT,
  LUAOBJECT_LIBRARY_DEBUG,
  LUAOBJECT_LIBRARY_CORE,
  LUAOBJECT_LIBRARY_KEYBOARD,
  LUAOBJECT_LIBRARY_MOUSE,
  LUAOBJECT_LIBRARY_CAMERA,
  LUAOBJECT_LIBRARY_CONFIG,
  LUAOBJECT_LIBRARY_SCENE,
  LUAOBJECT_LIBRARY_TOOLS,
  
  LUAOBJECT_LIBRARY_FFFF,
};

class LuaObject;


typedef void LuaObjectType;
typedef std::deque< const Engine::LuaObjectType* > LuaObjectPointerList;


class LuaObject {
public:
  static void InitLibrary( Game *setGame, const LUAOBJECT_LIBRARIESLIST libraryType, lua_State *luaState, const std::string& setLibraryName, const std::string& setLuaObjectsTableName, const luaL_Reg *object_lib, const luaL_Reg *lua_lib, lua_CFunction gcFunction = NULL );

  LuaObject();
  virtual ~LuaObject();

  struct LuaObjectLibrary {
    LUAOBJECT_LIBRARIESLIST type;
    std::string name;
    lua_State *lua;
    LuaObjectPointerList objectsList;
    std::string luaObjectsTableName;
    Game *game;

    LuaObjectLibrary()
    :type( LUAOBJECT_LIBRARY_0000 ), name( "" ), lua( NULL ) {
    }
  };

  //static int GarbageCollector( lua_State *lua, LUAOBJECT_LIBRARIESLIST libraryType );
  //static LuaObject** New( lua_State *lua, LuaObject *object, LUAOBJECT_LIBRARIESLIST libraryType );
  static const LuaObjectType* Push( lua_State *lua, const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const bool insertToObjectsList = true );
  static const LuaObjectPointerList* GetObjectsList( LUAOBJECT_LIBRARIESLIST libraryType );
  static LuaObjectType* Check( LUAOBJECT_LIBRARIESLIST libraryType );
  static void Destroy( LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType );
  static void RemoveFromLuaTable( const std::string& objectName, LUAOBJECT_LIBRARIESLIST libraryType );
  static void RemoveFromLuaTable( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType );
  static void BindTemplateToObject( const int luaObjectId, const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const char* luaTemplateTable );
  static void LoadObjectDataFromDump( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, const unsigned char *data, size_t dataSize );
  static bool SaveObjectDataToDump( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, char *&outData, size_t &dataSize );
  static int InitUserData( const LuaObjectType *object, LUAOBJECT_LIBRARIESLIST libraryType, const bool returnLuaObjectToStack = true );
  static void InitUserData( const LuaObjectType *object, const std::string& objectName, LUAOBJECT_LIBRARIESLIST libraryType, const bool returnLuaObjectToStack = true );
  static void CallFunction( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, const std::string& functionName );
  static bool IsFunctionExists( const int luaObjectId, LUAOBJECT_LIBRARIESLIST libraryType, const std::string& functionName );

  static bool LuaCheckStackParametersCount( LUAOBJECT_LIBRARIESLIST libraryType, int minCount );
  static bool LuaCheckStackParametersCountExact( LUAOBJECT_LIBRARIESLIST libraryType, int count );
  static const char* LuaGetString( LUAOBJECT_LIBRARIESLIST libraryType, int n );
  static const bool LuaGetBoolean( LUAOBJECT_LIBRARIESLIST libraryType, int n );
  static void LuaSetNil( LUAOBJECT_LIBRARIESLIST libraryType );
  static const LuaObjectLibrary* GetLibrary( LUAOBJECT_LIBRARIESLIST libraryType );

private:
  static int __LoadLibrary( lua_State *lua );

  struct _LuaObjectLibraryLoader {
    const luaL_Reg *object_lib;
    const luaL_Reg *lua_lib;
    lua_CFunction gcFunction;
    LuaObjectLibrary *lib;
  };

  static _LuaObjectLibraryLoader _luaObjectLibraryLoader;

  template< class STLContainer, class T >
  static void STLEraseByPointer( STLContainer &container, T *item ) {
    auto
      iter = container.begin(),
      iterEnd = container.end();
    while( iter != iterEnd ) {
      if( *iter == item ) {
        container.erase( iter );
        break;
      }
      ++iter;
    }
  }
};


};
