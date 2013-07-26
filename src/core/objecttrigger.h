#pragma once

#include "klib.h"
#include <deque>
#include "collision.h"

class Collision;
class ObjectTrigger;

typedef void ( *ObjectTriggerHandler )        ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );
typedef void ( *ObjectTriggerOnRemoveHandler )( ObjectTrigger *trigger );
extern ObjectTriggerOnRemoveHandler __ObjectTriggerOnRemoveGlobalHandler;

class MemoryWriter;
class MemoryReader;



class ObjectTrigger;

//lua-слушатели триггеров
class luaTriggerListenerStruct {
public:
  ObjectTrigger *object;
  std::string funcName;

  luaTriggerListenerStruct( const luaTriggerListenerStruct& setFrom )
    :object( setFrom.object ), funcName( setFrom.funcName ) {
  }

  luaTriggerListenerStruct( ObjectTrigger *setObject, const std::string &setFuncName )
    :object( setObject ), funcName( setFuncName ) {
  }

  luaTriggerListenerStruct& operator=( const luaTriggerListenerStruct& setFrom ) {
    this->funcName  = setFrom.funcName;
    this->object    = setFrom.object;
    return *this;
  }

private:
  luaTriggerListenerStruct(){}
};
typedef std::deque< luaTriggerListenerStruct > luaTriggerListenersList;



class ObjectTrigger
{
public:
  typedef void TriggerHandler( ObjectTrigger* trigger, Collision* collision, bool isInTrigger );
  typedef void TriggerHandlerInit( const std::string &objectName, const std::string &funcName );
  typedef std::deque< TriggerHandler* > TriggerHandlerList;

private:
  Vec3  *position;  //ссылка на координаты Object
  Vec3  offset;     //смещение центра триггера относительно position
  //Vec3  size;       //габариты триггера
  Vec3  triggerPosition;  //рассчитанная позиция триггера: *position + offset
  Collision *triggerRect; //фигура, описывающая триггер
  /*
  struct
  {
    Vec3 leftTop;
    Vec3 rightBottom;
  } rect;
  */

  typedef std::deque< Collision* > collisionInTriggerList;
  collisionInTriggerList  collisionsInTrigger; //список объектов, находящихся в триггере

  typedef std::deque< ObjectTriggerHandler > ObjectTriggerHandlerList;
  ObjectTriggerHandlerList  handlers;

  typedef std::deque< ObjectTriggerOnRemoveHandler > ObjectTriggerOnRemoveHandlerList;
  ObjectTriggerOnRemoveHandlerList  onRemoveHandlers;

  static TriggerHandlerInit *InitTriggerHandler;
  static luaTriggerListenersList *triggerLintenersList;
  static TriggerHandler *defaultTriggerHandler;

public:
  ObjectTrigger( Vec3 *setPosition );
  ~ObjectTrigger();

  void  Update                  ();
  void  CheckCollision          ( Collision *collision );
  bool  IsObjectAlreadyTriggered( Collision *collision );
  void  AddHandler              ( ObjectTriggerHandler handler );
  bool  IsHandlerExists         ( ObjectTriggerHandler handler );

  ObjectTrigger*  SetSize       ( const Vec3 &setSize );
  ObjectTrigger*  SetPolygon    ( const CollisionElementPolygon::PointList &points );
  const Vec3&     GetSize       ( );
  inline
  const Vec3&     GetPosition   ( ){ return *this->position; }
  bool            TestInPoint   ( const Vec2 &pos );
  bool            TestIntersectRect ( const Vec2 &leftTop, const Vec2 &rightBottom );
  void            Redraw        ( float phase, const Vec3 &offset, bool isActive );
  inline CollisionElementType GetType() { return ( this->triggerRect ? this->triggerRect->GetType() : COLLISION_ELEMENT_TYPE_UNDEFINED ); }
  bool            GetPolygon( CollisionElementPolygon::PointList& result );

  void  AddOnRemoveHandler      ( ObjectTriggerOnRemoveHandler handler );

  void  SaveToBuffer            ( MemoryWriter &writer );
  void  LoadFromBuffer          ( MemoryReader &reader, const std::string &thisObjectName, const Dword version );
  static inline void SetInitTriggerHandler( TriggerHandlerInit setHandler ) { ObjectTrigger::InitTriggerHandler = setHandler; }
  static inline void SetTriggerListenerList( luaTriggerListenersList *setList ) { ObjectTrigger::triggerLintenersList = setList; }
  static inline void SetDefaultTriggerHandler( TriggerHandler *setHandler ) { ObjectTrigger::defaultTriggerHandler = setHandler; }

private:
  void  _TouchHandlers( Collision *collision, bool isInTrigger );
};
