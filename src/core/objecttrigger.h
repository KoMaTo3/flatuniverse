#pragma once

#include "klib.h"
#include <deque>

class Collision;
class ObjectTrigger;

typedef void ( *ObjectTriggerHandler )        ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );
typedef void ( *ObjectTriggerOnRemoveHandler )( ObjectTrigger *trigger );
extern ObjectTriggerOnRemoveHandler __ObjectTriggerOnRemoveGlobalHandler;

class ObjectTrigger
{
private:
  Vec3  *position;  //ссылка на координаты Object
  Vec3  offset;     //смещение центра триггера относительно position
  Vec3  size;       //габариты триггера
  struct
  {
    Vec3 leftTop;
    Vec3 rightBottom;
  } rect;

  typedef std::deque< Collision* > collisionInTriggerList;
  collisionInTriggerList  collisionsInTrigger; //список объектов, находящихся в триггере

  typedef std::deque< ObjectTriggerHandler > ObjectTriggerHandlerList;
  ObjectTriggerHandlerList  handlers;

  typedef std::deque< ObjectTriggerOnRemoveHandler > ObjectTriggerOnRemoveHandlerList;
  ObjectTriggerOnRemoveHandlerList  onRemoveHandlers;

public:
  ObjectTrigger( Vec3 *setPosition );
  ~ObjectTrigger();

  void  Update                  ();
  void  CheckCollision          ( Collision *collision );
  bool  IsObjectAlreadyTriggered( Collision *collision );
  void  AddHandler              ( ObjectTriggerHandler handler );
  bool  IsHandlerExists         ( ObjectTriggerHandler handler );

  ObjectTrigger*  SetSize       ( const Vec3 &setSize );
  inline
  const Vec3&     GetSize       ( ){ return this->size; }
  inline
  const Vec3&     GetPosition   ( ){ return *this->position; }
  bool            TestInPoint   ( const Vec2 &pos );

  void  AddOnRemoveHandler      ( ObjectTriggerOnRemoveHandler handler );

private:
  void  _TouchHandlers( Collision *collision, bool isInTrigger );
};
