#pragma once

#include "klib.h"



class Collision
{
private:
  Vec3  *position;    //��������� �� ���������� �������, ���� �� ����������� ��� � �������� �������
  Vec3  velocity;     //��������
  Vec3  acceleration; //���������
  Vec3  size;         //������ � ������ �������
  bool  isStatic;     //������� ��� �������� (������� �� ���������� ������� ������������)
  float mass;         //����� �������
  Vec3  force;        //����� "����", ��������� ������ ������ ����� ��������/���������

  struct
  {
    Vec3  leftTop;
    Vec3  rightBottom;
    float radius2;    //������� ������� �������
  } _rect;            //������������ ������ ������� (�������)


public:
  Collision( Vec3* objectPosition );
  virtual ~Collision();

  Collision*  SetAcceleration ( const Vec3& newAcceleration );
  Collision*  SetVelocity     ( const Vec3& newVelocity );
  Collision*  SetPosition     ( const Vec3& newPosition );
  Collision*  SetPositionBy   ( const Vec3& deltaPosition );
  Collision*  SetSize         ( const Vec3& newSize );
  Collision*  SetIsStatic     ( bool newIsStatic );
  Collision*  SetMass         ( float newMass );
  Collision*  SetForce        ( const Vec3& newForce );

  inline
  float       GetRadius2      () const { return this->_rect.radius2; }
  const Vec3& GetPosition     () const;
  inline
  bool        IsStatic        () const { return this->isStatic; }
  bool        TestIntersect   ( Collision& item );

  bool        Update          ( float dt );
};
