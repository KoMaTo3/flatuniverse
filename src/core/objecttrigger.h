#pragma once

#include "klib.h"
#include <deque>

class Collision;

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

public:
  ObjectTrigger( Vec3 *setPosition );
  virtual ~ObjectTrigger();

  void  Update                  ();
  void  CheckCollision          ( Collision *collision );
  bool  IsObjectAlreadyTriggered( Collision *collision );

  ObjectTrigger*  SetSize( const Vec3 &setSize );
};
