#pragma once

#include "klib.h"
#include "tools.h"
#include <vector>
#include "file.h"
#include "lightmap.h"



class MemoryWriter;


enum CollisionElementType {
  COLLISION_ELEMENT_TYPE_UNDEFINED,
  COLLISION_ELEMENT_TYPE_SQUARE,    //����������� ������ ������ ����
  COLLISION_ELEMENT_TYPE_CIRCLE,    //����������� ���� � ��������
  COLLISION_ELEMENT_TYPE_POLYGON,   //����������� ����, ����� � ��������
};



struct CollisionRect {
  Vec3  leftTop;
  Vec3  rightBottom;
  float radius2;    //������� ������� �������
};



//"��������" ��� ��������� ����� ��������
class CollisionElement: public LightMap::ILightBlock {
  friend class CollisionElementSquare;
  friend class CollisionElementCircle;
  friend class CollisionElementPolygon;
public:
  CollisionElement( CollisionElementType setType, Vec3 *setPos, CollisionRect *setRect );
  virtual ~CollisionElement();
  inline        CollisionElementType GetType(){ return this->type; }
  virtual void  Update  () = 0;
  virtual void  __Dump  () = 0;
  virtual bool  TestIntersect( CollisionElement &object, Vec3 *outSolver ) = NULL;
  virtual void  _Render ( const Vec3 &offset ) = 0;
  virtual void  SaveToBuffer  ( MemoryWriter &writer ) = NULL;

//ILightBlock
  virtual const Vec2& GetPosition() const;          //center
  virtual const Vec2& GetSize() const;              //AABB
  virtual inline const Vec2& GetHalfSize() const;   //AABB/2
  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) = 0;
  virtual void SetPosition( const Vec2& setPosition );

protected:
  Vec3 *position;
  CollisionRect *_rect; //������������ ������ ������� (�������)
  CollisionElementType  type; //��� ��������: �������, �������, ����...
  Vec2                //���������, ����������� ��� �������� � LightMap
    _lastPosition,
    _lastSize,
    _lastHalfSize;
  static const float epsilon;


  DISALLOW_COPY_AND_ASSIGN( CollisionElement );
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) = 0;
};




//AABB-�������. ������ � ����� ���������
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

//ILightBlock
  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  Vec3 size;

  DISALLOW_COPY_AND_ASSIGN( CollisionElementSquare );
};



//����
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

//ILightBlock
  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  float diameter;

  DISALLOW_COPY_AND_ASSIGN( CollisionElementCircle );
};



//�������� �������
class CollisionElementPolygon: public CollisionElement {
public:
  typedef Vec2  Point;
  typedef std::vector< Point > PointList;
  typedef std::vector< Vec2 > AxisList;

  CollisionElementPolygon( Vec3 *setPos, CollisionRect *setRect );
  void Update ();
  void SetPointList( const PointList &setPoints );
  void GetPointList( PointList &points );
  void __Dump     ();
  bool TestIntersect            ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithSquare  ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithCircle  ( CollisionElement &object, Vec3 *outSolver );
  bool TestIntersectWithPolygon ( CollisionElement &object, Vec3 *outSolver );
  virtual void  _Render( const Vec3 &offset );
  virtual void  SaveToBuffer    ( MemoryWriter &writer );

//ILightBlock
  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache );

protected:
  virtual void _ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max );

private:
  PointList pointsSource, //��������� ����� ������������ ������
            pointsResult; //������������ ��������� �����
  AxisList  axes;         //�������������� ��� ��� ����� �������� �������

  DISALLOW_COPY_AND_ASSIGN  ( CollisionElementPolygon );
  bool  _IsAxisExists       ( const Vec2 &axis );
};
