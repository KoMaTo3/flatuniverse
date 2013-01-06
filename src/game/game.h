#pragma once

#include "core/core.h"
#include "core/object.h"
#include "worldgridmgr.h"
#include "lua.h"
#include <vector>

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

  //lua-таймеры
  struct GameLuaTimer
  {
    bool active;
    float time;
    std::string funcName;
  };
  std::vector< GameLuaTimer > luaTimers;

  //lua-слушатели клавиатуры
  typedef std::deque< std::string > luaKeyboardListenersList;
  luaKeyboardListenersList
      luaKeyboardListeners,
      luaMouseKeyListeners;

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
  static Dword  LUA_SetTimer          ( float time, const std::string &funcName );
  static void   LUA_CreateObject      ( const std::string &name, const std::string &parentName, const Vec3 &pos, const Vec2 &size, const std::string &textureName, const Vec4 &color );
  static void   LUA_ListenKeyboard    ( const std::string &funcName );
  static void   LUA_ListenMouseKey    ( const std::string &funcName );
  static void   LUA_GameExit          ();
  static Vec2   LUA_GetMousePos       ();
  static Vec2   LUA_GetCameraPos      ();
  static Size   LUA_GetWindowSize     ();
  static void   LUA_ObjectAddTrigger  ( const std::string &triggerName, const std::string &funcName );
  static void   LUA_ObjectTrigger_Handler( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );

  void Update();
  void UpdateLuaTimers();

  static void   KeyboardProc          ( Dword keyId, bool isPressed );
  static void   MouseKeyProc          ( Dword keyId, bool isPressed );
  static void   OnRemoveTrigger       ( ObjectTrigger *trigger );

  Vec3 GetGridPosUnderCamera( float scale = 1.0f );
  Vec3 GetGridPosUnderCursor();

  //debug/test
};
