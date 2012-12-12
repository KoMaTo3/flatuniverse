#include "memoryreader.h"


MemoryReader::MemoryReader()
:curPos( 0 )
{
}//constructor


MemoryReader::~MemoryReader()
{
}//destructor


/*
----------
  Read
  Чтение блока
----------
*/
int MemoryReader::Read( void *dest, int length )
{
  int realLen = min( length, this->curLength - this->curPos );

  memcpy( dest, this->curData + this->curPos, realLen );
  this->curPos += realLen;

  return realLen;
}//Read


/*
----------
  Read
  Чтение нескольких блоков
----------
*/
size_t MemoryReader::Read( void *dest, int length, int numBlocks )
{
  int realLen = min( length * numBlocks, this->curLength - this->curPos );
  size_t ret;
  if( realLen < length * numBlocks )
    ret = size_t( realLen / length );
  else
    ret = numBlocks;

  memcpy( dest, this->curData + this->curPos, realLen );
  this->curPos += realLen;

  return ret;
}//Read


/*
----------
  SetSource
  Установка источника данных
----------
*/
void MemoryReader::SetSource( void *srcData, int length )
{
  this->curData = (char*) srcData;
  this->curLength = length;
  this->curPos = 0;
}//SetSource


/*
----------
  CopyFromSource
----------
*/
void MemoryReader::CopyFromSource( void *srcData, int length )
{
  if( !srcData )
    return;

  this->data.alloc( length );
  this->curData = this->data.getData();
  memcpy( this->curData, srcData, length );
  this->curLength = length;
}//CopyFromSource


/*
----------
  Free
----------
*/
void MemoryReader::Free()
{
  this->data.free();
  this->curLength = this->curPos = 0;
  this->curData = 0;
}//Free


/*
----------
  SeekFromStart
----------
*/
void MemoryReader::SeekFromStart( int pos )
{
  if( pos > this->curLength )
    return;

  this->curPos = pos;
}//SeekFromStart


/*
----------
  SeekFromCur
----------
*/
void MemoryReader::SeekFromCur( int pos )
{
  if( this->curPos + pos > this->curLength )
    return;

  this->curPos += pos;
}//SeekFromCur
