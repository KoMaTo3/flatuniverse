#pragma once

#include "klib.h"
#include <deque>

class Collision;
class ObjectTrigger;

typedef void ( *ObjectTriggerHandler )        ( ObjectTrigger *trigger, Collision *collision, bool isInTrigger );
typedef void ( *ObjectTriggerOnRemoveHandler )( ObjectTrigger *trigger );
extern ObjectTriggerOnRemoveHandler __ObjectTriggerOnRemoveGlobalHandler;

class MemoryWriter;
class MemoryReader;

class ObjectTrigger
{
private:
  Vec3  *position;  //������ �� ���������� Object
  Vec3  offset;     //�������� ������ �������� ������������ position
  //Vec3  size;       //�������� ��������
  Vec3  triggerPosition;  //������������ ������� ��������: *position + offset
  Collision *triggerRect; //������, ����������� �������
  /*
  struct
  {
    Vec3 leftTop;
    Vec3 rightBottom;
  } rect;
  */

  typedef std::deque< Collision* > collisionInTriggerList;
  collisionInTriggerList  collisionsInTrigger; //������ ��������, ����������� � ��������

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
  const Vec3&     GetSize       ( );
  inline
  const Vec3&     GetPosition   ( ){ return *this->position; }
  bool            TestInPoint   ( const Vec2 &pos );

  void  AddOnRemoveHandler      ( ObjectTriggerOnRemoveHandler handler );

  void  SaveToBuffer            ( MemoryWriter &writer );
  void  LoadFromBuffer          ( MemoryReader &reader );

private:
  void  _TouchHandlers( Collision *collision, bool isInTrigger );
};
