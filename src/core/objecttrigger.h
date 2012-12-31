#pragma once

#include "klib.h"
#include <deque>

class Collision;

class ObjectTrigger
{
private:
  Vec3  *position;  //������ �� ���������� Object
  Vec3  offset;     //�������� ������ �������� ������������ position
  Vec3  size;       //�������� ��������
  struct
  {
    Vec3 leftTop;
    Vec3 rightBottom;
  } rect;

  typedef std::deque< Collision* > collisionInTriggerList;
  collisionInTriggerList  collisionsInTrigger; //������ ��������, ����������� � ��������

public:
  ObjectTrigger( Vec3 *setPosition );
  virtual ~ObjectTrigger();

  void  Update                  ();
  void  CheckCollision          ( Collision *collision );
  bool  IsObjectAlreadyTriggered( Collision *collision );

  ObjectTrigger*  SetSize( const Vec3 &setSize );
};
