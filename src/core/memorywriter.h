#pragma once

#include "memory.h"
#include "klib.h"
#include <string>

class MemoryWriter
{
private:
  memory  &buffer;  //������, � ������� ������ �������
  Dword   pos;      //������� ������� � �������
  Dword blockSize;  //������ �����, ����������� ��� �������� ����� � �����

public:
  MemoryWriter( memory& dest, Dword newBlockSize = 1024 );
  ~MemoryWriter();

  inline Dword  GetLength() { return this->pos; }

  MemoryWriter& operator<<( const std::string &str );

  template< class T >
  MemoryWriter& operator<<( T data )
  {
    this->CheckDataSizeToWrite( data );
    memcpy( this->buffer.getData() + this->pos, &data, sizeof( data ) );
    this->pos += sizeof( data );
    return *this;
  }

  void CheckSizeToWrite( Dword sizeToWrite );

  template< class T >
  void CheckDataSizeToWrite( T dataToWrite )
  {
    this->CheckSizeToWrite( sizeof( dataToWrite ) );
  }//CheckDataSizeToWrite
};
