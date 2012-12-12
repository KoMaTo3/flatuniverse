#include "keyboard.h"
#include "file.h"


Keyboard::Keyboard()
{
  this->keys.alloc        ( 1024 );
  this->keysOld.alloc     ( 1024 );
  this->keysPressed.alloc ( 1024 );
  this->keysReleased.alloc( 1024 );
  memset( this->keys.getData(),         0, this->keys.getLength() );
  memset( this->keysOld.getData(),      0, this->keys.getLength() );
  memset( this->keysPressed.getData(),  0, this->keys.getLength() );
  memset( this->keysReleased.getData(), 0, this->keys.getLength() );
}//constructor



Keyboard::~Keyboard()
{
}//destructor



/*
=============
  _ReallocBuffers
=============
*/
void Keyboard::_ReallocBuffers( Dword newKeysCount )
{
  if( this->keys.getLength() >= newKeysCount )
    return;

  memory  tmpKeys( this->keys ),
          tmpKeysOld( this->keysOld ),
          tmpKeysPressed( this->keysPressed ),
          tmpKeysReleased( this->keysReleased );
  this->keys.realloc        ( newKeysCount );
  this->keysOld.realloc     ( newKeysCount );
  this->keysPressed.realloc ( newKeysCount );
  this->keysReleased.realloc( newKeysCount );
  memset( this->keys.getData(),         0, this->keys.getLength() );
  memset( this->keysOld.getData(),      0, this->keysOld.getLength() );
  memset( this->keysPressed.getData(),  0, this->keysPressed.getLength() );
  memset( this->keysReleased.getData(), 0, this->keysReleased.getLength() );
  memcpy( this->keys.getData(),         tmpKeys.getData(),        tmpKeys.getLength() );
  memcpy( this->keysOld.getData(),      tmpKeysOld.getData(),     tmpKeysOld.getLength() );
  memcpy( this->keysPressed.getData(),  tmpKeysPressed.getData(), tmpKeysPressed.getLength() );
  memcpy( this->keysReleased.getData(), tmpKeysReleased.getData(),tmpKeysReleased.getLength() );
}//_ReallocBuffers




/*
=============
  DoPress
=============
*/
void Keyboard::DoPress( Dword keyId )
{
  if( this->keys.getLength() <= keyId )
    this->_ReallocBuffers( keyId + 1 );

  this->keys.getData()[ keyId ] = true;

  if( !this->keysOld.getData()[ keyId ] )
    this->keysPressed.getData()[ keyId ] = true;
}//DoPress



/*
=============
  DoRelease
=============
*/
void Keyboard::DoRelease( Dword keyId )
{
  if( this->keys.getLength() <= keyId )
    this->_ReallocBuffers( keyId + 1 );

  this->keys.getData()[ keyId ] = false;

  if( this->keysOld.getData()[ keyId ] )
    this->keysReleased.getData()[ keyId ] = true;
}//DoPress



/*
=============
  IsHold
=============
*/
bool Keyboard::IsHold( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keys.getData()[ keyId ] != 0;
}//IsHold



/*
=============
  IsFree
=============
*/
bool Keyboard::IsFree( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keys.getData()[ keyId ] == 0;
}//IsFree



/*
=============
  IsPressed
=============
*/
bool Keyboard::IsPressed( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keysPressed.getData()[ keyId ] != 0;
}//IsPressed



/*
=============
  IsReleased
=============
*/
bool Keyboard::IsReleased( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keysReleased.getData()[ keyId ] != 0;
}//IsReleased



/*
=============
  Update
=============
*/
void Keyboard::Update()
{
  if( this->keys.getLength() )
  {
    memcpy( this->keysOld.getData(), this->keys.getData(), this->keys.getLength() );
    memset( this->keysPressed.getData(), 0, this->keysPressed.getLength() );
    memset( this->keysReleased.getData(), 0, this->keysReleased.getLength() );
  }
}//Update
