#pragma once

extern "C" {

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

}

#include "core/klib.h"
#include "core/variable.h"
#include "core/file.h"
#include <string>

#define DEBU

class Lua;
class Game;

typedef bool  LUAFUNCPROC_RemoveObject      ( const std::string &name );
typedef Vec2  LUAFUNCPROC_GetObjectPos      ( const std::string &name );
typedef void  LUAFUNCPROC_SetObjectPos      ( const std::string &name, const Vec2 &pos );
typedef Dword LUAFUNCPROC_SetTimer          ( float time, const std::string &funcName, bool dontPause, const int luaFunctionId );
typedef void  LUAFUNCPROC_StopTimer         ( Dword id );
typedef void  LUAFUNCPROC_LogWrite          ( const std::string &text );
typedef void  LUAFUNCPROC_CreateObject      ( const std::string &name, const Vec3 &pos, int notInGrid );
typedef void  LUAFUNCPROC_ListenKeyboard    ( const std::string &funcName );
typedef void  LUAFUNCPROC_ListenMouseKey    ( const std::string &funcName );
typedef void  LUAFUNCPROC_ListenMouseMove   ( const std::string &funcName );
typedef void  LUAFUNCPROC_GameExit          ();
typedef Vec2  LUAFUNCPROC_GetMousePos       ();
typedef Vec2  LUAFUNCPROC_GetCameraPos      ();
typedef float LUAFUNCPROC_GetGridSize       ();
typedef Size  LUAFUNCPROC_GetWindowSize     ();
typedef void  LUAFUNCPROC_ObjectAddTrigger  ( const std::string &triggerName, const std::string &funcName );
//typedef void  LUAFUNCPROC_GuiAddTrigger     ( const std::string &objectName, const std::string &funcName );
typedef void  LUAFUNCPROC_SetCamera         ( const std::string &name );
typedef std::string LUAFUNCPROC_GetCamera   ();
typedef void  LUAFUNCPROC_ClearScene        ();
typedef int   LUAFUNCPROC_GetRandomSeed     ();
//typedef std::string LUAFUNCPROC_GuiGetText  ( const std::string &guiName );
//typedef void  LUAFUNCPROC_GuiSetText        ( const std::string &guiName, const std::string &text );
typedef void  LUAFUNCPROC_ObjectEnableRenderable  ( const std::string &objectName, const std::string &texture, const Vec2 &size, const Vec4 &color );
typedef void  LUAFUNCPROC_ObjectDisableRenderable ( const std::string &objectName );
typedef void  LUAFUNCPROC_ObjectEnableCollision   ( const std::string &objectName, bool isStatic, const Vec3 &size, const Vec3 &velocity, const Vec3 &acceleration );
typedef void  LUAFUNCPROC_ObjectDisableCollision  ( const std::string &objectName );
typedef void  LUAFUNCPROC_ObjectEnableTrigger     ( const std::string &objectName, const Vec3 &size );
typedef void  LUAFUNCPROC_ObjectDisableTrigger    ( const std::string &objectName );
typedef void  LUAFUNCPROC_ObjectEnableLightBlockByCollision ( const std::string &objectName );
typedef void  LUAFUNCPROC_ObjectDisableLightBlockByCollision( const std::string &objectName );
//typedef bool  LUAFUNCPROC_GuiGetChecked     ( const std::string &guiName );
typedef bool  LUAFUNCPROC_GetCollisionStatic( const std::string &name );
typedef void  LUAFUNCPROC_SetCollisionStatic( const std::string &name, const bool isStatic );
typedef void  LUAFUNCPROC_DebugRender       ( int flags );
typedef std::string LUAFUNCPROC_GetObjectByPoint  ( int type, const Vec2 &point, const std::string &afterObject );
typedef std::string LUAFUNCPROC_GetObjectByRect   ( int type, const Vec2 &leftTop, const Vec2 &rightBottom );
//typedef void  LUAFUNCPROC_SetGuiVisibility        ( int show );
typedef void  LUAFUNCPROC_SelectObject            ( const std::string &name );
typedef std::string LUAFUNCPROC_GetSelectedObject ();
//typedef void  LUAFUNCPROC_GuiAttr           ( const std::string &guiName, const std::string &parameter, Variable &value, bool isSet );
typedef void  LUAFUNCPROC_LoadScript        ( const std::string &fileName );
typedef void  LUAFUNCPROC_ObjectAttr        ( const std::string &objectName, VariableAttributesList &setAttributes, VariableAttributesList &getAttributes );
typedef void  LUAFUNCPROC_ListenCollision   ( const std::string &objectName, const std::string &funcName );
typedef void  LUAFUNCPROC_ListenTrigger     ( const std::string &objectName, const std::string &funcName );
typedef float LUAFUNCPROC_GetTime           ();
typedef void  LUAFUNCPROC_SetObjectForce    ( const std::string &name, int forceNum, const Vec2 &force );
typedef void  LUAFUNCPROC_RemoveObjectForce ( const std::string &name, int forceNum );
typedef bool  LUAFUNCPROC_ObjectHasTag      ( const std::string &name, const std::string &tag );
typedef void  LUAFUNCPROC_ObjectAddTag      ( const std::string &name, const std::string &tag );
typedef void  LUAFUNCPROC_ObjectRemoveTag   ( const std::string &name, const std::string &tag );
typedef void  LUAFUNCPROC_ObjectSetAnimation( const std::string &actionAfterAnimationComplete, const std::string &animationAfterAnimationComplete, const std::string &objectName, const std::string &templateName, const std::string &animation );
typedef void  LUAFUNCPROC_ObjectStopAnimation ( const std::string &objectName );
typedef void  LUAFUNCPROC_SetPause          ( bool isPause );
typedef void  LUAFUNCPROC_SetLightAmbient   ( const Vec4 &color );
typedef void  LUAFUNCPROC_WorldSave         ( const std::string& fileName );
typedef void  LUAFUNCPROC_WorldLoad         ( const std::string& fileName );

extern LUAFUNCPROC_RemoveObject     *LUAFUNC_RemoveObject;
extern LUAFUNCPROC_GetObjectPos     *LUAFUNC_GetObjectPos;
extern LUAFUNCPROC_SetObjectPos     *LUAFUNC_SetObjectPos;
extern LUAFUNCPROC_SetTimer         *LUAFUNC_SetTimer;
extern LUAFUNCPROC_StopTimer        *LUAFUNC_StopTimer;
extern LUAFUNCPROC_LogWrite         *LUAFUNC_LogWrite;
extern LUAFUNCPROC_CreateObject     *LUAFUNC_CreateObject;
extern LUAFUNCPROC_ListenKeyboard   *LUAFUNC_ListenKeyboard;
extern LUAFUNCPROC_ListenMouseKey   *LUAFUNC_ListenMouseKey;
extern LUAFUNCPROC_ListenMouseMove  *LUAFUNC_ListenMouseMove;
extern LUAFUNCPROC_GameExit         *LUAFUNC_GameExit;
extern LUAFUNCPROC_GetMousePos      *LUAFUNC_GetMousePos;
extern LUAFUNCPROC_GetCameraPos     *LUAFUNC_GetCameraPos;
extern LUAFUNCPROC_GetGridSize      *LUAFUNC_GetGridSize;
extern LUAFUNCPROC_GetWindowSize    *LUAFUNC_GetWindowSize;
extern LUAFUNCPROC_ObjectAddTrigger *LUAFUNC_ObjectAddTrigger;
//extern LUAFUNCPROC_GuiAddTrigger    *LUAFUNC_GuiAddTrigger;
extern LUAFUNCPROC_SetCamera        *LUAFUNC_SetCamera;
extern LUAFUNCPROC_GetCamera        *LUAFUNC_GetCamera;
extern LUAFUNCPROC_ClearScene       *LUAFUNC_ClearScene;
extern LUAFUNCPROC_GetRandomSeed    *LUAFUNC_GetRandomSeed;
//extern LUAFUNCPROC_GuiGetText       *LUAFUNC_GuiGetText;
//extern LUAFUNCPROC_GuiSetText       *LUAFUNC_GuiSetText;
extern LUAFUNCPROC_ObjectEnableRenderable   *LUAFUNC_ObjectEnableRenderable;
extern LUAFUNCPROC_ObjectDisableRenderable  *LUAFUNC_ObjectDisableRenderable;
extern LUAFUNCPROC_ObjectEnableCollision    *LUAFUNC_ObjectEnableCollision;
extern LUAFUNCPROC_ObjectDisableCollision   *LUAFUNC_ObjectDisableCollision;
extern LUAFUNCPROC_ObjectEnableTrigger      *LUAFUNC_ObjectEnableTrigger;
extern LUAFUNCPROC_ObjectDisableTrigger     *LUAFUNC_ObjectDisableTrigger;
extern LUAFUNCPROC_ObjectEnableLightBlockByCollision  *LUAFUNC_ObjectEnableLightBlockByCollision;
extern LUAFUNCPROC_ObjectDisableLightBlockByCollision *LUAFUNC_ObjectDisableLightBlockByCollision;
//extern LUAFUNCPROC_GuiGetChecked            *LUAFUNC_GuiGetChecked;
extern LUAFUNCPROC_GetCollisionStatic       *LUAFUNC_GetCollisionStatic;
extern LUAFUNCPROC_SetCollisionStatic       *LUAFUNC_SetCollisionStatic;
extern LUAFUNCPROC_DebugRender      *LUAFUNC_DebugRender;
extern LUAFUNCPROC_GetObjectByPoint *LUAFUNC_GetObjectByPoint;
extern LUAFUNCPROC_GetObjectByRect  *LUAFUNC_GetObjectByRect;
//extern LUAFUNCPROC_SetGuiVisibility *LUAFUNC_SetGuiVisibility;
extern LUAFUNCPROC_SelectObject     *LUAFUNC_SelectObject;
extern LUAFUNCPROC_GetSelectedObject        *LUAFUNC_GetSelectedObject;
//extern LUAFUNCPROC_GuiAttr          *LUAFUNC_GuiAttr;
extern LUAFUNCPROC_LoadScript       *LUAFUNC_LoadScript;
extern LUAFUNCPROC_ObjectAttr       *LUAFUNC_ObjectAttr;
extern LUAFUNCPROC_ListenCollision  *LUAFUNC_ListenCollision;
extern LUAFUNCPROC_ListenTrigger    *LUAFUNC_ListenTrigger;
extern LUAFUNCPROC_SetObjectForce   *LUAFUNC_SetObjectForce;
extern LUAFUNCPROC_RemoveObjectForce        *LUAFUNC_RemoveObjectForce;
extern LUAFUNCPROC_ObjectHasTag     *LUAFUNC_ObjectHasTag;
extern LUAFUNCPROC_ObjectAddTag     *LUAFUNC_ObjectAddTag;
extern LUAFUNCPROC_ObjectRemoveTag  *LUAFUNC_ObjectRemoveTag;
extern LUAFUNCPROC_ObjectSetAnimation       *LUAFUNC_ObjectSetAnimation;
extern LUAFUNCPROC_ObjectStopAnimation      *LUAFUNC_ObjectStopAnimation;
extern LUAFUNCPROC_SetPause         *LUAFUNC_SetPause;
extern LUAFUNCPROC_SetLightAmbient  *LUAFUNC_SetLightAmbient;
extern LUAFUNCPROC_WorldSave        *LUAFUNC_WorldSave;
extern LUAFUNCPROC_WorldLoad        *LUAFUNC_WorldLoad;


//callbacks
void LUACALLBACK_Timer            ( Lua *lua, Dword id, const std::string &funcName, const int luaFunctionId );
void LUACALLBACK_ListenKeyboard   ( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed );
void LUACALLBACK_ListenMouseKey   ( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed );
void LUACALLBACK_ListenMouseMove  ( Lua *lua, const std::string &funcName, const Vec2 &pos );
void LUACALLBACK_ObjectTrigger    ( Lua *lua, const std::string &funcName, const std::string &triggerName, const std::string &objectName, bool isInTrigger );
//void LUACALLBACK_GuiTrigger       ( Lua *lua, const std::string &funcName, const std::string &objectName );
void LUACALLBACK_ListenCollision  ( Lua *lua, const std::string &funcName, const std::string &objectName, const std::string &targetName, Byte flags, const Vec3 &velocity );
void LUACALLBACK_ListenTrigger    ( Lua *lua, const std::string &funcName, const std::string &objectName, const std::string &targetName, bool isInTrigger );


//очерёдность применения атрибутов при вызове ObjectAttr({...})
enum {
  LUA_OBJECT_ATTRS_PRIORITET_RENDERABLE = 0,
  LUA_OBJECT_ATTRS_PRIORITET_COLLISION,
  LUA_OBJECT_ATTRS_PRIORITET_COLLISION_SIZE,
  LUA_OBJECT_ATTRS_PRIORITET_LIGHTBLOCKBYCOLLISION,
  LUA_OBJECT_ATTRS_PRIORITET_TRIGGER,
  LUA_OBJECT_ATTRS_PRIORITET_LIGHTPOINT,
  LUA_OBJECT_ATTRS_PRIORITET_DEFAULT,
  __LUA_OBJECT_ATTRS_PRIORITET_COUNT
};


class Lua
{
private:
  lua_State *luaState;

public:
  static File log;

  Lua();
  virtual ~Lua();

  bool  Init          ( Game *game );
  void  Destroy       ();
  bool  RunFile       ( const std::string &fileName );
  bool  RunScript     ( const std::string &script );
  bool  CallFunction  ( const std::string &funcName );
  bool  CallTableTableFunction    ( const std::string &table, const std::string &key, const std::string &function );
  int   GetStackParmsCount();
  void  Unref         ( const int referenceId );
  static int  GetColor( lua_State *lua, int stackIndex, FU_OUT Vec4& color );
  bool  GetBoolean    ( const int stackIndex, const bool removeFromStack = false );
  void  Pop           ( const int count );
  void  DumpStack     ();

  void  ShowError     ( const std::string &comment, const std::string &errorName );

  static int ErrorHandler       ( lua_State *lua );

  //Lua-функции
  static int LUA_Alert            ( lua_State *lua );
  static int LUA_ObjectRemove     ( lua_State *lua );
  static int LUA_GetObjectPos     ( lua_State *lua );
  static int LUA_SetObjectPos     ( lua_State *lua );
  static int LUA_SetTimer         ( lua_State *lua );
  static int LUA_StopTimer        ( lua_State *lua );
  static int LUA_LogWrite         ( lua_State *lua );
  static int LUA_CreateObject     ( lua_State *lua );
  static int LUA_ListenKeyboard   ( lua_State *lua );
  static int LUA_ListenMouseKey   ( lua_State *lua );
  static int LUA_ListenMouseMove  ( lua_State *lua );
  static int LUA_GameExit         ( lua_State *lua );
  static int LUA_GetMousePos      ( lua_State *lua );
  static int LUA_GetCameraPos     ( lua_State *lua );
  static int LUA_GetGridSize      ( lua_State *lua );
  static int LUA_GetWindowSize    ( lua_State *lua );
  static int LUA_ObjectAddTrigger ( lua_State *lua );
  //static int LUA_GuiAddTrigger    ( lua_State *lua );
  static int LUA_SetCamera        ( lua_State *lua );
  static int LUA_GetCamera        ( lua_State *lua );
  static int LUA_ClearScene       ( lua_State *lua );
  static int LUA_GetRandomSeed    ( lua_State *lua );
  //static int LUA_GuiGetText       ( lua_State *lua );
  //static int LUA_GuiSetText       ( lua_State *lua );
  static int LUA_ObjectRenderable ( lua_State *lua );
  static int LUA_ObjectCollision  ( lua_State *lua );
  static int LUA_ObjectTrigger    ( lua_State *lua );
  static int LUA_ObjectLightBlockByCollision( lua_State *lua );
  //static int LUA_GuiGetChecked    ( lua_State *lua );
  static int LUA_GetCollisionStatic ( lua_State *lua );
  static int LUA_SetCollisionStatic ( lua_State *lua );
  static int LUA_DebugRender      ( lua_State *lua );
  static int LUA_GetObjectByPoint ( lua_State *lua );
  static int LUA_GetObjectByRect  ( lua_State *lua );
  //static int LUA_SetGuiVisibility ( lua_State *lua );
  static int LUA_SelectObject     ( lua_State *lua );
  static int LUA_GetSelectedObject( lua_State *lua );
  //static int LUA_GuiAttr          ( lua_State *lua );
  static int LUA_LoadScript       ( lua_State *lua );
  static int LUA_ObjectAttr       ( lua_State *lua );
  static int LUA_Render           ( lua_State *lua );
  static int LUA_ListenCollision  ( lua_State *lua );
  static int LUA_ListenTrigger    ( lua_State *lua );
  static int LUA_GetTime          ( lua_State *lua );
  static int LUA_SetObjectForce   ( lua_State *lua );
  static int LUA_RemoveObjectForce( lua_State *lua );
  static int LUA_ObjectHasTag     ( lua_State *lua );
  static int LUA_ObjectAddTag     ( lua_State *lua );
  static int LUA_ObjectRemoveTag  ( lua_State *lua );
  static int LUA_SetClipboard     ( lua_State *lua );
  static int LUA_GetClipboard     ( lua_State *lua );
  static int LUA_ObjectSetAnimation ( lua_State *lua );
  static int LUA_ObjectStopAnimation( lua_State *lua );
  static int LUA_SetPause         ( lua_State *lua );
  static int LUA_SetLightAmbient  ( lua_State *lua );
  static int LUA_WorldSave        ( lua_State *lua );
  static int LUA_WorldLoad        ( lua_State *lua );

  //callbacks
  static void LUACALLBACK_Timer           ( Lua *lua, Dword id, const std::string &funcName, const int luaFunctionId );
  static void LUACALLBACK_ListenKeyboard  ( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed );
  static void LUACALLBACK_ListenMouseKey  ( Lua *lua, const std::string &funcName, Dword keyId, bool isPressed );
  static void LUACALLBACK_ListenMouseMove ( Lua *lua, const std::string &funcName, const Vec2 &pos );
  static void LUACALLBACK_ObjectTrigger   ( Lua *lua, const std::string &funcName, const std::string &triggerName, const std::string &objectName, bool isInTrigger );
  //static void LUACALLBACK_GuiTrigger      ( Lua *lua, const std::string &funcName, const std::string &objectName );
  static void LUACALLBACK_ListenCollision ( Lua *lua, const std::string &funcName, const std::string &objectName, const std::string &targetName, Byte flags, const Vec3 &velocity );
  static void LUACALLBACK_ListenTrigger   ( Lua *lua, const std::string &funcName, const std::string &objectName, const std::string &targetName, bool isInTrigger );

  static VariableAttributesListByPrioritet attrsListByPrioritet;

private:
  void        LoadLibs();
};
