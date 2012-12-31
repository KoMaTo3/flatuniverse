#include "objecttrigger.h"
#include "collision.h"
#include "file.h"


ObjectTrigger::ObjectTrigger( Vec3 *setPosition )
:position( setPosition ), offset( 0.0f, 0.0f, 0.0f ), size( 0.0f, 0.0f, 0.0f )
{
  if( !setPosition )
    __log.PrintInfo( Filelevel_WARNING, "ObjectTrigger => position is NULL" );
}//constructor



ObjectTrigger::~ObjectTrigger()
{
}//destructor



/*
=============
  Update
=============
*/
void ObjectTrigger::Update()
{
  this->rect.leftTop      = *this->position + this->offset - this->size;
  this->rect.rightBottom  = *this->position + this->offset + this->size;
}//Update



/*
=============
  CheckCollision
=============
*/
void ObjectTrigger::CheckCollision( Collision *collision )
{
  if( !collision )
  {
    __log.PrintInfo( Filelevel_ERROR, "ObjectTrigger::CheckCollision => collision is NULL" );
    return;
  }

  bool intersect = collision->TestIntersect( this->rect.leftTop, this->rect.rightBottom );
  bool inTrigger = this->IsObjectAlreadyTriggered( collision );
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::CheckCollision => intersect[%d] triggered[%d]", intersect, inTrigger );

  if( intersect && !inTrigger ) //חאר¸כ ג ענטדדונ
  {
    __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::CheckCollision => triggered" );
    this->collisionsInTrigger.push_back( collision );
  }
  else if( !intersect && inTrigger )
  {
    __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::CheckCollision => leave from trigger" );
    collisionInTriggerList::iterator iter, iterEnd = this->collisionsInTrigger.end();
    for( iter = this->collisionsInTrigger.begin(); iter != iterEnd; ++iter )
      if( *iter == collision )
      {
        this->collisionsInTrigger.erase( iter );
        break;
      }
  }
}//CheckCollision



/*
=============
  IsObjectAlreadyTriggered
=============
*/
bool ObjectTrigger::IsObjectAlreadyTriggered( Collision *collision )
{
  if( !collision )
  {
    __log.PrintInfo( Filelevel_ERROR, "ObjectTrigger::IsObjectAlreadyTriggered => object is NULL" );
    return false;
  }

  collisionInTriggerList::iterator iter, iterEnd = this->collisionsInTrigger.end();
  for( iter = this->collisionsInTrigger.begin(); iter != iterEnd; ++iter )
    if( *iter == collision )
      return true;

  return false;
}//IsObjectAlreadyTriggered



/*
=============
  SetSize
=============
*/
ObjectTrigger* ObjectTrigger::SetSize( const Vec3 &setSize )
{
  this->size = setSize;
  return this;
}//SetSize
