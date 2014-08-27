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
const std::string GAME_OBJECT_HANDLER_ONUPDATECORE    = "OnUpdateCore";
const std::string GAME_OBJECT_HANDLER_ONFRAME         = "OnFrame";
const std::string GAME_OBJECT_HANDLER_ONFRAMECORE     = "OnFrameCore";
const std::string GAME_OBJECT_HANDLER_ONDESTROY       = "OnDestroy";
const std::string GAME_OBJECT_HANDLER_ONKEYPRESS      = "OnKeyPress";
const std::string GAME_OBJECT_HANDLER_ONKEYRELEASE    = "OnKeyRelease";
const std::string GAME_OBJECT_HANDLER_ONMOUSEMOVE     = "OnMouseMove";
const std::string GAME_OBJECT_HANDLER_ONMOUSEKEYPRESS = "OnMouseKeyPress";
const std::string GAME_OBJECT_HANDLER_ONMOUSEKEYRELEASE = "OnMouseKeyRelease";
const std::string GAME_OBJECT_HANDLER_ONTRIGGER       = "OnTrigger";

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
    Object *object;

    GameLuaTimer(){}
    GameLuaTimer( const GameLuaTimer& from )
    :active( from.active ), time( from.time ), funcName( from.funcName ), dontPause( from.dontPause ), id( from.id ), luaFunctionId( from.luaFunctionId ), object( from.object )
    { }
    GameLuaTimer& operator=( const GameLuaTimer& from ) {
      this->active = from.active;
      this->dontPause = from.dontPause;
      this->funcName = from.funcName;
      this->id = from.id;
      this->time = from.time;
      this->luaFunctionId = from.luaFunctionId;
      this->object = from.object;
      return *this;
    }
    GameLuaTimer( int setId, const std::string& setFuncName, const int setLuaFunctionId, Object *setObject )
    :id( setId ), funcName( setFuncName ), luaFunctionId( setLuaFunctionId ), object( setObject )
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
    luaObjectOnUpdateCore,
    luaObjectOnFrame,
    luaObjectOnFrameCore,
    luaObjectOnKeyPress,
    luaObjectOnKeyRelease,
    luaObjectOnMouseMove,
    luaObjectOnMouseKeyPress,
    luaObjectOnMouseKeyRelease,
    luaObjectOnCollision,
    luaObjectOnTrigger;

public:
  Game();
  ~Game();
  //static bool   LUA_RemoveObject      ( const std::string &name );

  static void   LuaObject_OnSetScript ( Object *object );
  static void   LuaObject_OnTrigger   ( ObjectTrigger* trigger, Collision* collision, bool isInTrigger );

  void Update();
  void UpdateLuaTimers();
  void SetActive( bool setActive );
  void ClearLuaTimers();
  void StringToObjectList( const std::string &str, ObjectList &list ) const;
  std::string ObjectListToString( const ObjectList &list ) const;
  void OnRemoveObject( const std::string& objectName );
  int IsListeningCollision( Object *object );

  static void   KeyboardProc          ( Dword keyId, bool isPressed );
  static void   MouseKeyProc          ( Dword keyId, bool isPressed );
  static void   MouseMoveProc         ( const Vec2 &pos );
  static void   OnRemoveTrigger       ( ObjectTrigger *trigger );
  static void   CollisionProc         ( Collision *a, Collision *b, Byte flags, const Vec3 &velocity );
  static void   TriggerProc           ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );

  static void   ObjectOnLoad          ( Object* obj );
  static void   ObjectOnUnload        ( Object* obj, bool isClean );
  static void   ObjectOnDestroy       ( Object* obj );
  static void   ObjectOnIsActive      ( Object* obj, bool isActive );
  void          EraseLuaHandler       ( const int luaReferenceId, const std::string &functionName, LuaFunctionHandlersList &handlersList );
  static void   OnCollisionMgrUpdate  ();

  //debug/test
};
