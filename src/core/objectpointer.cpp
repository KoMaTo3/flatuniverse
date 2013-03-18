#include "objectpointer.h"




ObjectPointer::ObjectPointer( const ObjectPointer& copyFrom )
:object( copyFrom.object ), valid( copyFrom.valid )
{
  if( copyFrom.object )
    __log.PrintInfo( Filelevel_ERROR, "ObjectPointer +1: copy-operator from pointer[x%p] with object[x%p]", &copyFrom, copyFrom.object );
}//constructor



ObjectPointer::ObjectPointer( IObjectPointer *setObject )
:object( setObject )
{
  this->valid = ( setObject != NULL );
  __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer +1: object[x%p] pointer[x%p]", setObject, this );
  if( setObject ) {
    setObject->PointerAdd( this );
  }
}//constructor



ObjectPointer::~ObjectPointer()
{
  if( this->object )
    __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer -1: object[x%p] valid[%d] pointer[x%p]", this->object, this->valid, this );
  if( this->GetValid() )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove, object[x%p]...", this->object );
    this->object->PointerRemove( this );
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove ok" );
  }
  this->valid = false;
  this->object = NULL;
}//destructor



/*
void ObjectPointer::Init( IObjectPointer *setObject )
{
  if( this->object || this->valid )
  {
    __log.PrintInfo( Filelevel_ERROR, "ObjectPointer::Init => already initialized by object x%p", this->object );
    return;
  }
  this->object = setObject;
  this->valid = ( setObject != NULL );
  if( setObject )
    setObject->PointerAdd( this );
  if( setObject )
    __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer +1: object[x%p] pointer[x%p]", setObject, this );
}//Init



void ObjectPointer::Reset()
{
  if( this->object )
    __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer -1: object[x%p] valid[%d] pointer[x%p]", this->object, this->valid, this );
  if( this->GetValid() )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove, object[x%p]...", this->object );
    this->object->PointerRemove( this );
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove ok" );
  }
  this->valid = false;
  this->object = NULL;
}//Reset
*/
