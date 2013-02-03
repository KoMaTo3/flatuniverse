#include "collision.h"
#include "file.h"


Vec3 __nullCollisionPosition( 0.0f, 0.0f, 0.0f );


Collision::Collision( Vec3* objectPosition )
:position( NULL ), velocity( 0.0f, 0.0f, 0.0f ), acceleration( 0.0f, 0.0f, 0.0f ), isStatic( true ), mass( 1.0f ), force( 0.0f, 0.0f, 0.0f )
{
  this->position = objectPosition;
}//constructor



Collision::~Collision()
{
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
  Vec3 halfSize           = this->size * 0.5f;
  this->_rect.leftTop     = *this->position - halfSize;
  this->_rect.rightBottom = *this->position + halfSize;
  //this->_rect.radius2     = halfSize.x * halfSize.x + halfSize.y * halfSize.y;
  this->_rect.radius2     = this->size.x * this->size.x + this->size.y * this->size.y;

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
  SetSize
=============
*/
Collision* Collision::SetSize( const Vec3& newSize )
{
  this->size = newSize;
  return this;
}//SetSize



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
  if( this->_rect.rightBottom.x <= item._rect.leftTop.x ||
    this->_rect.leftTop.x       >= item._rect.rightBottom.x ||
    this->_rect.rightBottom.y   <= item._rect.leftTop.y ||
    this->_rect.leftTop.y       >= item._rect.rightBottom.y
    )
    return false;

  Vec3 intersectPower = Vec3(
      Math::Fabs( max( this->_rect.leftTop.x, item._rect.leftTop.x ) - min( this->_rect.rightBottom.x, item._rect.rightBottom.x ) ),
      Math::Fabs( max( this->_rect.leftTop.y, item._rect.leftTop.y ) - min( this->_rect.rightBottom.y, item._rect.rightBottom.y ) ),
      0.0f
    );
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
    if( item0->_rect.leftTop.x < item1->_rect.leftTop.x ) //двигаем влево
      itemResolver0.resolveVector.x = -intersectPower.x;
    else
      itemResolver0.resolveVector.x = intersectPower.x;
    if( item0->_rect.leftTop.y < item1->_rect.leftTop.y ) //поднимаем
      itemResolver0.resolveVector.y = -intersectPower.y;
    else
      itemResolver0.resolveVector.y = intersectPower.y;
    item0->resolver.push_back( itemResolver0 );
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
  this->SetPositionBy( result->resolveVector );

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
  this->SetSize( v3 );

  reader >> b;
  this->SetIsStatic( b );

  reader >> f;
  this->SetMass( f );

  reader >> v3;
  this->SetForce( v3 );
}//LoadFromBuffer
