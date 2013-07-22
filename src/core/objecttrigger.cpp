#include "objecttrigger.h"
#include "collision.h"
#include "file.h"
#include "memorywriter.h"
#include "memoryreader.h"

extern ObjectTriggerOnRemoveHandler __ObjectTriggerOnRemoveGlobalHandler = NULL;
ObjectTrigger::TriggerHandlerInit *ObjectTrigger::InitTriggerHandler = NULL;
luaTriggerListenersList *ObjectTrigger::triggerLintenersList = NULL;
ObjectTrigger::TriggerHandler *ObjectTrigger::defaultTriggerHandler = NULL;


ObjectTrigger::ObjectTrigger( Vec3 *setPosition )
:position( setPosition ), offset( 0.0f, 0.0f, 0.0f )
{
  if( !setPosition )
    __log.PrintInfo( Filelevel_ERROR, "ObjectTrigger => position is NULL" );
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger +1" );
  this->triggerRect = new Collision( this->position );
}//constructor



ObjectTrigger::~ObjectTrigger()
{
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::~ObjectTrigger" );
  ObjectTriggerOnRemoveHandlerList::iterator iter, iterEnd = this->onRemoveHandlers.end();
  for( iter = this->onRemoveHandlers.begin(); iter != iterEnd; ++iter )
    ( *iter )( this );
  __log.PrintInfo( Filelevel_DEBUG, "__ObjectTriggerOnRemoveGlobalHandler = x%X", __ObjectTriggerOnRemoveGlobalHandler );
  if( __ObjectTriggerOnRemoveGlobalHandler )
    __ObjectTriggerOnRemoveGlobalHandler( this );
  DEF_DELETE( this->triggerRect );
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger -1" );
}//destructor



/*
=============
  Update
=============
*/
void ObjectTrigger::Update()
{
  this->triggerPosition   = *this->position + this->offset;
  this->triggerRect->Update( 0.0f );
  //this->rect.leftTop      = this->triggerPosition - this->size * 0.5f;
  //this->rect.rightBottom  = this->triggerPosition + this->size * 0.5f;
}//Update



/*
=============
  TestInPoint
=============
*/
bool ObjectTrigger::TestInPoint( const Vec2 &pos )
{
  return this->triggerRect->TestInPoint( pos );
  /*
  return
    !( this->rect.rightBottom.x < pos.x ||
     this->rect.leftTop.x       > pos.x ||
     this->rect.rightBottom.y   < pos.y ||
     this->rect.leftTop.y       > pos.y
    );
    */
}//TestInPoint




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

  bool intersect = collision->TestIntersect( *this->triggerRect );
  bool inTrigger = this->IsObjectAlreadyTriggered( collision );

  if( intersect && !inTrigger ) //חאר¸כ ג ענטדדונ
  {
    __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::CheckCollision => triggered" );
    this->collisionsInTrigger.push_back( collision );
    this->_TouchHandlers( collision, inTrigger );
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
    this->_TouchHandlers( collision, inTrigger );
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
  SetPolygon
=============
*/
ObjectTrigger* ObjectTrigger::SetPolygon( const CollisionElementPolygon::PointList &points )
{
  this->triggerRect->InitPolygon( points );
  return this;
}//SetPolygon



/*
=============
  Redraw
=============
*/
void ObjectTrigger::Redraw( float phase, const Vec3 &offset, bool isActive ) {
  if( this->triggerRect ) {
    this->triggerRect->Render( phase, offset, isActive );
  }
}//Redraw



/*
=============
  SetSize
=============
*/
ObjectTrigger* ObjectTrigger::SetSize( const Vec3 &setSize )
{
  this->triggerRect->InitSquare( setSize );
  return this;
}//SetSize



/*
=============
  GetSize
=============
*/
const Vec3& ObjectTrigger::GetSize( )
{
  return this->triggerRect->GetSize();
}//GetSize



/*
=============
  AddHandler
=============
*/
void ObjectTrigger::AddHandler( ObjectTriggerHandler handler )
{
  if( this->IsHandlerExists( handler ) )
    return;
  this->handlers.push_back( handler );
}//AddHandler



/*
=============
  IsHandlerExists
=============
*/
bool ObjectTrigger::IsHandlerExists( ObjectTriggerHandler handler )
{
  ObjectTriggerHandlerList::iterator iter, iterEnd = this->handlers.end();
  for( iter = this->handlers.begin(); iter != iterEnd; ++iter )
    if( *iter == handler )
      return true;
  return false;
}//IsHandlerExists



/*
=============
  _TouchHandlers
=============
*/
void ObjectTrigger::_TouchHandlers( Collision *collision, bool isInTrigger )
{
  ObjectTriggerHandlerList::iterator iter, iterEnd = this->handlers.end();
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::_TouchHandlers => count[%d]", this->handlers.size() );
  for( iter = this->handlers.begin(); iter != iterEnd; ++iter )
    ( *iter)( this, collision, isInTrigger );
}//_TouchHandlers



void ObjectTrigger::AddOnRemoveHandler( ObjectTriggerOnRemoveHandler handler )
{
  this->onRemoveHandlers.push_back( handler );
}//AddOnRemoveHandler





/*
=============
  SaveToBuffer
=============
*/
void ObjectTrigger::SaveToBuffer( MemoryWriter &writer )
{
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::SaveToBuffer..." );
  writer << this->offset;
  this->triggerRect->SaveToBuffer( writer );

  //handlers
  Dword count = this->handlers.size();
  writer << count;
  if( count && ObjectTrigger::triggerLintenersList ) {
    __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::SaveToBuffer => count[%d]", count );
    luaTriggerListenersList::const_iterator iter, iterEnd = ObjectTrigger::triggerLintenersList->end();
    for( iter = ObjectTrigger::triggerLintenersList->begin(); iter != iterEnd; ++iter ) {
      if( iter->object == this ) {
        writer << iter->funcName;
        __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::SaveToBuffer => . handler['%s']", iter->funcName.c_str() );
      }
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::SaveToBuffer done" );
}//SaveToBuffer



/*
=============
  LoadFromBuffer
=============
*/
void ObjectTrigger::LoadFromBuffer( MemoryReader &reader, const std::string &thisObjectName, const Dword version )
{
  Vec3 v3;

  reader >> v3;
  this->offset = v3;

  DEF_DELETE( this->triggerRect );
  this->triggerRect = new Collision( this->position );
  this->triggerRect->LoadFromBuffer( reader, "", version );

  if( version >= 0x00000005 ) {
    //handlers
    Dword count, q;
    reader >> count;
    __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::LoadFromBuffer => handlers[%d]", count );
    std::string s;
    if( count && this->InitTriggerHandler && ObjectTrigger::triggerLintenersList ) { //something wrong...
      for( q = 0; q < count; ++q ) {
        reader >> s;
        __log.PrintInfo( Filelevel_DEBUG, "ObjectTrigger::LoadFromBuffer => . handler['%s']", s.c_str() );
        this->InitTriggerHandler( s, thisObjectName );
      }
    }
  }
}//LoadFromBuffer
