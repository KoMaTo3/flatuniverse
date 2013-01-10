#include "objectpointer.h"



ObjectPointer::ObjectPointer()
:object( NULL ), valid( false )
{
}//constructor



ObjectPointer::ObjectPointer( const ObjectPointer& copyFrom )
:object( copyFrom.object ), valid( copyFrom.valid )
{
  if( copyFrom.object )
    __log.PrintInfo( Filelevel_ERROR, "ObjectPointer +1: copy-operator from pointer[x%X] with object[x%X]", &copyFrom, copyFrom.object );
}//constructor



ObjectPointer::ObjectPointer( IObjectPointer *setObject )
:object( NULL ), valid( false )
{
  this->Init( setObject );
}//constructor



ObjectPointer::~ObjectPointer()
{
  this->Reset();
}//destructor




void ObjectPointer::Init( IObjectPointer *setObject )
{
  if( this->object || this->valid )
  {
    __log.PrintInfo( Filelevel_ERROR, "ObjectPointer::Init => already initialized by object x%X", this->object );
    return;
  }
  this->object = setObject;
  this->valid = ( setObject != NULL );
  if( setObject )
    setObject->PointerAdd( this );
  if( setObject )
    __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer +1: object[x%X] pointer[x%X]", setObject, this );
}//Init



void ObjectPointer::Reset()
{
  if( this->object )
    __log.PrintInfo( Filelevel_DEBUG, "ObjectPointer -1: object[x%X] valid[%d] pointer[x%X]", this->object, this->valid, this );
  if( this->GetValid() )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove, object[x%X]...", this->object );
    this->object->PointerRemove( this );
    __log.PrintInfo( Filelevel_DEBUG, ". pointer remove ok" );
  }
  this->valid = false;
  this->object = NULL;
}//Reset
