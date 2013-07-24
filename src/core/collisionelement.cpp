#include "collisionelement.h"
#include "file.h"
#include "memorywriter.h"
#include "memoryreader.h"

#ifndef COLISION_NO_OPENGL
#include "../gl/gl.h"
#endif


const float COLLISION_POLYGON_AXIS_E = 0.01f;

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
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::Update => size[%3.1f; %3.1f]", this->size.x, this->size.y );
  this->_rect->leftTop = Vec3( this->position->x - halfSize.x, this->position->y - halfSize.y, 0.0f );
  this->_rect->rightBottom = Vec3( this->position->x + halfSize.x, this->position->y + halfSize.y, 0.0f );
  this->_rect->radius2 = this->size.x * this->size.x + this->size.y * this->size.y;
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
      break;
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

  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementSquare::_ProjectObjectToAxis" );
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





//
//  CollisionElementCircle
//

CollisionElementCircle::CollisionElementCircle( Vec3 *setPos, CollisionRect *setRect )
:CollisionElement( COLLISION_ELEMENT_TYPE_CIRCLE, setPos, setRect )
{
}//constructor



/*
=============
  SetDiameter
=============
*/
void CollisionElementCircle::SetDiameter( float setDiameter ) {
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::SetDiameter => diameter[%3.1f]", setDiameter );
  this->diameter = setDiameter;
}//SetDiameter


/*
=============
  Update
=============
*/
void CollisionElementCircle::Update() {
  float halfSize = this->diameter * 0.5f;
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::Update => diameter[%3.1f]", this->diameter );
  this->_rect->leftTop = Vec3( this->position->x - halfSize, this->position->y - halfSize, 0.0f );
  this->_rect->rightBottom = Vec3( this->position->x + halfSize, this->position->y + halfSize, 0.0f );
  this->_rect->radius2 = halfSize * halfSize * 4.0f;
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
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersect => first square test: false" );
    return false;
  }
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersect => first square test: true" );
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersect => diameters: %3.1f : %3.1f", this->diameter, ( ( CollisionElementCircle* ) &object )->diameter );

  switch( object.GetType() ) {
    case COLLISION_ELEMENT_TYPE_SQUARE: {
      return this->TestIntersectWithSquare( object, outSolver );
      break;
    }
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      return this->TestIntersectWithCircle( object, outSolver );
      break;
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
  float radius = this->diameter * 0.5f;
  bool thisPosXInSquare = this->position->x >= object._rect->leftTop.x && this->position->x <= object._rect->rightBottom.x;
  bool thisPosYInSquare = this->position->y >= object._rect->leftTop.y && this->position->y <= object._rect->rightBottom.y;
  if( thisPosXInSquare || thisPosYInSquare ) {  //круг выталкивается либо вертикально, либо горизонтально
    if( outSolver ) {
      float dy = this->position->y - object.position->y;
      float xDivY = ( fabs( dy ) < 0.01f ? 10.0f : fabs( ( this->position->x - object.position->x ) / dy ) );
      __log.PrintInfo( Filelevel_DEBUG, ". xDivY[%3.1f] dy[%3.1f]", xDivY, dy );
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
    __log.PrintInfo( Filelevel_DEBUG, ". linear: solve[%3.1f; %3.1f]", ( outSolver ? outSolver->x : 0.0f ), ( outSolver ? outSolver->y : 0.0f ) );
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
      __log.PrintInfo( Filelevel_DEBUG, ". no collision" );
      return false;
    } else {
      if( outSolver ) {
        dir.Normalize();
        dir *= distance;
        *outSolver = dir;
      }
    __log.PrintInfo( Filelevel_DEBUG, ". diagonal: solve[%3.1f; %3.1f]", dir.x, dir.y );
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
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersectWithCircle => %3.1f >= %3.1f", minDistance, distance2 );
  if( minDistance < distance2 ) {
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersectWithCircle => no intersect" );
    return false;
  }

  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersectWithCircle => intersected, solvnig" );
  if( outSolver ) {
    float currentDistance = Vec3( *obj->position - *this->position ).Length();
    float minDistance = ( this->diameter + obj->diameter ) * 0.5f;
    __log.PrintInfo( Filelevel_DEBUG, ". currentDistance[%3.1f]", currentDistance );
    __log.PrintInfo( Filelevel_DEBUG, ". minDistance[%3.1f]", minDistance );
    distance.Normalize();
    *outSolver = -distance * ( minDistance - currentDistance + 1.0f );
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::TestIntersectWithCircle => solvation[%3.1f; %3.1f]", outSolver->x, outSolver->y );
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

  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementCircle::_ProjectObjectToAxis" );
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

  Point *point;
  Vec2 radiusVector( Vec2Null );
  Vec2 edge0, edge1, edge2;
  float sign;
  num = 0;
  iterEnd = this->pointsResult.end();
  for( iter = this->pointsResult.begin(); iter != iterEnd; ++iter, ++num ) {
    point = &( *iter );
    radiusVector.Set( max( fabs( point->x ), radiusVector.x ), max( fabs( point->y ), radiusVector.y ) );
    if( num > 1 ) {
      //нормальзовать не нужно т.к. нужно не значение, а только знак
      edge0 = this->pointsResult[ num - 1 ] - this->pointsResult[ num - 2 ];
      edge1 = this->pointsResult[ num ] - this->pointsResult[ num - 2 ];
      edge1.Rotate90CW();
      sign = edge0.Dot( edge1 );
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
      __log.PrintInfo( Filelevel_DEBUG, ". axis[%3.1f; %3.1f]", axis.x, axis.y );
    }
  }//foreach pointsSource

  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::SetPointList => raduis2[%3.1f]", this->_rect->radius2 );
}//SetPointList


/*
=============
  Update
=============
*/
void CollisionElementPolygon::Update() {
  PointList::iterator iter, iterEnd = this->pointsSource.end();
  int count = this->pointsSource.size();
  //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::Update => points[%d] pointsResult[%d]", count, this->pointsResult.size() );
  if( count > 2 ) {
    Vec2 min, max, radius;
    Point *point;
    int num = 0;
    Vec2 pos2D( this->position->x, this->position->y );
    for( iter = this->pointsSource.begin(); iter != iterEnd; ++iter, ++num ) {
      this->pointsResult[ num ] = pos2D + *iter;
      point = &this->pointsResult[ num ];
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
      //__log.PrintInfo( Filelevel_DEBUG, ". point[%3.1f; %3.1f]", this->pointsResult[ num ].x, this->pointsResult[ num ].y );
    }
    this->pointsResult[ this->pointsResult.size() - 1 ] = this->pointsResult[ 0 ];
    this->_rect->leftTop.Set( min.x, min.y, 0.0f );
    this->_rect->rightBottom.Set( max.x, max.y, 0.0f );
    //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::Update => rect[%3.1f; %3.1f]-[%3.1f; %3.1f]", this->_rect->leftTop.x, this->_rect->leftTop.y, this->_rect->rightBottom.x, this->_rect->rightBottom.y );
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
      break;
    }
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      //__log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with circle" );
      return this->TestIntersectWithCircle( object, outSolver );
      break;
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

    __log.PrintInfo( Filelevel_DEBUG, ". projectionObject[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 0 ], projectMax[ 0 ], squareAxis[ axisNum ].x, squareAxis[ axisNum ].y, curIntersectPower );
    __log.PrintInfo( Filelevel_DEBUG, ". projectionThis[%3.1f; %3.1f] to axis[%3.1f; %3.1f] curIntersectPower[%3.1f]", projectMin[ 1 ], projectMax[ 1 ], squareAxis[ axisNum ].x, squareAxis[ axisNum ].y, curIntersectPower );

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
  PointList::const_iterator iterPoint = this->pointsResult.begin(), iterPointEnd = this->pointsResult.end();
  int pointsNum = this->pointsResult.size() - 1;
  float minDistance = this->_rect->radius2 + object._rect->radius2,
    curDistance;
  minDistance *= minDistance;
  Vec2 axis( Vec2Null );
  for( int num = 0; num < pointsNum; ++num, ++iterPoint ) {
    curDistance = ( *iterPoint - Vec2( object.position->x, object.position->y ) ).LengthSqr();
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
      intersectResultInitialized = true;
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
  if( !this->axes.size() ) {
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
  float dot;
  PointList::const_iterator iter, iterEnd = this->pointsResult.end();
  for( iter = this->pointsResult.begin(); iter != iterEnd; ++iter ) {
    dot = axis.Dot( *iter );
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

  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::_ProjectObjectToAxis" );
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
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::_Render => point[%3.1f; %3.1f]", iter->x, iter->y );
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
