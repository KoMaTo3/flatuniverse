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

  //lua-таймеры
  struct GameLuaTimer
  {
    bool active;
    float time;
    std::string funcName;
  };
  std::vector< GameLuaTimer > luaTimers;

  //lua-слушатели клавиатуры и мыши
  typedef std::deque< std::string > luaKeyboardListenersList;
  luaKeyboardListenersList
      luaKeyboardListeners,
      luaMouseKeyListeners,
      luaMouseMoveListeners;

  //триггеры
  struct GameObjectTrigger
  {
    std::string   funcName;
    ObjectTrigger *trigger;
  };
  typedef std::deque< GameObjectTrigger > GameObjectTriggerList;
  GameObjectTriggerList objectTriggers;

  //gui-слушатели
  struct GuiTrigger
  {
    std::string   funcName;
    Object        *object;
  };
  typedef std::deque< GuiTrigger > GuiTriggerList;
  GuiTriggerList guiTriggers;

  typedef std::map< g2Controller*, stdDequeString > dropDownListsType;
  dropDownListsType dropDownLists;
  /*
  struct {
    stdDequeString  tileSize;
    stdDequeString  layers;
  } dropDownLists;
  */

public:
  Game();
  ~Game();
  static bool   LUA_RemoveObject      ( const std::string &name );
  static Vec2   LUA_GetObjectPos      ( const std::string &name );
  static void   LUA_SetObjectPos      ( const std::string &name, const Vec2 &pos );
  static Dword  LUA_SetTimer          ( float time, const std::string &funcName );
  static void   LUA_CreateObject      ( const std::string &name, const Vec3 &pos );
  static void   LUA_ListenKeyboard    ( const std::string &funcName );
  static void   LUA_ListenMouseKey    ( const std::string &funcName );
  static void   LUA_ListenMouseMove   ( const std::string &funcName );
  static void   LUA_GameExit          ();
  static Vec2   LUA_GetMousePos       ();
  static Vec2   LUA_GetCameraPos      ();
  static Size   LUA_GetWindowSize     ();
  static void   LUA_ObjectAddTrigger  ( const std::string &triggerName, const std::string &funcName );
  static void   LUA_GuiAddTrigger     ( const std::string &objectName, const std::string &funcName );
  static void   LUA_ObjectTrigger_Handler( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );
  static void   LUA_GuiTrigger_Handler( Object *guiObject );
  static void   LUA_SetCamera         ( const std::string &name );
  static std::string  LUA_GetCamera   ();
  static void   LUA_ClearScene        ();
  static std::string LUA_GuiGetText   ( const std::string &guiName );
  static void   LUA_GuiSetText        ( const std::string &guiName, const std::string &text );
  static void   LUA_ObjectEnableRenderable  ( const std::string &objectName, const std::string &texture, const Vec2 &size, const Vec4 &color );
  static void   LUA_ObjectDisableRenderable ( const std::string &objectName );
  static void   LUA_ObjectEnableCollision   ( const std::string &objectName, bool isStatic, const Vec3 &size, const Vec3 &velocity, const Vec3 &acceleration );
  static void   LUA_ObjectDisableCollision  ( const std::string &objectName );
  static void   LUA_ObjectEnableTrigger     ( const std::string &objectName, const Vec3 &size );
  static void   LUA_ObjectDisableTrigger    ( const std::string &objectName );
  static bool   LUA_GuiGetChecked     ( const std::string &guiName );
  static bool   LUA_GetCollisionStatic( const std::string &name );
  static void   LUA_SetCollisionStatic( const std::string &name, bool isStatic );
  static void   LUA_DebugRender       ( int flags );
  static std::string LUA_GetObjectByPoint   ( int type, const Vec2 &point, const std::string &afterObject );
  static void   LUA_SetGuiVisibility        ( int show );
  static void   LUA_SelectObject            ( const std::string &name );
  static std::string LUA_GetSelectedObject  ();
  static void   LUA_GuiAttr           ( const std::string &guiName, const std::string &parameter, Variable &value, bool isSet );
  static void   LUA_LoadScript        ( const std::string &fileName );

  void Update();
  void UpdateLuaTimers();

  static void   KeyboardProc          ( Dword keyId, bool isPressed );
  static void   MouseKeyProc          ( Dword keyId, bool isPressed );
  static void   MouseMoveProc         ( const Vec2 &pos );
  static void   OnRemoveTrigger       ( ObjectTrigger *trigger );
  static void   GuiProc               ( Object *obj );

  Vec3 GetGridPosUnderCamera( float scale = 1.0f );
  Vec3 GetGridPosUnderCursor();

  //debug/test
};
