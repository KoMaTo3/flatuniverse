#pragma once

#include "klib.h"
#include "tools.h"
#include <vector>
#include "file.h"



class MemoryWriter;


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
  virtual void  Update  () = 0;
  virtual void  __Dump  () = 0;
  virtual bool  TestIntersect( CollisionElement &object, Vec3 *outSolver ) = NULL;
  virtual void  _Render ( const Vec3 &offset ) = 0;
  virtual void  SaveToBuffer  ( MemoryWriter &writer ) = NULL;

protected:
  Vec3 *position;
  CollisionRect *_rect; //рассчитанный контур объекта (квадрат)
  CollisionElementType  type; //тип коллизии: квадрат, полигон, круг...


  DISALLOW_COPY_AND_ASSIGN( CollisionElement );
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) = 0;
};




//AABB-квадрат. всегда в одном положении
class CollisionElementSquare: public CollisionElement {
public:
  CollisionElementSquare( Vec3 *setPos, CollisionRect *setRect );
  void Update ();
  void SetSize( const Vec3 &setSize );
  void __Dump ();
  bool TestIntersect( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver );
  virtual void  _Render( const Vec3 &offset );
  virtual void  SaveToBuffer  ( MemoryWriter &writer );
  inline Vec3& GetSizeModifier() {
    return this->size;
  }

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
  virtual void  SaveToBuffer  ( MemoryWriter &writer );

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
  virtual void  SaveToBuffer    ( MemoryWriter &writer );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  PointList pointsSource, //положение точек относительно центра
            pointsResult; //рассчитанное положение точек
  AxisList  axes;         //разделительные оси без учета поворота объекта

  DISALLOW_COPY_AND_ASSIGN  ( CollisionElementPolygon );
  bool  _IsAxisExists       ( const Vec2 &axis );
};
