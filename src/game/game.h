#pragma once

#include "core/core.h"
#include "core/object.h"
#include "worldgridmgr.h"
#include "lua.h"
#include <vector>
#include <map>

class Lua;

class Game
{
public:
  struct
  {
    bool show;
    Object *obj[ 40 ];
  } gridTooltipUnderMouse;

  Core              *core;
  WorldGridManager  *world;
  Lua               *lua;
  bool              isActive; //true - playing, false - editing

  //lua-таймеры
  struct GameLuaTimer
  {
    bool active;
    float time;
    std::string funcName;
    bool dontPause; //не паузится от Game::SetActive
  };
  std::vector< GameLuaTimer > luaTimers;

  //lua-слушатели клавиатуры и мыши
  typedef std::deque< std::string > luaKeyboardListenersList;
  luaKeyboardListenersList
      luaKeyboardListeners,
      luaMouseKeyListeners,
      luaMouseMoveListeners;

  //lua-слушатели коллизий
  luaCollisionListenersList luaCollisionListeners;
  luaTriggerListenersList luaTriggerListeners;

  //триггеры
  struct GameObjectTrigger
  {
    std::string   funcName;
    ObjectTrigger *trigger;
  };
  typedef std::deque< GameObjectTrigger > GameObjectTriggerList;
  GameObjectTriggerList objectTriggers;

public:
  Game();
  ~Game();
  static bool   LUA_RemoveObject      ( const std::string &name );
  static Vec2   LUA_GetObjectPos      ( const std::string &name );
  static void   LUA_SetObjectPos      ( const std::string &name, const Vec2 &pos );
  static Dword  LUA_SetTimer          ( float time, const std::string &funcName, bool dontPause );
  static void   LUA_StopTimer         ( Dword id );
  static void   LUA_CreateObject      ( const std::string &name, const Vec3 &pos, int notInGrid );
  static void   LUA_ListenKeyboard    ( const std::string &funcName );
  static void   LUA_ListenMouseKey    ( const std::string &funcName );
  static void   LUA_ListenMouseMove   ( const std::string &funcName );
  static void   LUA_GameExit          ();
  static Vec2   LUA_GetMousePos       ();
  static Vec2   LUA_GetCameraPos      ();
  static float  LUA_GetGridSize       ();
  static Size   LUA_GetWindowSize     ();
  static void   LUA_ObjectAddTrigger  ( const std::string &triggerName, const std::string &funcName );
  static void   LUA_ObjectTrigger_Handler( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );
  static void   LUA_SetCamera         ( const std::string &name );
  static std::string  LUA_GetCamera   ();
  static void   LUA_ClearScene        ();
  static void   LUA_ObjectEnableRenderable  ( const std::string &objectName, const std::string &texture, const Vec2 &size, const Vec4 &color );
  static void   LUA_ObjectDisableRenderable ( const std::string &objectName );
  static void   LUA_ObjectEnableCollision   ( const std::string &objectName, bool isStatic, const Vec3 &size, const Vec3 &velocity, const Vec3 &acceleration );
  static void   LUA_ObjectDisableCollision  ( const std::string &objectName );
  static void   LUA_ObjectEnableTrigger     ( const std::string &objectName, const Vec3 &size );
  static void   LUA_ObjectDisableTrigger    ( const std::string &objectName );
  static bool   LUA_GetCollisionStatic( const std::string &name );
  static void   LUA_SetCollisionStatic( const std::string &name, bool isStatic );
  static void   LUA_DebugRender       ( int flags );
  static std::string LUA_GetObjectByPoint   ( int type, const Vec2 &point, const std::string &afterObject );
  static std::string LUA_GetObjectByRect    ( int type, const Vec2 &leftTop, const Vec2 &rightBottom );
  static void   LUA_SelectObject            ( const std::string &name );
  static std::string LUA_GetSelectedObject  ();
  static void   LUA_LoadScript        ( const std::string &fileName );
  static void   LUA_ObjectAttr        ( const std::string &objectName, VariableAttributesList &setAttributes, VariableAttributesList &getAttributes );
  static void   LUA_ListenCollision   ( const std::string &objectName, const std::string &funcName );
  static void   LUA_ListenTrigger     ( const std::string &objectName, const std::string &funcName );
  static void   LUA_SetObjectForce    ( const std::string &objectName, int num, const Vec2& force );
  static void   LUA_RemoveObjectForce ( const std::string &objectName, int num );
  static bool   LUA_ObjectHasTag      ( const std::string &objectName, const std::string &tag );
  static void   LUA_ObjectAddTag      ( const std::string &objectName, const std::string &tag );
  static void   LUA_ObjectRemoveTag   ( const std::string &objectName, const std::string &tag );
  static void   LUA_ObjectSetAnimation( const std::string &objectName, const std::string &templateName, const std::string &animation );
  static void   LUA_ObjectStopAnimation ( const std::string &objectName );
  static void   LUA_SetPause          ( bool isPause );

  void Update();
  void UpdateLuaTimers();
  void SetActive( bool setActive );
  void ClearLuaTimers();
  void StringToObjectList( const std::string &str, ObjectList &list ) const;
  std::string ObjectListToString( const ObjectList &list ) const;

  static void   KeyboardProc          ( Dword keyId, bool isPressed );
  static void   MouseKeyProc          ( Dword keyId, bool isPressed );
  static void   MouseMoveProc         ( const Vec2 &pos );
  static void   OnRemoveTrigger       ( ObjectTrigger *trigger );
  static void   CollisionProc         ( Collision *a, Collision *b, Byte flags, const Vec3 &velocity );
  static void   TriggerProc           ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );

  //debug/test
};
