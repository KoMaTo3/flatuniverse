/*==============================
  Класс MemoryReader
  Класс работы с буффером как с файлом
==============================*/

#pragma once

#include "memory.h"

class MemoryReader
{
private:
  memory  data;
  int     curPos,
          curLength;
  char    *curData;  //либо указатель на data либо на внешний блок

public:
  MemoryReader();
  virtual ~MemoryReader();

  void    SetSource     ( void *srcData, int length );
  void    CopyFromSource( void *srcData, int length );
  int     Read          ( void *dest, int len );
  size_t  Read          ( void *dest, int len, int numBlocks );
  void    Free          ();
  void    SeekFromStart ( int pos );
  void    SeekFromCur   ( int pos );
  inline
    int   GetCurPos     () { return this->curPos; }
};
