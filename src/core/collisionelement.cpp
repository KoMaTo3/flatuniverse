#include "collisionelement.h"
#include "file.h"
#include "memorywriter.h"
#include "memoryreader.h"

#ifndef COLISION_NO_OPENGL
#include "../gl/gl.h"
#endif


const float COLLISION_POLYGON_AXIS_E = 0.01f;
const float CollisionElement::epsilon = 0.0f;

/*
//=============
//  CollisionElement
//=============
*/
CollisionElement::CollisionElement( CollisionElementType setType, Vec3 *setPos, CollisionRect *setRect )
:type( setType ), position( setPos ), _rect( setRect )
{
  if( !setPos ) {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElement => position is NULL" );
  }
  if( !setRect ) {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElement => rect is NULL" );
  }
}//constructor



CollisionElement::~CollisionElement() {
}



/*
=============
  GetPosition
=============
*/
const Vec2& CollisionElement::GetPosition() const {
  return this->_lastPosition;
}//GetPosition



/*
=============
  GetSize
=============
*/
const Vec2& CollisionElement::GetSize() const {
  return this->_lastSize;
}//GetSize



/*
=============
  GetHalfSize
=============
*/
const Vec2& CollisionElement::GetHalfSize() const {
  return this->_lastHalfSize;
}//GetHalfSize



/*
=============
  SetPosition
=============
*/
void CollisionElement::SetPosition( const Vec2& setPosition ) {
  this->position->x = setPosition.x;
  this->position->y = setPosition.y;
}//SetPosition











//
//  CollisionElementSquare
//

CollisionElementSquare::CollisionElementSquare( Vec3 *setPos, CollisionRect *setRect )
:CollisionElement( COLLISION_ELEMENT_TYPE_SQUARE, setPos, setRect )
{
}//constructor


/*
=============
  Update
=============
*/
void CollisionElementSquare::Update() {
  Vec3 halfSize = this->size * 0.5f;
  this->_rect->leftTop = Vec3( this->position->x - halfSize.x, this->position->y - halfSize.y, 0.0f );
  this->_rect->rightBottom = Vec3( this->position->x + halfSize.x, this->position->y + halfSize.y, 0.0f );
  this->_rect->radius2 = this->size.x * this->size.x + this->size.y * this->size.y;
  this->_lastPosition.Set( this->position->x, -this->position->y );
  this->_lastSize.Set( this->size.x, this->size.y );
  this->_lastHalfSize = this->_lastSize * 0.5f;
}


/*
=============
  __Dump
=============
*/
void CollisionElementSquare::__Dump() {
  __log.PrintAligned( "CollisionElementSquare: rect[%3.1f; %3.1f]-[%3.1f; %3.1f] radius2[%3.1f]\n", this->_rect->leftTop.x, this->_rect->leftTop.y, this->_rect->rightBottom.x, this->_rect->rightBottom.y, this->_rect->radius2 );
}//__Dump


/*
=============
  SetSize
=============
*/
void CollisionElementSquare::SetSize( const Vec3 &setSize ) {
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::SetSize => size[%3.1f; %3.1f; %3.1f;]", setSize.x, setSize.y, setSize.z );
  this->size = setSize;
}//SetSize



/*
=============
  TestIntersect
=============
*/
bool CollisionElementSquare::TestIntersect( CollisionElement &object, Vec3 *outSolver ) {
  if( this->type < object.GetType() ) { //всегда решение коллизий должен делать более "старший" вид коллизии
    bool res = object.TestIntersect( *this, outSolver );
    *outSolver = -*outSolver;
    return res;
  }

  if( this->_rect->rightBottom.x  <= object._rect->leftTop.x ||
    this->_rect->leftTop.x        >= object._rect->rightBottom.x ||
    this->_rect->rightBottom.y    <= object._rect->leftTop.y ||
    this->_rect->leftTop.y        >= object._rect->rightBottom.y
    )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::TestIntersect => first square test: false" );
    return false;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::TestIntersect => first square test: true" );

  switch( object.GetType() ) {
    case COLLISION_ELEMENT_TYPE_SQUARE: {
      return this->TestIntersectWithSquare( object, outSolver );
    }
  }//type

  return true;
}//TestIntersect



/*
=============
  TestIntersectWithSquare
  square + square
  В эту функцию мы попадаем уже пройдя первоначальную проверку по квадратам, остаётся только вернуть решение
=============
*/
bool CollisionElementSquare::TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver ) {
  if( outSolver ) {
    outSolver->Set(
      Math::Fabs( max( this->_rect->leftTop.x, object._rect->leftTop.x ) - min( this->_rect->rightBottom.x, object._rect->rightBottom.x ) ),
      Math::Fabs( max( this->_rect->leftTop.y, object._rect->leftTop.y ) - min( this->_rect->rightBottom.y, object._rect->rightBottom.y ) ),
      0.0f
    );
    //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::TestIntersectWithSquare => result[ %3.1f; %3.1f ]", outSolver->x, outSolver->y );
  }
  return true;
}//TestIntersectWithSquare



/*
=============
  _ProjectObjectToAxis
=============
*/
void CollisionElementSquare::_ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) {
  if( !min || !max ) {
    return;
  }

  float resultPoints[ 4 ];
  resultPoints[ 0 ] = axis.Dot( Vec2( this->_rect->leftTop.x, this->_rect->leftTop.y ) );
  resultPoints[ 1 ] = axis.Dot( Vec2( this->_rect->rightBottom.x, this->_rect->leftTop.y ) );
  resultPoints[ 2 ] = axis.Dot( Vec2( this->_rect->rightBottom.x, this->_rect->rightBottom.y ) );
  resultPoints[ 3 ] = axis.Dot( Vec2( this->_rect->leftTop.x, this->_rect->rightBottom.y ) );
  *min = *max = resultPoints[ 0 ];
  for( int q = 1; q < 4; ++q ) {
    if( resultPoints[ q ] < *min ) {
      *min = resultPoints[ q ];
    }
    if( resultPoints[ q ] > *max ) {
      *max = resultPoints[ q ];
    }
  }
}//_ProjectObjectToAxis



/*
=============
  _Render
=============
*/
void CollisionElementSquare::_Render( const Vec3 &offset ) {
#ifndef COLISION_NO_OPENGL
  glBegin( GL_QUADS );
  glVertex3f( this->_rect->leftTop.x - offset.x, this->_rect->leftTop.y - offset.y, 0.0f );
  glVertex3f( this->_rect->rightBottom.x - offset.x, this->_rect->leftTop.y - offset.y, 0.0f );
  glVertex3f( this->_rect->rightBottom.x - offset.x, this->_rect->rightBottom.y - offset.y, 0.0f );
  glVertex3f( this->_rect->leftTop.x - offset.x, this->_rect->rightBottom.y - offset.y, 0.0f );
  glEnd();
#endif
}//_Render



/*
=============
  SaveToBuffer
=============
*/
void CollisionElementSquare::SaveToBuffer( MemoryWriter &writer ) {
  writer << this->size;
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::SaveToBuffer => squareSize[%3.1f; %3.1f; %3.1f;]", this->size.x, this->size.y, this->size.z );
}//SaveToBuffer



/*
=============
  FillBuffer
=============
*/
void CollisionElementSquare::FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
  //outer light
  if( lightPosition.x < this->_lastPosition.x - this->_lastHalfSize.x ) { //add right edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x, this->_lastPosition.y - this->_lastHalfSize.y - this->epsilon ), Vec2( this->_lastPosition.x - this->_lastHalfSize.x, this->_lastPosition.y + this->_lastHalfSize.y + this->epsilon ), cache );
  }
  if( lightPosition.x > this->_lastPosition.x + this->_lastHalfSize.x ) { //add left edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x + this->_lastHalfSize.x, this->_lastPosition.y - this->_lastHalfSize.y - this->epsilon ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x, this->_lastPosition.y + this->_lastHalfSize.y + this->epsilon ), cache );
  }
  if( lightPosition.y < this->_lastPosition.y - this->_lastHalfSize.y ) { //add bottom edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x - this->epsilon, this->_lastPosition.y - this->_lastHalfSize.y ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x + this->epsilon, this->_lastPosition.y - this->_lastHalfSize.y ), cache );
  }
  if( lightPosition.y > this->_lastPosition.y + this->_lastHalfSize.y ) { //add top edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x - this->epsilon, this->_lastPosition.y + this->_lastHalfSize.y ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x + this->epsilon, this->_lastPosition.y + this->_lastHalfSize.y ), cache );
  }

  /*
  //inner light
  if( lightPosition.x < this->_lastPosition.x + this->_lastHalfSize.x ) { //add right edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x + this->_lastHalfSize.x, this->_lastPosition.y - this->_lastHalfSize.y - this->epsilon ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x, this->_lastPosition.y + this->_lastHalfSize.y + this->epsilon ), cache );
  }
  if( lightPosition.x > this->_lastPosition.x - this->_lastHalfSize.x ) { //add left edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x, this->_lastPosition.y - this->_lastHalfSize.y - this->epsilon ), Vec2( this->_lastPosition.x - this->_lastHalfSize.x, this->_lastPosition.y + this->_lastHalfSize.y + this->epsilon ), cache );
  }
  if( lightPosition.y < this->_lastPosition.y + this->_lastHalfSize.y ) { //add bottom edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x - this->epsilon, this->_lastPosition.y + this->_lastHalfSize.y ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x + this->epsilon, this->_lastPosition.y + this->_lastHalfSize.y ), cache );
  }
  if( lightPosition.y > this->_lastPosition.y - this->_lastHalfSize.y ) { //add top edge
    this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->_lastPosition.x - this->_lastHalfSize.x - this->epsilon, this->_lastPosition.y - this->_lastHalfSize.y ), Vec2( this->_lastPosition.x + this->_lastHalfSize.x + this->epsilon, this->_lastPosition.y - this->_lastHalfSize.y ), cache );
  }
  */
}//FillBuffer





//
//  CollisionElementCircle
//

CollisionElementCircle::CollisionElementCircle( Vec3 *setPos, CollisionRect *setRect )
  :CollisionElement( COLLISION_ELEMENT_TYPE_CIRCLE, setPos, setRect ), diameter( 0.0f )
{
}//constructor



/*
=============
  SetDiameter
=============
*/
void CollisionElementCircle::SetDiameter( float setDiameter ) {
  this->diameter = setDiameter;
}//SetDiameter


/*
=============
  Update
=============
*/
void CollisionElementCircle::Update() {
  float halfSize = this->diameter * 0.5f;
  this->_rect->leftTop = Vec3( this->position->x - halfSize, this->position->y - halfSize, 0.0f );
  this->_rect->rightBottom = Vec3( this->position->x + halfSize, this->position->y + halfSize, 0.0f );
  this->_rect->radius2 = halfSize * halfSize * 4.0f;
  this->_lastPosition.Set( this->position->x, -this->position->y );
  this->_lastHalfSize.Set( halfSize, halfSize );
  this->_lastSize = this->_lastHalfSize * 2.0f;
}


/*
=============
  __Dump
=============
*/
void CollisionElementCircle::__Dump() {
  __log.PrintAligned( "CollisionElementCircle: rect[%3.1f; %3.1f]-[%3.1f; %3.1f] radius2[%3.1f] diameter[%3.1f]\n", this->_rect->leftTop.x, this->_rect->leftTop.y, this->_rect->rightBottom.x, this->_rect->rightBottom.y, this->_rect->radius2, this->diameter );
}//__Dump



/*
=============
  TestIntersect
=============
*/
bool CollisionElementCircle::TestIntersect( CollisionElement &object, Vec3 *outSolver ) {
  if( this->type < object.GetType() ) { //всегда решение коллизий должен делать более "старший" вид коллизии
    bool res = object.TestIntersect( *this, outSolver );
    *outSolver = -*outSolver;
    return res;
  }

  if( this->_rect->rightBottom.x  <= object._rect->leftTop.x ||
    this->_rect->leftTop.x        >= object._rect->rightBottom.x ||
    this->_rect->rightBottom.y    <= object._rect->leftTop.y ||
    this->_rect->leftTop.y        >= object._rect->rightBottom.y
    )
  {
    return false;
  }

  switch( object.GetType() ) {
    case COLLISION_ELEMENT_TYPE_SQUARE: {
      return this->TestIntersectWithSquare( object, outSolver );
    }
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      return this->TestIntersectWithCircle( object, outSolver );
    }
  }//type

  return true;
}//TestIntersect



/*
=============
  TestIntersectWithSquare
  circle + square
=============
*/
bool CollisionElementCircle::TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver ) {
  bool thisPosXInSquare = this->position->x >= object._rect->leftTop.x && this->position->x <= object._rect->rightBottom.x;
  bool thisPosYInSquare = this->position->y >= object._rect->leftTop.y && this->position->y <= object._rect->rightBottom.y;
  if( thisPosXInSquare || thisPosYInSquare ) {  //круг выталкивается либо вертикально, либо горизонтально
    if( outSolver ) {
      float dy = this->position->y - object.position->y;
      float xDivY = ( fabs( dy ) < 0.01f ? 10.0f : fabs( ( this->position->x - object.position->x ) / dy ) );
      if( xDivY > 1.0f ) {  //горизонтально
        outSolver->Set(
          this->position->x < object.position->x ? object._rect->leftTop.x - this->_rect->rightBottom.x : object._rect->rightBottom.x - this->_rect->leftTop.x,
          0.0f, 0.0f );
      } else {  //вертикально
        outSolver->Set( 0.0f,
          this->position->y < object.position->y ? object._rect->leftTop.y - this->_rect->rightBottom.y : object._rect->rightBottom.y - this->_rect->leftTop.y,
          0.0f );
      }
    }
  } else {  //круг находится по диагонали от квадрата
    Vec3 point( Vec3Null );
    if( this->position->x < object.position->x ) {
      point.x = object._rect->leftTop.x;
    } else {
      point.x = object._rect->rightBottom.x;
    }
    if( this->position->y < object.position->y ) {
      point.y = object._rect->leftTop.y;
    } else {
      point.y = object._rect->rightBottom.y;
    }
    Vec3 dir = *this->position - point;
    float distance = this->diameter * 0.5f - dir.Length();
    if( distance < 0.0f ) {
      return false;
    } else {
      if( outSolver ) {
        dir.Normalize();
        dir *= distance;
        *outSolver = dir;
      }
    }
  }
  /*
  if( outSolver ) {
    outSolver->Set(
      Math::Fabs( max( this->_rect->leftTop.x, object._rect->leftTop.x ) - min( this->_rect->rightBottom.x, object._rect->rightBottom.x ) ),
      Math::Fabs( max( this->_rect->leftTop.y, object._rect->leftTop.y ) - min( this->_rect->rightBottom.y, object._rect->rightBottom.y ) ),
      0.0f
    );
  }
  */
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersectWithSquare => thisType[%d]", this->type );
  return true;
}//TestIntersectWithSquare



/*
=============
  TestIntersectWithCircle
=============
*/
bool CollisionElementCircle::TestIntersectWithCircle( CollisionElement &object, Vec3 *outSolver ) {
  CollisionElementCircle *obj = ( CollisionElementCircle* ) &object;
  float summRadius = ( this->diameter + obj->diameter ) * 0.5f;
  float minDistance = summRadius * summRadius;
  Vec3 distance       = *obj->position - *this->position;
  float distance2     = distance.x * distance.x + distance.y * distance.y;
  if( minDistance < distance2 ) {
    return false;
  }

  if( outSolver ) {
    float currentDistance = Vec3( *obj->position - *this->position ).Length();
    float minDistance = ( this->diameter + obj->diameter ) * 0.5f;
    distance.Normalize();
    *outSolver = -distance * ( minDistance - currentDistance + 1.0f );
  }

  return true;
}//TestIntersectWithCircle



/*
=============
  _ProjectObjectToAxis
=============
*/
void CollisionElementCircle::_ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) {
  if( !min || !max ) {
    return;
  }

  float center = axis.Dot( Vec2( this->position->x, this->position->y ) );
  *min = center - this->diameter * 0.5f;
  *max = center + this->diameter * 0.5f;
}//_ProjectObjectToAxis



/*
=============
  _Render
=============
*/
void CollisionElementCircle::_Render( const Vec3 &offset ) {
#ifndef COLISION_NO_OPENGL
  glBegin( GL_TRIANGLE_FAN );
  for( float a = 0.0f; a < Math::PI * 2.0f; a += 0.5f ) {
    glVertex3f( this->position->x - offset.x + this->diameter * 0.5f * Math::Sin16( a ), this->position->y - offset.y + this->diameter * 0.5f * Math::Cos16( a ), 0.0f );
  }
  glEnd();
#endif
}//_Render



/*
=============
  SaveToBuffer
=============
*/
void CollisionElementCircle::SaveToBuffer( MemoryWriter &writer ) {
  writer << this->diameter;
}//SaveToBuffer



/*
=============
  FillBuffer
=============
*/
void CollisionElementCircle::FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
  //NOTE: !!!!!!
}//FillBuffer







//
// CollisionElementPolygon
//

CollisionElementPolygon::CollisionElementPolygon( Vec3 *setPos, CollisionRect *setRect )
:CollisionElement( COLLISION_ELEMENT_TYPE_POLYGON, setPos, setRect )
{
}//constructor

/*
=============
  SetPointList
  Точки необходимо задавать обходя по часовой стрелке
=============
*/
void CollisionElementPolygon::SetPointList( const PointList &setPoints ) {
  if( setPoints.size() < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::SetPointList => not enough points" );
    return;
  }
  this->pointsSource = setPoints;
  this->pointsResult.clear();
  this->pointsResult.resize( setPoints.size() + 1 );

  //copy src to result
  int num = 0;
  PointList::iterator iter, iterEnd = this->pointsSource.end();
  for( iter = this->pointsSource.begin(); iter != iterEnd; ++iter, ++num ) {
    this->pointsResult[ num ] = *iter;
  }
  this->pointsResult[ this->pointsResult.size() - 1 ] = this->pointsSource[ 0 ];

  Vec2 radiusVector( Vec2Null );
  Vec2 edge0, edge1, edge2;
  num = 0;
  iterEnd = this->pointsResult.end();
  for( iter = this->pointsResult.begin(); iter != iterEnd; ++iter, ++num ) {
    Point *point = &( *iter );
    radiusVector.Set( max( fabs( point->x ), radiusVector.x ), max( fabs( point->y ), radiusVector.y ) );
    if( num > 1 ) {
      //нормальзовать не нужно т.к. нужно не значение, а только знак
      edge0 = this->pointsResult[ num - 1 ] - this->pointsResult[ num - 2 ];
      edge1 = this->pointsResult[ num ] - this->pointsResult[ num - 2 ];
      edge1.Rotate90CW();
      float sign = edge0.Dot( edge1 );
      if( sign > 0 ) {
        __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::SetPointList => points[%d] is not clockwise", num );
        this->pointsResult.clear();
        this->pointsSource.clear();
        return;
      }
    }
  }//for
  this->_rect->radius2 = Square( radiusVector.Length() );

  //формирования списка разделающих осей
  Vec2 axis;
  Vec2 tmpVec2;
  iterEnd = this->pointsResult.end();
  iter = this->pointsResult.begin();
  num = 1;
  for( ++iter; iter != iterEnd; ++iter, ++num ) {
    tmpVec2 = this->pointsResult[ num ] - this->pointsResult[ num - 1 ];
    axis.Set( tmpVec2.x, tmpVec2.y );
    axis.Rotate90CW();
    if( axis.x < 0 ) {
      axis *= -1.0f;
    } else if( axis.y < 0.0f ) {
      axis *= -1.0f;
    }
    axis.Normalize();
    if( !this->_IsAxisExists( axis ) ) {
      this->axes.push_back( axis );
    }
  }//foreach pointsSource
}//SetPointList



/*
=============
  GetPointList
=============
*/
void CollisionElementPolygon::GetPointList( PointList &points ) {
  points = this->pointsSource;
}//GetPointList



/*
=============
  Update
=============
*/
void CollisionElementPolygon::Update() {
  PointList::iterator iter, iterEnd = this->pointsSource.end();
  int count = this->pointsSource.size();
  if( count > 2 ) {
    Vec2 min, max, radius;
    int num = 0;
    Vec2 pos2D( this->position->x, this->position->y );
    for( iter = this->pointsSource.begin(); iter != iterEnd; ++iter, ++num ) {
      this->pointsResult[ num ] = pos2D + *iter;
      Point *point = &this->pointsResult[ num ];
      if( num == 0 ) {
        min.Set( point->x, point->y );
        max = min;
      } else {
        min.Set(
          min.x < point->x ? min.x : point->x,
          min.y < point->y ? min.y : point->y
          );
        max.Set(
          max.x > point->x ? max.x : point->x,
          max.y > point->y ? max.y : point->y
          );
      }
    }
    this->pointsResult[ this->pointsResult.size() - 1 ] = this->pointsResult[ 0 ];
    this->_rect->leftTop.Set( min.x, min.y, 0.0f );
    this->_rect->rightBottom.Set( max.x, max.y, 0.0f );
    this->_lastPosition.Set( this->position->x, -this->position->y );
    this->_lastSize.Set( radius.x, radius.y );
    this->_lastHalfSize = this->_lastSize * 0.5f;
  } else {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::Update => not enough points" );
  }
}//Update



/*
=============
  TestIntersect
=============
*/
bool CollisionElementPolygon::TestIntersect( CollisionElement &object, Vec3 *outSolver ) {
  if( this->type < object.GetType() ) { //всегда решение коллизий должен делать более "старший" вид коллизии
    bool res = object.TestIntersect( *this, outSolver );
    *outSolver = -*outSolver;
    return res;
  }

  if( this->_rect->rightBottom.x  <= object._rect->leftTop.x ||
    this->_rect->leftTop.x        >= object._rect->rightBottom.x ||
    this->_rect->rightBottom.y    <= object._rect->leftTop.y ||
    this->_rect->leftTop.y        >= object._rect->rightBottom.y
    )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => first square test: false" );
    return false;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => first square test: true" );

  switch( object.GetType() ) {
    case COLLISION_ELEMENT_TYPE_SQUARE: {
      //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with square" );
      return this->TestIntersectWithSquare( object, outSolver );
    }
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with circle" );
      return this->TestIntersectWithCircle( object, outSolver );
    }
    case COLLISION_ELEMENT_TYPE_POLYGON: {
      //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with polygon" );
      //return this->TestIntersectWithPolygon( object, outSolver );
      break;
    }
    default: {
      __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::TestIntersect => bad type" );
    }
  }//type

  return true;
}//TestIntersect


/*
=============
  __Dump
=============
*/
void CollisionElementPolygon::__Dump() {
  __log.PrintAligned( "CollisionElementPolygon: rect[%3.1f; %3.1f]-[%3.1f; %3.1f] radius2[%3.1f]\n", this->_rect->leftTop.x, this->_rect->leftTop.y, this->_rect->rightBottom.x, this->_rect->rightBottom.y, this->_rect->radius2 );
}//__Dump



/*
=============
  TestIntersectWithSquare
=============
*/
bool CollisionElementPolygon::TestIntersectWithSquare( CollisionElement &object, Vec3 *outSolver ) {
  if( this->pointsResult.size() < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::TestIntersectWithSquare => not enough points" );
    return false;
  }

  //Проецируем объекты на разделяющие оси: оси полигона + 2 оси квадрата
  AxisList::const_iterator iter, iterEnd = this->axes.end();
  float projectMin[ 2 ], projectMax[ 2 ];
  Vec2 intersectAxis;
  bool isIntersected = true;
  bool intersectResultInitialized = false;
  bool inverseSolver = false;
  float lastIntersectPower = 100000.0f, curIntersectPower;

  //проецируем на оси полигона
  for( iter = this->axes.begin(); iter != iterEnd; ++iter ) {
    object._ProjectObjectToAxis( *iter, &projectMin[ 0 ], &projectMax[ 0 ] );
    this->_ProjectObjectToAxis( *iter, &projectMin[ 1 ], &projectMax[ 1 ] );
    if( projectMin[ 0 ] > projectMax[ 1 ] || projectMax[ 0 ] < projectMin[ 1 ] ) {
      isIntersected = false;
      break;
    }
    curIntersectPower = Math::Fabs( max( projectMin[ 0 ], projectMin[ 1 ] ) - min( projectMax[ 0 ], projectMax[ 1 ] ) );

    if( intersectResultInitialized ) {
      if( curIntersectPower < lastIntersectPower ) {
        lastIntersectPower = curIntersectPower;
        intersectAxis = *iter;
        inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
      }
    } else {
      lastIntersectPower = curIntersectPower;
      intersectAxis = *iter;
      intersectResultInitialized = true;
      inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
    }
  }//foreach axes

  //проецируем на оси квадрата
  Vec2 squareAxis[ 2 ];
  squareAxis[ 0 ].Set( 1.0f, 0.0f );
  squareAxis[ 1 ].Set( 0.0f, 1.0f );
  for( int axisNum = 0; axisNum < 2; ++axisNum ) {
    object._ProjectObjectToAxis( squareAxis[ axisNum ], &projectMin[ 0 ], &projectMax[ 0 ] );
    this->_ProjectObjectToAxis( squareAxis[ axisNum ], &projectMin[ 1 ], &projectMax[ 1 ] );
    if( projectMin[ 0 ] > projectMax[ 1 ] || projectMax[ 0 ] < projectMin[ 1 ] ) {
      isIntersected = false;
      break;
    }
    curIntersectPower = Math::Fabs( max( projectMin[ 0 ], projectMin[ 1 ] ) - min( projectMax[ 0 ], projectMax[ 1 ] ) );

    if( intersectResultInitialized ) {
      if( curIntersectPower < lastIntersectPower ) {
        lastIntersectPower = curIntersectPower;
        intersectAxis = squareAxis[ axisNum ];
        inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
      }
    } else {
      lastIntersectPower = curIntersectPower;
      intersectAxis = squareAxis[ axisNum ];
      intersectResultInitialized = true;
      inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
    }
  }//foreach axes

  if( isIntersected ) {
    if( outSolver ) {
      outSolver->Set( intersectAxis.x * lastIntersectPower, intersectAxis.y * lastIntersectPower, 0.0f );
      if( inverseSolver ) {
        *outSolver *= -1.0f;
      }
    }
  } else {
    return false;
  }

  return true;
}//TestIntersectWithSquare



/*
=============
  TestIntersectWithCircle
  TODO: требует доработки
=============
*/
bool CollisionElementPolygon::TestIntersectWithCircle( CollisionElement &object, Vec3 *outSolver ) {
  if( this->pointsResult.size() < 3 ) {
    __log.PrintInfo( Filelevel_ERROR, "CollisionElementPolygon::TestIntersectWithCircle => not enough points" );
    return false;
  }

  __log.PrintInfo( Filelevel_WARNING, "CollisionElementPolygon::TestIntersectWithCircle => insert algorythm" );

  //Проецируем объекты на разделяющие оси: оси полигона + 2 оси квадрата
  AxisList::const_iterator iter, iterEnd = this->axes.end();
  float projectMin[ 2 ], projectMax[ 2 ];
  Vec2 intersectAxis;
  bool isIntersected = true;
  bool intersectResultInitialized = false;
  bool inverseSolver = false;
  float lastIntersectPower = 100000.0f, curIntersectPower;

  //проецируем на оси полигона
  for( iter = this->axes.begin(); iter != iterEnd; ++iter ) {
    object._ProjectObjectToAxis( *iter, &projectMin[ 0 ], &projectMax[ 0 ] );
    this->_ProjectObjectToAxis( *iter, &projectMin[ 1 ], &projectMax[ 1 ] );
    if( projectMin[ 0 ] > projectMax[ 1 ] || projectMax[ 0 ] < projectMin[ 1 ] ) {
      isIntersected = false;
      break;
    }
    curIntersectPower = Math::Fabs( max( projectMin[ 0 ], projectMin[ 1 ] ) - min( projectMax[ 0 ], projectMax[ 1 ] ) );

    __log.PrintInfo( Filelevel_DEBUG, ". projectionObject[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 0 ], projectMax[ 0 ], iter->x, iter->y, curIntersectPower );
    __log.PrintInfo( Filelevel_DEBUG, ". projectionThis[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 1 ], projectMax[ 1 ], iter->x, iter->y, curIntersectPower );

    if( intersectResultInitialized ) {
      if( curIntersectPower < lastIntersectPower ) {
        lastIntersectPower = curIntersectPower;
        intersectAxis = *iter;
        inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
      }
    } else {
      lastIntersectPower = curIntersectPower;
      intersectAxis = *iter;
      intersectResultInitialized = true;
      inverseSolver = ( projectMax[ 0 ] > projectMax[ 1 ] );
    }
  }//foreach axes

  //ищем ближайшую точку полигона и проецируем объекты на ось точка=>центрКруга
  PointList::const_iterator iterPoint = this->pointsResult.begin();
  int pointsNum = this->pointsResult.size() - 1;
  float minDistance = this->_rect->radius2 + object._rect->radius2;
  minDistance *= minDistance;
  Vec2 axis( Vec2Null );
  for( int num = 0; num < pointsNum; ++num, ++iterPoint ) {
    float curDistance = ( *iterPoint - Vec2( object.position->x, object.position->y ) ).LengthSqr();
    if( curDistance < minDistance ) {
      minDistance = curDistance;
      axis = *iterPoint;
    }
  }//foreach pointNum
  if( axis.x < 0 ) {
    axis *= -1.0f;
  } else if( axis.y < 0.0f ) {
    axis *= -1.0f;
  }
  axis.Normalize();
  object._ProjectObjectToAxis( axis, &projectMin[ 0 ], &projectMax[ 0 ] );
  this->_ProjectObjectToAxis( axis, &projectMin[ 1 ], &projectMax[ 1 ] );
  if( projectMin[ 0 ] > projectMax[ 1 ] || projectMax[ 0 ] < projectMin[ 1 ] ) {
    isIntersected = false;
  } else {
    curIntersectPower = Math::Fabs( max( projectMin[ 0 ], projectMin[ 1 ] ) - min( projectMax[ 0 ], projectMax[ 1 ] ) );

    __log.PrintInfo( Filelevel_DEBUG, ". projectionObject[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 0 ], projectMax[ 0 ], axis.x, axis.y, curIntersectPower );
    __log.PrintInfo( Filelevel_DEBUG, ". projectionThis[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 1 ], projectMax[ 1 ], axis.x, axis.y, curIntersectPower );

    if( intersectResultInitialized ) {
      if( curIntersectPower < lastIntersectPower ) {
        lastIntersectPower = curIntersectPower;
        intersectAxis = axis;
      }
    } else {
      lastIntersectPower = curIntersectPower;
      intersectAxis = axis;
      //intersectResultInitialized = true;
    }
  }
  //

  if( isIntersected ) {
    __log.PrintInfo( Filelevel_DEBUG, ". lastIntersectPower[%3.1f] intersectAxis[%3.1f; %3.1f] inverseSolver[%d]", lastIntersectPower, intersectAxis.x, intersectAxis.y, inverseSolver );
    if( outSolver ) {
      outSolver->Set( intersectAxis.x * lastIntersectPower, intersectAxis.y * lastIntersectPower, 0.0f );
      if( inverseSolver ) {
        *outSolver *= -1.0f;
      }
      __log.PrintInfo( Filelevel_DEBUG, ". resultsolveIntersect[%3.1f; %3.1f]", outSolver->x, outSolver->y );
    }
    __log.PrintInfo( Filelevel_DEBUG, ". result[%d] power[%3.1f] axis[%3.1f; %3.1f]", isIntersected, lastIntersectPower, intersectAxis.x, intersectAxis.y );
  } else {
    __log.PrintInfo( Filelevel_DEBUG, ". no intersect" );
    return false;
  }

  return true;
}//TestIntersectWithCircle



/*
=============
  _IsAxisExists
=============
*/
bool CollisionElementPolygon::_IsAxisExists( const Vec2 &axis ) {
  if( this->axes.empty() ) {
    return false;
  }

  AxisList::const_iterator iter, iterEnd = this->axes.end();
  for( iter = this->axes.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter - axis ).LengthFast() < COLLISION_POLYGON_AXIS_E ) {
      return true;
    }
  }

  return false;
}//_IsAxisExists



/*
=============
  _ProjectObjectToAxis
=============
*/
void CollisionElementPolygon::_ProjectObjectToAxis( const Vec2 &axis, FU_OUT float *min, FU_OUT float *max ) {
  if( !min || !max ) {
    return;
  }

  bool isInitialized = false;
  PointList::const_iterator iter, iterEnd = this->pointsResult.end();
  for( iter = this->pointsResult.begin(); iter != iterEnd; ++iter ) {
    float dot = axis.Dot( *iter );
    if( isInitialized ) {
      if( dot < *min ) {
        *min = dot;
      } else if( dot > *max ) {
        *max = dot;
      }
    } else {
      *min = *max = dot;
      isInitialized = true;
    }
  }//foreach pointsResult
}//_ProjectObjectToAxis



/*
=============
  _Render
=============
*/
void CollisionElementPolygon::_Render( const Vec3 &offset ) {
#ifndef COLISION_NO_OPENGL
  glBegin( GL_TRIANGLE_FAN );
  PointList::const_iterator iter, iterEnd = this->pointsResult.end();
  for( iter = this->pointsResult.begin(); iter != iterEnd; ++iter ) {
    glVertex3f( iter->x - offset.x, iter->y - offset.y, 0.0f );
  }
  glEnd();
#endif
}//_Render



/*
=============
  SaveToBuffer
=============
*/
void CollisionElementPolygon::SaveToBuffer( MemoryWriter &writer ) {
  int count = this->pointsSource.size();
  writer << count;

  PointList::const_iterator iter, iterEnd = this->pointsSource.end();
  for( iter = this->pointsSource.begin(); iter != iterEnd; ++iter ) {
    writer << *iter;
  }
}//SaveToBuffer



/*
=============
  FillBuffer
=============
*/
void CollisionElementPolygon::FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
  if( this->pointsResult.size() < 2 ) {
    return;
  }
  PointList::const_iterator
    iter = this->pointsResult.begin() + 1,
    iterEnd = this->pointsResult.end(),
    iterPrev = this->pointsResult.begin();
  for(; iter != iterEnd; ++iter ) {
    Vec2 pointCurrent( iter->x, -iter->y );
    Vec2 pointPrev( iterPrev->x, -iterPrev->y );
    Vec2 vec( pointCurrent - pointPrev );
    vec.Rotate90CCW();
    if( vec.Dot( lightPosition - pointCurrent ) <= 0.0f ) {
      //this->AddEdgeToBuffer( lightPosition, buffer, pointCurrent, pointPrev, cache );
      Vec2
        vec0( pointCurrent - pointPrev ),
        vec1( pointPrev - pointCurrent )
        ;
      vec0.NormalizeFast();
      vec1.NormalizeFast();
      vec.NormalizeFast();
      //vec *= ( 1.0f + this->epsilon );  //углубляем ребро на 1+epsilon
      vec0 *= ( 1.0f + this->epsilon ); //удлинняем ребро на 1+epsilon
      vec1 *= ( 1.0f + this->epsilon );
      Vec2 resVec0 = pointCurrent + vec0 + vec;
      Vec2 resVec1 = pointPrev + vec1 + vec;
      this->AddEdgeToBuffer( lightPosition, buffer, resVec0, resVec1, cache );
    }
    iterPrev = iter;
  }
}//FillBuffer
