#pragma once

#include "klib.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include <deque>
#include <vector>
#include "tools.h"

#define COLLISION_FRICTION_FORCE   ( 0.95f )

enum CollisionElementType {
  COLLISION_ELEMENT_TYPE_UNDEFINED,
  COLLISION_ELEMENT_TYPE_SQUARE,    //выталкивает только самого себя
  COLLISION_ELEMENT_TYPE_CIRCLE,    //выталкивает себя и квадраты
  COLLISION_ELEMENT_TYPE_POLYGON,   //выталкивает себя, круги и квадраты
};

struct CollisionRect {
  Vec3  leftTop;
  Vec3  rightBottom;
  float radius2;    //квадрат радиуса объекта
};

//"болванка" для различных видов коллизий
class CollisionElement {
  friend class CollisionElementSquare;
  friend class CollisionElementCircle;
  friend class CollisionElementPolygon;
public:
  CollisionElement( CollisionElementType setType, Vec3 *setPos, CollisionRect *setRect );
  virtual ~CollisionElement(){}
  inline        CollisionElementType GetType(){ return this->type; }
  virtual void  Update() = 0;
  virtual void  __Dump() = 0;
  virtual bool  TestIntersect( CollisionElement &object, Vec3 *outSolver ) = NULL;
  virtual void  _Render( const Vec3 &offset ) = 0;

protected:
  Vec3 *position;
  CollisionRect *_rect; //рассчитанный контур объекта (квадрат)
  CollisionElementType  type; //тип коллизии: квадрат, полигон, круг...


  DISALLOW_COPY_AND_ASSIGN( CollisionElement );
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) = 0;
};

//квадрат. всегда в одном положении
class CollisionElementSquare: public CollisionElement {
public:
  CollisionElementSquare( Vec3 *setPos, CollisionRect *setRect );
  void Update ();
  void SetSize( const Vec3 &setSize );
  void __Dump ();
  bool TestIntersect( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver );
  virtual void  _Render( const Vec3 &offset );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  Vec3 size;

  DISALLOW_COPY_AND_ASSIGN( CollisionElementSquare );
};

//круг
class CollisionElementCircle: public CollisionElement {
public:
  CollisionElementCircle( Vec3 *setPos, CollisionRect *setRect );
  void Update ();
  void SetDiameter( float setDiameter );
  inline
  float GetDiameter() { return this->diameter; }
  void __Dump     ();
  bool TestIntersect( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithCircle( CollisionElement &object, Vec3 *outSolver );
  virtual void _Render( const Vec3 &offset );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  float diameter;

  DISALLOW_COPY_AND_ASSIGN( CollisionElementCircle );
};

//выпуклый полигон
class CollisionElementPolygon: public CollisionElement {
public:
  typedef Vec2  Point;
  typedef std::vector< Point > PointList;
  typedef std::vector< Vec2 > AxisList;

  CollisionElementPolygon( Vec3 *setPos, CollisionRect *setRect );
  void Update ();
  void SetPointList( const PointList &setPoints );
  void __Dump     ();
  bool TestIntersect            ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithSquare  ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithCircle  ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithPolygon ( CollisionElement &object, Vec3 *outSolver );
  virtual void  _Render( const Vec3 &offset );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  PointList pointsSource, //положение точек относительно центра
            pointsResult; //рассчитанное положение точек
  AxisList  axes;         //разделительные оси без учета поворота объекта

  DISALLOW_COPY_AND_ASSIGN  ( CollisionElementPolygon );
  bool  _IsAxisExists       ( const Vec2 &axis );
};






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

  CollisionRect _rect;//рассчитанный контур объекта (квадрат)

  CollisionElement *collisionElement;

  struct CollisionResolver
  {
    Vec3 power;
    Vec3 resolveVector;
    bool useAllAxices;
  };
  typedef std::deque< CollisionResolver > CollisionResolverList;
  CollisionResolverList resolver;

  DISALLOW_COPY_AND_ASSIGN( Collision );

public:
  Collision( Vec3* objectPosition );
  virtual ~Collision();

  Collision*  SetAcceleration ( const Vec3& newAcceleration );
  Collision*  SetVelocity     ( const Vec3& newVelocity );
  Collision*  SetPosition     ( const Vec3& newPosition );
  Collision*  SetPositionBy   ( const Vec3& deltaPosition );
  //Collision*  SetSize         ( const Vec3& newSize );
  Collision*  SetIsStatic     ( bool newIsStatic );
  Collision*  SetMass         ( float newMass );
  Collision*  SetForce        ( const Vec3& newForce );
  Collision*  InitSquare      ( const Vec3& newSize );
  Collision*  InitCircle      ( float setDiameter );
  Collision*  InitPolygon     ( const CollisionElementPolygon::PointList &points );

  inline
  float       GetRadius2      () const { return this->_rect.radius2; }
  const Vec3& GetPosition     () const;
  inline
  const Vec3& GetVelocity     () const { return this->velocity; }
  inline
  const Vec3& GetAcceleration () const { return this->acceleration; }
  inline
  const Vec3& GetSize         () const { return this->size; }
  inline
  float       GetMass         () const { return this->mass; }
  inline
  const Vec3& GetForce        () const { return this->force; }
  inline
  bool        IsStatic        () const { return this->isStatic; }
  bool        TestIntersect   ( Collision& item );
  bool        TestIntersect   ( const Vec3 &leftTopPoint, const Vec3 &rightBottomPoint );
  bool        TestInPoint     ( const Vec2 &pos );

  bool        Update          ( float dt );
  void        ResolveCollision();

  void        SaveToBuffer    ( MemoryWriter &writer );
  void        LoadFromBuffer  ( MemoryReader &reader );

  void        Render          ( float phase, const Vec3 &offset, bool isActive );

  void        __Dump();
};
