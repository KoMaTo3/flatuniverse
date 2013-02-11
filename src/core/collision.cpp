#include "collision.h"
#include "file.h"
#include "gl/gl.h"

//разделяющие оси могу иметь погрешности, поэтому игнорируем некоторые почти-параллельные оси
const float COLLISION_POLYGON_AXIS_E = 0.01f;


Vec3 __nullCollisionPosition( 0.0f, 0.0f, 0.0f );


Collision::Collision( Vec3* objectPosition )
:position( NULL ), velocity( 0.0f, 0.0f, 0.0f ), acceleration( 0.0f, 0.0f, 0.0f ), isStatic( true ), mass( 1.0f ), force( 0.0f, 0.0f, 0.0f ), collisionElement( NULL )
{
  this->position = objectPosition;
}//constructor



Collision::~Collision()
{
  DEF_DELETE( collisionElement );
}//destructor



/*
=============
  Update
  Обновление состояния объекта:
    перерасчет скоростей, положения, дополнительных значений
=============
*/
bool Collision::Update( float dt )
{
  if( !this->position )
    return false;

  //__log.PrintInfo( Filelevel_DEBUG, "Collision::Update => dt[%3.3f] pos[%3.3f;%3.3f] vel[%3.3f;%3.3f] acc[%3.3f;%3.3f]", dt, this->position->x, this->position->y, this->velocity.x, this->velocity.y, this->acceleration.x, this->acceleration.y );
  this->velocity += this->acceleration * dt * COLLISION_FRICTION_FORCE;
  *this->position += ( this->velocity + this->force ) * dt;

  //пересчет характеристик
  /*
  Vec3 halfSize           = this->size * 0.5f;
  this->_rect.leftTop     = *this->position - halfSize;
  this->_rect.rightBottom = *this->position + halfSize;
  //this->_rect.radius2     = halfSize.x * halfSize.x + halfSize.y * halfSize.y;
  this->_rect.radius2     = this->size.x * this->size.x + this->size.y * this->size.y;
  */
  if( this->collisionElement ) {
    this->collisionElement->Update();
  }

  this->resolver.clear();

  return true;
}//Update



/*
=============
  SetAcceleration
=============
*/
Collision* Collision::SetAcceleration( const Vec3& newAcceleration )
{
  this->acceleration = newAcceleration;
  return this;
}//SetAcceleration



/*
=============
  SetVelocity
=============
*/
Collision* Collision::SetVelocity( const Vec3& newVelocity )
{
  this->velocity = newVelocity;
  return this;
}//SetVelocity



/*
=============
  SetPosition
=============
*/
Collision* Collision::SetPosition( const Vec3& newPosition )
{
  if( !this->position )
  {
    __log.PrintInfo( Filelevel_ERROR, "Collision::SetPosition => NULL pointer" );
    return this;
  }
  *this->position = newPosition;
  return this;
}//SetPosition



/*
=============
  SetPositionBy
=============
*/
Collision* Collision::SetPositionBy( const Vec3& deltaPosition )
{
  if( !this->position )
  {
    __log.PrintInfo( Filelevel_ERROR, "Collision::SetPositionBy => NULL pointer" );
    return this;
  }
  *this->position += deltaPosition;
  return this;
}//SetPositionBy



/*
=============
  InitSquare
=============
*/
Collision* Collision::InitSquare( const Vec3& newSize )
{
  if( !this->collisionElement ) {
    CollisionElementSquare *item = new CollisionElementSquare( this->position, &this->_rect );
    this->collisionElement = item;
    item->SetSize( newSize );
  } else if( this->collisionElement->GetType() != COLLISION_ELEMENT_TYPE_SQUARE ) {
    __log.PrintInfo( Filelevel_ERROR, "Collision::InitSquare => this already initialized by type %d", this->collisionElement->GetType() );
    return this;
  }
  this->size = newSize;
  return this;
}//InitSquare



/*
=============
  InitCircle
=============
*/
Collision* Collision::InitCircle( float setDiameter ) {
  if( this->collisionElement ) {
    __log.PrintInfo( Filelevel_ERROR, "Collision::InitCircle => already initialized" );
    return this;
  }
  CollisionElementCircle *item = new CollisionElementCircle( this->position, &this->_rect );
  this->collisionElement = item;
  item->SetDiameter( setDiameter );

  return this;
}//InitCircle



/*
=============
  InitPolygon
=============
*/
Collision* Collision::InitPolygon( const CollisionElementPolygon::PointList &points ) {
  if( this->collisionElement ) {
    __log.PrintInfo( Filelevel_ERROR, "Collision::InitPolygon => already initialized" );
    return this;
  }
  CollisionElementPolygon *item = new CollisionElementPolygon( this->position, &this->_rect );
  this->collisionElement = item;
  item->SetPointList( points );

  return this;
}//InitPolygon




/*
=============
  GetPosition
=============
*/
const Vec3& Collision::GetPosition () const
{
  if( !this->position )
    return __nullCollisionPosition;
  return *this->position;
}//GetPosition



/*
=============
  SetIsStatic
=============
*/
Collision* Collision::SetIsStatic( bool newIsStatic )
{
  this->isStatic = newIsStatic;
  return this;
}//SetIsStatic




/*
=============
  SetMass
=============
*/
Collision* Collision::SetMass( float newMass )
{
  this->mass = newMass;
  return this;
}//SetMass




/*
=============
  SetForce
=============
*/
Collision* Collision::SetForce( const Vec3& newForce )
{
  this->force = newForce;
  return this;
}//SetForce




/*
=============
  TestIntersect
=============
*/
bool Collision::TestIntersect( Collision& item )
{
  //__log.PrintInfo( Filelevel_DEBUG, "(%3.3f;%3.3f):(%3.3f;%3.3f) - (%3.3f;%3.3f):(%3.3f;%3.3f)", this->_rect.leftTop.x, this->_rect.leftTop.y, this->_rect.rightBottom.x, this->_rect.rightBottom.y
  //  , item._rect.leftTop.x, item._rect.leftTop.y, item._rect.rightBottom.x, item._rect.rightBottom.y
  //  );
  /*
  if( this->_rect.rightBottom.x <= item._rect.leftTop.x ||
    this->_rect.leftTop.x       >= item._rect.rightBottom.x ||
    this->_rect.rightBottom.y   <= item._rect.leftTop.y ||
    this->_rect.leftTop.y       >= item._rect.rightBottom.y
    )
  {
    __log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => first square test: false" );
    return false;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => first square test: true" );

  Vec3 intersectPower = Vec3(
      Math::Fabs( max( this->_rect.leftTop.x, item._rect.leftTop.x ) - min( this->_rect.rightBottom.x, item._rect.rightBottom.x ) ),
      Math::Fabs( max( this->_rect.leftTop.y, item._rect.leftTop.y ) - min( this->_rect.rightBottom.y, item._rect.rightBottom.y ) ),
      0.0f
    );
    */
  Vec3 intersectPower;
  if( !this->collisionElement || !item.collisionElement ) {
    __log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => some collision is NULL" );
    return false;
  }
  if( !this->collisionElement->TestIntersect( *item.collisionElement, &intersectPower ) ) {
    __log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => no intersect" );
    return false;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => intersect, solving" );
  Vec3 halfIntersectPower = intersectPower * 0.5f;

  /*
  if( intersectPower.y < intersectPower.x )
    intersectPower.x  = 0.0f;
  else
    intersectPower.y  = 0.0f;
    */

  Collision *item0, *item1;
  bool collisionSolved = false;
  CollisionResolver itemResolver0, itemResolver1;

  if( this->IsStatic() )  //static + dynamic
  {
    item0 = &item;
    item1 = this;
  }
  else if( item.IsStatic() ) //dymanic + static
  {
    item0 = this;
    item1 = &item;
  }
  else  //dynamic + dynamic
  {
    item0 = this;
    item1 = &item;
    collisionSolved = true;

    itemResolver0.power = itemResolver1.power = halfIntersectPower;
    //item0
    itemResolver0.resolveVector = itemResolver1.resolveVector = Vec3Null;
    if( item0->_rect.leftTop.x < item1->_rect.leftTop.x ) //двигаем item0 - влево, item1 - вправо
    {
      itemResolver0.resolveVector.x = -halfIntersectPower.x;
      itemResolver1.resolveVector.x = halfIntersectPower.x;
    }
    else
    {
      itemResolver0.resolveVector.x = halfIntersectPower.x;
      itemResolver1.resolveVector.x = -halfIntersectPower.x;
    }
    if( item0->_rect.leftTop.y < item1->_rect.leftTop.y ) //двигаем item0 - вверх, item1 - вниз
    {
      itemResolver0.resolveVector.y = -halfIntersectPower.y;
      itemResolver1.resolveVector.y = halfIntersectPower.y;
    }
    else
    {
      itemResolver0.resolveVector.y = halfIntersectPower.y;
      itemResolver1.resolveVector.y = -halfIntersectPower.y;
    }
    item0->resolver.push_back( itemResolver0 );
    item1->resolver.push_back( itemResolver1 );

    /*
    if( intersectPower.x )
    {
      if( item0->_rect.leftTop.x < item1->_rect.leftTop.x ) //двигаем item0 - влево, item1 - вправо
      {
        item0->SetPositionBy( -intersectPower * 0.5f );
        item1->SetPositionBy( intersectPower * 0.5f );
      }
      else
      {
        item0->SetPositionBy( intersectPower * 0.5f );
        item1->SetPositionBy( -intersectPower * 0.5f );
      }
      float velocity = ( item0->velocity.x + item1->velocity.x ) * 0.5f;
      item0->velocity.x = velocity;
      item1->velocity.x = velocity;
    }
    else
    {
      if( item0->_rect.leftTop.y < item1->_rect.leftTop.y ) //двигаем item0 - вверх, item1 - вниз
      {
        item0->SetPositionBy( -intersectPower );
        item1->SetPositionBy( intersectPower );
        if( item0->velocity.y > item1->velocity.y )
          item0->velocity.y = item1->velocity.y;
        if( item1->velocity.y < item0->velocity.y )
          item1->velocity.y = item0->velocity.y;
      }
      else
      {
        item0->SetPositionBy( intersectPower );
        item1->SetPositionBy( -intersectPower );
        if( item0->velocity.y < item1->velocity.y )
          item0->velocity.y = item1->velocity.y;
        if( item1->velocity.y > item0->velocity.y )
          item1->velocity.y = item0->velocity.y;
      }
      if(  item0->velocity.y <= 0.0f && item1->velocity.y <= 0.0f
        || item0->velocity.y >= 0.0f && item1->velocity.y >= 0.0f
        )
      {
        float velocity = ( item0->velocity.y + item1->velocity.y ) * 0.5f;
        item0->velocity.y = velocity;
        item1->velocity.y = velocity;
      }
    }
    //пересчитываем позицию объектов
    item0->Update( 0.0f );
    item1->Update( 0.0f );
    */
  }

  if( !collisionSolved )  //dynamic[item0] + static[item1]
  {
    itemResolver0.power = intersectPower;
    itemResolver0.resolveVector = Vec3Null;
    itemResolver0.useAllAxices = false;
    if( item0->collisionElement->GetType() == COLLISION_ELEMENT_TYPE_SQUARE && item1->collisionElement->GetType() == COLLISION_ELEMENT_TYPE_SQUARE ) {
      if( item0->_rect.leftTop.x < item1->_rect.leftTop.x ) //двигаем влево
        itemResolver0.resolveVector.x = -intersectPower.x;
      else
        itemResolver0.resolveVector.x = intersectPower.x;
      if( item0->_rect.leftTop.y < item1->_rect.leftTop.y ) //поднимаем
        itemResolver0.resolveVector.y = -intersectPower.y;
      else
        itemResolver0.resolveVector.y = intersectPower.y;
      item0->resolver.push_back( itemResolver0 );
    } else {
      itemResolver0.power = intersectPower;
      itemResolver0.resolveVector = intersectPower;
      itemResolver0.useAllAxices = true;
      item0->resolver.push_back( itemResolver0 );
    }
    /*
    if( intersectPower.x )
    {
      if( item0->_rect.leftTop.x < item1->_rect.leftTop.x ) //двигаем влево
      {
        item0->SetPositionBy( -intersectPower );
        if( item0->velocity.x > item1->velocity.x )
          item0->velocity.x = item1->velocity.x;
      }
      else  //двигаем вправо
      {
        item0->SetPositionBy( intersectPower );
        if( item0->velocity.x < item1->velocity.x )
          item0->velocity.x = item1->velocity.x;
      }
    }
    else
    {
      if( item0->_rect.leftTop.y < item1->_rect.leftTop.y ) //поднимаем
      {
        item0->SetPositionBy( -intersectPower );
        if( item0->velocity.y > item1->velocity.y )
          item0->velocity.y = item1->velocity.y;
      }
      else  //опускаем
      {
        item0->SetPositionBy( intersectPower );
        if( item0->velocity.y < item1->velocity.y )
          item0->velocity.y = item1->velocity.y;
      }
    }
    //__log.PrintInfo( Filelevel_DEBUG, "%d", __LINE__ );
    //пересчитываем позицию объекта
    item0->Update( 0.0f );
    */
  }

  return true;
}//TestIntersect



/*
=============
  TestIntersect
  Тест пересечения коллизии с прямоугольной областью
=============
*/
bool Collision::TestIntersect( const Vec3 &leftTopPoint, const Vec3 &rightBottomPoint )
{
  return
    !( this->_rect.rightBottom.x <= leftTopPoint.x ||
     this->_rect.leftTop.x       >= rightBottomPoint.x ||
     this->_rect.rightBottom.y   <= leftTopPoint.y ||
     this->_rect.leftTop.y       >= rightBottomPoint.y
    );
}//TestIntersect




/*
=============
  TestInPoint
=============
*/
bool Collision::TestInPoint( const Vec2 &pos )
{
  return
    !( this->_rect.rightBottom.x < pos.x ||
     this->_rect.leftTop.x       > pos.x ||
     this->_rect.rightBottom.y   < pos.y ||
     this->_rect.leftTop.y       > pos.y
    );
}//TestInPoint





/*
=============
  ResolveCollision
=============
*/
void Collision::ResolveCollision()
{
  //__log.PrintInfo( Filelevel_DEBUG, "Collision::ResolveCollision => %d", this->resolver.size() );
  if( !this->resolver.size() )
    return;

  CollisionResolver *result = &( *this->resolver.begin() );
  float resultSquare = result->power.x * result->power.y, square;
  CollisionResolverList::iterator iter = this->resolver.begin(), iterEnd = this->resolver.end();
  for( ++iter; iter != iterEnd; ++iter )
  {
    square = iter->power.x * iter->power.y;
    if( square > resultSquare )
    {
      resultSquare = square;
      result = &( *iter );
    }
  }
  //__log.PrintInfo( Filelevel_DEBUG, ". power[%3.3f; %3.3f] vector[%3.3f; %3.3f]", result->power.x, result->power.y, result->resolveVector.x, result->resolveVector.y );

  Vec3 resolved( Vec3Null );
  if( result->useAllAxices ) {
    //Vec3 coeffVec = result->resolveVector;
    //coeffVec.Normalize();
    //float coeffSign = Sign( coeffVec.y );
    //__log.PrintInfo( Filelevel_DEBUG, "coeffVec: %3.1f:%3.1f", Sign( this->velocity.y ), Sign( coeffVec.y ) );
    //if( Sign( this->velocity.y ) != coeffSign && coeffSign != 0.0f ) {
    //  this->velocity.y = 0.0f;
    //}
  } else {
    if( result->power.x < result->power.y ) //смещаем по x
      result->resolveVector.y = 0.0f;
    else  //смещаем по y
    {
      result->resolveVector.x = 0.0f;
      if( result->resolveVector.y < 0.0f && this->velocity.y > 0.0f )
        this->velocity.y = 0.0f;
      else
      if( result->resolveVector.y > 0.0f && this->velocity.y < 0.0f )
        this->velocity.y = 0.0f;
    }
  }
  this->SetPositionBy( result->resolveVector );
  if( result->resolveVector.y != 0.0f && Sign( result->resolveVector.y ) != Sign( this->velocity.y ) ) {
    this->velocity.y *= 0.5f;
  }

  this->Update( 0.0f );
}//ResolveCollision






/*
=============
  SaveToBuffer
=============
*/
void Collision::SaveToBuffer( MemoryWriter &writer )
{
  writer << this->GetVelocity();
  writer << this->GetAcceleration();
  writer << this->GetSize();
  writer << this->IsStatic();
  writer << this->GetMass();
  writer << this->GetForce();
}//SaveToBuffer



/*
=============
  LoadFromBuffer
=============
*/
void Collision::LoadFromBuffer( MemoryReader &reader )
{
  Vec3 v3;
  bool b;
  float f;

  reader >> v3;
  this->SetVelocity( v3 );

  reader >> v3;
  this->SetAcceleration( v3 );

  reader >> v3;
  this->InitSquare( v3 );

  reader >> b;
  this->SetIsStatic( b );

  reader >> f;
  this->SetMass( f );

  reader >> v3;
  this->SetForce( v3 );
}//LoadFromBuffer



/*
=============
  __Dump
=============
*/
void Collision::__Dump() {
  __log.PrintUseTime( "Collision::__Dump\n" );
  __log.PrintAligned( ". initialized: %s\n", ( this->collisionElement ? "true" : "false" ) );
  if( this->collisionElement ) {
    __log.PrintAligned( ". type: %d\n", this->collisionElement->GetType() );
    this->collisionElement->__Dump();
  }
  __log.PrintAligned( "Collision::__Dump Done\n" );
}//__Dump




/*
=============
  Render
=============
*/
void Collision::Render( float phase, const Vec3 &offset, bool isActive ) {
#ifndef COLISION_NO_OPENGL
  if( !this->collisionElement ) {
    return;
  }
  if( isActive ) {
    glColor4f( 1.0f, 0.0f, 0.0f, Math::Sqrt16( phase ) );
  } else {
    glColor4f( 0.0f, 1.0f, 0.0f, phase );
  }
  switch( this->collisionElement->GetType() ) {
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      glBegin( GL_TRIANGLE_FAN );
      CollisionElementCircle *item = ( CollisionElementCircle* ) this->collisionElement;
      for( float a = 0.0f; a < Math::PI * 2.0f; a += 0.5f ) {
        glVertex3f( this->position->x - offset.x + item->GetDiameter() * 0.5f * Math::Sin16( a ), this->_rect.leftTop.y - offset.y + item->GetDiameter() * ( 0.5f + 0.5f * Math::Cos16( a ) ), 0.0f );
      }
      glEnd();
      break;
    }
    case COLLISION_ELEMENT_TYPE_POLYGON: {
      break;
    }
    default: {
      glBegin( GL_QUADS );
      glVertex3f( this->_rect.leftTop.x - offset.x, this->_rect.leftTop.y - offset.y, 0.0f );
      glVertex3f( this->_rect.rightBottom.x - offset.x, this->_rect.leftTop.y - offset.y, 0.0f );
      glVertex3f( this->_rect.rightBottom.x - offset.x, this->_rect.rightBottom.y - offset.y, 0.0f );
      glVertex3f( this->_rect.leftTop.x - offset.x, this->_rect.rightBottom.y - offset.y, 0.0f );
      glEnd();
    }
  }
#endif
}//Render




//
//  CollisionElement
//


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
      edge0 = this->pointsResult[ num ];  //точка
      edge1 = this->pointsResult[ num - 1 ];
      edge2 = this->pointsResult[ num - 2 ];
      sign = ( edge0.x - edge1.x ) * ( edge0.y - edge2.y ) - ( edge0.y - edge1.y )*( edge0.x - edge2.x );
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
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::Update => points[%d] pointsResult[%d]", count, this->pointsResult.size() );
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
      __log.PrintInfo( Filelevel_DEBUG, ". point[%3.1f; %3.1f]", this->pointsResult[ num ].x, this->pointsResult[ num ].y );
    }
    this->_rect->leftTop.Set( min.x, min.y, 0.0f );
    this->_rect->rightBottom.Set( max.x, max.y, 0.0f );
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::Update => rect[%3.1f; %3.1f]-[%3.1f; %3.1f]", this->_rect->leftTop.x, this->_rect->leftTop.y, this->_rect->rightBottom.x, this->_rect->rightBottom.y );
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
    __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => first square test: false" );
    return false;
  }
  __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => first square test: true" );

  switch( object.GetType() ) {
    case COLLISION_ELEMENT_TYPE_SQUARE: {
      __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with square" );
      return this->TestIntersectWithSquare( object, outSolver );
      break;
    }
    case COLLISION_ELEMENT_TYPE_CIRCLE: {
      __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with circle" );
      //return this->TestIntersectWithCircle( object, outSolver );
      break;
    }
    case COLLISION_ELEMENT_TYPE_POLYGON: {
      __log.PrintInfo( Filelevel_DEBUG, "CollisionElementPolygon::TestIntersect => with polygon" );
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

  __log.PrintInfo( Filelevel_WARNING, "CollisionElementPolygon::TestIntersectWithSquare => insert algorythm" );

  //Проецируем объекты на разделяющие оси: оси полигона + 2 оси квадрата
  AxisList::const_iterator iter, iterEnd = this->axes.end();
  float projectMin[ 2 ], projectMax[ 2 ];
  Vec2 intersectAxis;
  bool isIntersected = true;
  bool intersectResultInitialized = false;
  float lastIntersectPower = 100000.0f, curIntersectPower;
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
      }
    } else {
      intersectAxis = *iter;
      intersectResultInitialized = true;
    }
    __log.PrintInfo( Filelevel_DEBUG, ". projectionObject[%3.1f; %3.1f] to axis[%3.1f; %3.1f]", projectMin[ 0 ], projectMax[ 0 ], iter->x, iter->y );
    __log.PrintInfo( Filelevel_DEBUG, ". projectionThis[%3.1f; %3.1f] to axis[%3.1f; %3.1f]", projectMin[ 1 ], projectMax[ 1 ], iter->x, iter->y );
  }//foreach axes

  __log.PrintInfo( Filelevel_DEBUG, ". result[%d] power[%3.1f] axis[%3.1f; %3.1f]", isIntersected, lastIntersectPower, intersectAxis.x, intersectAxis.y );

  return true;
}//TestIntersectWithSquare



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
