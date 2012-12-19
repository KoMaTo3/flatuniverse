#include "memorywriter.h"



MemoryWriter::MemoryWriter( memory& dest, Dword newBlockSize )
:buffer( dest ), pos( 0 ), blockSize( newBlockSize )
{
}//constructor



MemoryWriter::~MemoryWriter()
{
}//destructor



MemoryWriter& MemoryWriter::operator<<( const std::string &str )
{
  Dword strLen = str.size();
  this->CheckDataSizeToWrite( sizeof( strLen ) + strLen );

  this->operator<<( strLen );
  memcpy( this->buffer.getData() + this->pos, &str[ 0 ], strLen );

  this->pos += strLen;

  return *this;
}




/*
=============
  CheckSizeToWrite
=============
*/
void MemoryWriter::CheckSizeToWrite( Dword sizeToWrite )
{
    if( this->pos + sizeToWrite > this->buffer.getLength() )
    {
      Dword toAdd = Dword( Math::Ceil( float( sizeToWrite - ( this->buffer.getLength() - this->pos ) ) / float( this->blockSize ) ) ) * this->blockSize;
      if( this->buffer.getLength() )
      {
        memory temp( this->buffer );
        this->buffer.realloc( this->buffer.getLength() + toAdd );
        memcpy( this->buffer.getData(), temp.getData(), temp.getLength() );
      }
      else
        this->buffer.alloc( toAdd );
    }
}//CheckSizeToWrite
