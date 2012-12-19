#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


class WorldSaver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct Grid   //информация о гридах
  {
    Short x, y;
    //Dword blocksCount;    //число блоков, занимаемых гридом
    GridNumList blocksNums; //номера блоков
  };
  typedef std::deque< Grid > GridsList;
  GridNumList freeBlocks;    //номера свободных блоков
private:
  GridsList grids;

public:
  void  SaveGrid    ( Short x, Short y, memory& data );
  Grid* GridExists  ( Short x, Short y );
  void  FreeGrid    ( Short x, Short y );
};
