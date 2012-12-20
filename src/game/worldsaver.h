#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


class WorldSaver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct Grid   //���������� � ������
  {
    Short x, y;
    //Dword blocksCount;    //����� ������, ���������� ������
    GridNumList blocksNums; //������ ������
  };
  typedef std::deque< Grid* > GridsList;
  GridNumList freeBlocks;    //������ ��������� ������
private:
  GridsList grids;
  const Dword blockSize;
  memory worldData;     //������ � ����

public:
  WorldSaver();

  bool  LoadGrid    ( Short x, Short y, FU_OUT memory& data );
  void  SaveGrid    ( Short x, Short y, FU_IN memory& data );
  Grid* GridExists  ( Short x, Short y );
  void  FreeGrid    ( Short x, Short y );
};
