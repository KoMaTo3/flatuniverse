#include "collision.h"
#include "file.h"
#include "gl/gl.h"

//разделяющие оси могу иметь погрешности, поэтому игнорируем некоторые почти-параллельные оси


#define COLLISION_FRICTION_FORCE   ( 0.98f )


Vec3 __nullCollisionPosition( 0.0f, 0.0f, 0.0f );

Collision::CollisionHandlerInit *Collision::InitCollisionHandler = NULL;
luaCollisionListenersList *Collision::collisionListenersList = NULL;
Collision::CollisionHandler *Collision::defaultCollisionHandler = NULL;


Collision::Collision( Vec3* objectPosition )
:position( NULL ), velocity( 0.0f, 0.0f, 0.0f ), acceleration( 0.0f, 0.0f, 0.0f ), isStatic( true ), mass( 1.0f )
, force( 0.0f, 0.0f, 0.0f ), collisionElement( NULL ), handlers( NULL ), newHandlers( NULL ), offset( Vec3Null ), positionLast( 0.0f, 0.0f, 0.0f ), _rect()
{
  this->position = objectPosition;
  this->positionResult = *this->position + this->offset;
  if( this->position->Length() < 0.1f ) __log.PrintInfo( Filelevel_DEBUG, "+Collision: position is 0:0" );
}//constructor



Collision::~Collision()
{
  DEF_DELETE( collisionElement );
  DEF_DELETE( this->handlers );
  DEF_DELETE( this->newHandlers );
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

  this->positionLast = this->positionResult;
  //__log.PrintInfo( Filelevel_DEBUG, "Collision::Update => dt[%3.3f] pos[%3.3f;%3.3f] vel[%3.3f;%3.3f] acc[%3.3f;%3.3f]", dt, this->position->x, this->position->y, this->velocity.x, this->velocity.y, this->acceleration.x, this->acceleration.y );
  this->velocity += this->acceleration * dt;
  this->velocity.y *= COLLISION_FRICTION_FORCE;
  *this->position += ( this->velocity + this->force ) * dt;
  this->positionResult = *this->position + this->offset;

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
  this->positionResult = *this->position + this->offset;
  if( this->position->Length() < 0.1f ) __log.PrintInfo( Filelevel_DEBUG, "Collision::SetPosition position is 0:0" );
  return this;
}//SetPosition



/*
=============
  SetZ
=============
*/
Collision* Collision::SetZ( const float newZ )
{
  if( !this->position )
  {
    __log.PrintInfo( Filelevel_ERROR, "Collision::SetZ => NULL pointer" );
    return this;
  }
  this->position->z = newZ;
  this->positionResult.z = this->position->z + this->offset.z;
  return this;
}//SetZ



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
  this->positionResult = *this->position + this->offset;
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
    CollisionElementSquare *item = new CollisionElementSquare( &this->positionResult, &this->_rect );
    this->collisionElement = item;
    item->SetSize( newSize );
    //__log.PrintInfo( Filelevel_DEBUG, "Collision::InitSquare => size[%3.1f; %3.1f; %3.1f;]", newSize.x, newSize.y, newSize.z );
  } else if( this->collisionElement->GetType() != COLLISION_ELEMENT_TYPE_SQUARE ) {
    __log.PrintInfo( Filelevel_ERROR, "Collision::InitSquare => this already initialized by type %d", this->collisionElement->GetType() );
    return this;
  } else {
    ( ( CollisionElementSquare* ) this->collisionElement )->SetSize( newSize );
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
  CollisionElementCircle *item = new CollisionElementCircle( &this->positionResult, &this->_rect );
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
  CollisionElementPolygon *item = new CollisionElementPolygon( &this->positionResult, &this->_rect );
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
  return this->positionResult;
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
    //__log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => no intersect" );
    return false;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => intersect, solving" );
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
    //__log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => dynamic + dynamic" );
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
    if( item0->collisionElement->GetType() == COLLISION_ELEMENT_TYPE_SQUARE && item0->collisionElement->GetType() == item1->collisionElement->GetType() ) {
      itemResolver0.useAllAxices = 0;
      itemResolver1.useAllAxices = 0;
    }
    itemResolver0.target = item1;
    itemResolver1.target = item0;
    //__log.PrintInfo( Filelevel_DEBUG, "Collision::TestIntersect => item0[x%p] item1[x%p]", item0, item1 );
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
    itemResolver0.target = item1;
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
      //__log.PrintInfo( Filelevel_DEBUG, "testz: %3.1f; %3.1f", itemResolver0.resolveVector.x, itemResolver0.resolveVector.y );
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
  if( this->resolver.empty() )
    return;

  CollisionResolver *result = &( *this->resolver.begin() );
  float resultSquare = result->power.x * result->power.y;
  CollisionResolverList::iterator iter = this->resolver.begin(), iterEnd = this->resolver.end();
  for( ++iter; iter != iterEnd; ++iter )
  {
    float square = iter->power.x * iter->power.y;
    if( square > resultSquare )
    {
      resultSquare = square;
      result = &( *iter );
    }
  }
  //__log.PrintInfo( Filelevel_DEBUG, ". power[%3.3f; %3.3f] vector[%3.3f; %3.3f] useAllAxices[%d]", result->power.x, result->power.y, result->resolveVector.x, result->resolveVector.y, result->useAllAxices );

  Vec3 resolved( Vec3Null );
  Byte flags = 0;
  Vec3 oldVelocity( this->GetVelocity() );
  if( result->useAllAxices ) {
    //Vec3 coeffVec = result->resolveVector;
    //coeffVec.Normalize();
    //float coeffSign = Sign( coeffVec.y );
    //__log.PrintInfo( Filelevel_DEBUG, "coeffVec: %3.1f:%3.1f", Sign( this->velocity.y ), Sign( coeffVec.y ) );
    //if( Sign( this->velocity.y ) != coeffSign && coeffSign != 0.0f ) {
    //  this->velocity.y = 0.0f;
    //}
  } else {
    if( result->power.x < result->power.y ) { //смещаем по x
      result->resolveVector.y = 0.0f;
      flags |= ( result->resolveVector.x < 0.0f ? 8 : 2 );
    } else { //смещаем по y
      result->resolveVector.x = 0.0f;
      if( result->resolveVector.y < 0.0f && this->velocity.y > 0.0f ) {
        flags |= 4;
      } else if( result->resolveVector.y > 0.0f && this->velocity.y < 0.0f ) {
        flags |= 1;
      }
    }
  }
  this->SetPositionBy( result->resolveVector * 0.99f );
  if( result->resolveVector.y != 0.0f && Sign( result->resolveVector.y ) != Sign( this->velocity.y ) ) {
    float velocityYCoeff = min( 1.0f, 1.0f / Math::Fabs( result->resolveVector.y ) );
    this->velocity.y *= velocityYCoeff;
  }

  //call handlers
  if( this->handlers && !this->handlers->empty() ) {
    CollisionHandlerList::const_iterator iter, iterEnd = this->handlers->end();
    for( iter = this->handlers->begin(); iter != iterEnd; ++iter ) {
      ( *iter )( this, result->target, flags, oldVelocity );
    }
  }
  if( this->newHandlers && !this->newHandlers->empty() ) {
    for( auto &handler: *this->newHandlers ) {
      this->handlers->push_back( handler );
    }
    this->newHandlers->clear();
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
  //writer << this->GetSize();
  writer << this->IsStatic();
  writer << this->GetMass();
  writer << this->GetForce();

  writer << this->collisionElement->GetType();
  this->collisionElement->SaveToBuffer( writer );

  //handlers
  /*
  Dword count = ( this->handlers ? this->handlers->size() : 0 );
  writer << count;
  if( this->handlers ) {
    __log.PrintInfo( Filelevel_DEBUG, "Collision::SaveToBuffer => handlerList[x%p]", this->handlers );
  }
  if( count && Collision::collisionListenersList ) {
    __log.PrintInfo( Filelevel_DEBUG, "Collision::SaveToBuffer => handlers[x%p] count[%d]", this->handlers, count );
    luaCollisionListenersList::const_iterator iter, iterEnd = Collision::collisionListenersList->end();
    for( iter = Collision::collisionListenersList->begin(); iter != iterEnd; ++iter ) {
      if( iter->object == this ) {
        writer << iter->funcName;
        __log.PrintInfo( Filelevel_DEBUG, "Collision::SaveToBuffer => . handler['%s']", iter->funcName.c_str() );
      }
    }
  }
  */
}//SaveToBuffer



/*
=============
  LoadFromBuffer
=============
*/
void Collision::LoadFromBuffer( MemoryReader &reader, const std::string &thisObjectName, const Dword version )
{
  Vec3 v3;
  Vec2 v2;
  bool b;
  float f;
  //int i;
  CollisionElementType cet;

  reader >> v3;
  this->SetVelocity( v3 );

  reader >> v3;
  this->SetAcceleration( v3 );

  //reader >> v3;
  //this->InitSquare( v3 );

  reader >> b;
  this->SetIsStatic( b );

  reader >> f;
  this->SetMass( f );

  reader >> v3;
  this->SetForce( v3 );

  reader >> cet;
  switch( cet ) {
    case COLLISION_ELEMENT_TYPE_SQUARE:
      reader >> v3;
      //__log.PrintInfo( Filelevel_DEBUG, "Collision::LoadFromBuffer => squareSize[%3.1f; %3.1f; %3.1f;]", v3.x, v3.y, v3.z );
      this->InitSquare( v3 );
      break;
    case COLLISION_ELEMENT_TYPE_CIRCLE:
      reader >> f;
      this->InitCircle( f );
      break;
    case COLLISION_ELEMENT_TYPE_POLYGON: {
      int i;
      reader >> i;  //points in poly
      CollisionElementPolygon::PointList poly;
      for( int num = 0; num < i; ++num ) {
        reader >> v2;
        poly.push_back( v2 );
      }
      this->InitPolygon( poly );
      break;
    }
    default:
      break;
  }
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
  this->collisionElement->_Render( offset );
  /*
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
  */
#endif
}//Render




/*
=============
  AddHandler
=============
*/
void Collision::AddHandler( CollisionHandler *handler ) {
  if( !this->handlers ) {
    this->handlers = new CollisionHandlerList;
    this->newHandlers = new CollisionHandlerList;
  }

  this->newHandlers->push_back( handler );
  //__log.PrintInfo( Filelevel_DEBUG, "Collision::AddHandler => collision[x%p] handler[x%p]", this, handler );
}//AddHandler



/*
=============
  GetPolygon
=============
*/
bool Collision::GetPolygon( CollisionElementPolygon::PointList& result ) {
  if( this->GetType() != COLLISION_ELEMENT_TYPE_POLYGON ) {
    result.clear();
    return false;
  }
  static_cast< CollisionElementPolygon* >( this->collisionElement )->GetPointList( result );
  return true;
}//GetPolygon
