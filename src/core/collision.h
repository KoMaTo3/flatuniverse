#pragma once

#include "klib.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include "collisionelement.h"
#include <deque>
#include <vector>
#include "tools.h"

#define COLLISION_FRICTION_FORCE   ( 0.95f )



class Collision;

//lua-��������� ��������
class luaCollisionListenerStruct {
public:
  Collision *object;
  std::string funcName;

  luaCollisionListenerStruct( const luaCollisionListenerStruct& setFrom )
    :object( setFrom.object ), funcName( setFrom.funcName ) {
  }

  luaCollisionListenerStruct( Collision *setObject, const std::string &setFuncName )
    :object( setObject ), funcName( setFuncName ) {
  }

  luaCollisionListenerStruct& operator=( const luaCollisionListenerStruct& setFrom ) {
    this->funcName  = setFrom.funcName;
    this->object    = setFrom.object;
    return *this;
  }

private:
  luaCollisionListenerStruct(){}
};
typedef std::deque< luaCollisionListenerStruct > luaCollisionListenersList;




class Collision
{
public:
  typedef void CollisionHandler( Collision* a, Collision* b );
  typedef void CollisionHandlerInit( const std::string &funcName, const std::string &objectName );
  typedef std::deque< CollisionHandler* > CollisionHandlerList;

private:
  Vec3  *position;    //��������� �� ���������� �������, ���� �� ����������� ��� � �������� �������
  Vec3  velocity;     //��������
  Vec3  acceleration; //���������
  Vec3  size;         //������ � ������ �������
  bool  isStatic;     //������� ��� �������� (������� �� ���������� ������� ������������)
  float mass;         //����� �������
  Vec3  force;        //����� "����", ��������� ������ ������ ����� ��������/���������

  CollisionRect _rect;//������������ ������ ������� (�������)

  CollisionElement *collisionElement;
  CollisionHandlerList *handlers;
  static CollisionHandlerInit *InitCollisionHandler;
  static luaCollisionListenersList *collisionLintenersList;
  static CollisionHandler *defaultCollisionHandler;

  struct CollisionResolver
  {
    Vec3        power;
    Vec3        resolveVector;
    bool        useAllAxices;
    Collision   *target;
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
  void        AddHandler      ( CollisionHandler *handler );

  void        SaveToBuffer    ( MemoryWriter &writer );
  void        LoadFromBuffer  ( MemoryReader &reader, const std::string &thisObjectName, const Dword version );
  static inline void SetInitCollisionHandler( CollisionHandlerInit setHandler ) { Collision::InitCollisionHandler = setHandler; }
  static inline void SetCollisionListenerList( luaCollisionListenersList *setList ) { Collision::collisionLintenersList = setList; }
  static inline void SetDefaultCollisionHandler( CollisionHandler *setHandler ) { Collision::defaultCollisionHandler = setHandler; }

  void        Render          ( float phase, const Vec3 &offset, bool isActive );

  void        __Dump();
};
