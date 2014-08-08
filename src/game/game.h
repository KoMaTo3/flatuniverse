#pragma once

#include "core/core.h"
#include "core/object.h"
#include "core/animationset.h"
#include "worldgridmgr.h"
#include "lua.h"
#include <vector>
#include <map>
#include <list>

class Lua;

const std::string GAME_OBJECT_HANDLER_ONCOLLISION     = "OnCollision";
const std::string GAME_OBJECT_HANDLER_ONCREATE        = "OnCreate";
const std::string GAME_OBJECT_HANDLER_ONUPDATE        = "OnUpdate";
const std::string GAME_OBJECT_HANDLER_ONFRAME         = "OnFrame";
const std::string GAME_OBJECT_HANDLER_ONDESTROY       = "OnDestroy";
const std::string GAME_OBJECT_HANDLER_ONKEYPRESS      = "OnKeyPress";
const std::string GAME_OBJECT_HANDLER_ONKEYRELEASE    = "OnKeyRelease";
const std::string GAME_OBJECT_HANDLER_ONMOUSEMOVE     = "OnMouseMove";
const std::string GAME_OBJECT_HANDLER_ONMOUSEKEYPRESS = "OnMouseKeyPress";
const std::string GAME_OBJECT_HANDLER_ONMOUSEKEYRELEASE = "OnMouseKeyRelease";

class Game
{
public:
  struct
  {
    bool show;
    Object *obj[ 40 ];
  } gridTooltipUnderMouse;

  Core                *core;
  World::GridManager  *world;
  Lua                 *lua;
  bool                isActive; //true - playing, false - editing

  //lua-таймеры
  struct GameLuaTimer
  {
    bool active;
    float time;
    std::string funcName;
    bool dontPause; //не паузится от Game::SetActive
    Dword id;
    int luaFunctionId;

    GameLuaTimer(){}
    GameLuaTimer( const GameLuaTimer& from )
    :active( from.active ), time( from.time ), funcName( from.funcName ), dontPause( from.dontPause ), id( from.id ), luaFunctionId( from.luaFunctionId )
    { }
    GameLuaTimer& operator=( const GameLuaTimer& from ) {
      this->active = from.active;
      this->dontPause = from.dontPause;
      this->funcName = from.funcName;
      this->id = from.id;
      this->time = from.time;
      this->luaFunctionId = from.luaFunctionId;
      return *this;
    }
    GameLuaTimer( int setId, const std::string& setFuncName, const int setLuaFunctionId )
    :id( setId ), funcName( setFuncName ), luaFunctionId( setLuaFunctionId )
    { }
  };
  std::vector< GameLuaTimer > luaTimers;

  //lua-слушатели клавиатуры и мыши
  typedef std::vector< std::string > luaKeyboardListenersList;
  luaKeyboardListenersList
      luaKeyboardListeners,
      luaMouseKeyListeners,
      luaMouseMoveListeners;

  //lua-слушатели коллизий
  luaCollisionListenersList collisionListeners;
  luaTriggerListenersList luaTriggerListeners;

  //триггеры
  struct GameObjectTrigger
  {
    std::string   funcName;
    ObjectTrigger *trigger;

    GameObjectTrigger(){}
    GameObjectTrigger( GameObjectTrigger& from )
    :funcName( from.funcName ), trigger( from.trigger )
    { }
    GameObjectTrigger& operator=( const GameObjectTrigger& from ) {
      this->funcName = from.funcName;
      this->trigger = from.trigger;
      return *this;
    }
  };
  typedef std::deque< GameObjectTrigger > GameObjectTriggerList;
  GameObjectTriggerList objectTriggers;
  typedef std::hash_map< std::string, Animation::ANIMATION_SET_ACTION > AnimationSetActionByName;
  AnimationSetActionByName animationSetActionByName;

  struct LuaFunctionHandler {
    int referenceId;

    LuaFunctionHandler( int setReferenceId ): referenceId( setReferenceId ) {}
  };
  typedef std::deque< LuaFunctionHandler > LuaFunctionHandlersList;
  LuaFunctionHandlersList
    luaObjectOnUpdate,
    luaObjectOnFrame,
    luaObjectOnKeyPress,
    luaObjectOnKeyRelease,
    luaObjectOnMouseMove,
    luaObjectOnMouseKeyPress,
    luaObjectOnMouseKeyRelease,
    luaObjectOnCollision;

public:
  Game();
  ~Game();
  static bool   LUA_RemoveObject      ( const std::string &name );
  static Vec2   LUA_GetObjectPos      ( const std::string &name );
  static void   LUA_SetObjectPos      ( const std::string &name, const Vec2 &pos );
  static Dword  LUA_SetTimer          ( float time, const std::string &funcName, bool dontPause, const int luaFunctionId );
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
  static void   LUA_ObjectEnableLightBlockByCollision ( const std::string &objectName );
  static void   LUA_ObjectDisableLightBlockByCollision( const std::string &objectName );
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
  static void   LUA_ObjectSetAnimation( const std::string &actionAfterAnimationComplete, const std::string &animationAfterAnimationComplete, const std::string &objectName, const std::string &templateName, const std::string &animation );
  static void   LUA_ObjectStopAnimation ( const std::string &objectName );
  static void   LUA_SetPause          ( bool isPause );
  static void   LUA_SetLightAmbient   ( const Vec4 &color );
  static void   LUA_WorldSave         ( const std::string& fileName );
  static void   LUA_WorldLoad         ( const std::string& fileName );

  static void   LuaObject_OnSetScript ( Object *object );

  void Update();
  void UpdateLuaTimers();
  void SetActive( bool setActive );
  void ClearLuaTimers();
  void StringToObjectList( const std::string &str, ObjectList &list ) const;
  std::string ObjectListToString( const ObjectList &list ) const;
  void OnRemoveObject( const std::string& objectName );

  static void   KeyboardProc          ( Dword keyId, bool isPressed );
  static void   MouseKeyProc          ( Dword keyId, bool isPressed );
  static void   MouseMoveProc         ( const Vec2 &pos );
  static void   OnRemoveTrigger       ( ObjectTrigger *trigger );
  static void   CollisionProc         ( Collision *a, Collision *b, Byte flags, const Vec3 &velocity );
  static void   TriggerProc           ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );

  static void   ObjectOnLoad          ( Object* obj );
  static void   ObjectOnUnload        ( Object* obj );
  void          EraseLuaHandler       ( const int luaReferenceId, const std::string &functionName, LuaFunctionHandlersList &handlersList );

  //debug/test
};
