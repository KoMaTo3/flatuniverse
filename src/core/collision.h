#pragma once

#include "klib.h"



class Collision
{
private:
  Vec3  *position;    //указывает на координаты объекта, дабы не дублировать код и ускорить расчеты
  Vec3  velocity;     //скорость
  Vec3  acceleration; //ускорение
  Vec3  size;         //ширина и высота объекта
  bool  isStatic;     //статика или динамика (статика не подвержена внешним воздействиям)
  float mass;         //масса объекта
  Vec3  force;        //некая "сила", смещающая объект помимо своих скорости/ускорения

  struct
  {
    Vec3  leftTop;
    Vec3  rightBottom;
    float radius2;    //квадрат радиуса объекта
  } _rect;            //рассчитанный контур объекта (квадрат)


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
