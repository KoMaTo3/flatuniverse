#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


namespace World {


//������ ����� ����������
const Dword VERSION = 0x0000000B;


class Saver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct GridInfo   //���������� � ������
  {
    Short x, y;
    //Dword blocksCount;    //����� ������, ���������� ������
    GridNumList blocksNums; //������ ������
    Dword version;          //������ �����
  };
  typedef std::deque< GridInfo* > GridsList;
  GridNumList freeBlocks;    //������ ��������� ������
private:
  GridsList grids;
  const Dword blockSize;
  memory worldData;     //������ � ����
  std::string fileName; //����, � ������� ����������� ���

public:
  Saver();

  bool  LoadGrid        ( Short x, Short y, FU_OUT memory& data, FU_OUT Dword &version );
  void  SaveGrid        ( Short x, Short y, FU_IN memory& data );
  GridInfo* GridExists  ( Short x, Short y );
  void  FreeGrid        ( Short x, Short y );
  void  AllocFreeBlocks ( Dword blocksCount );

  Dword LoadFromFile    ( const std::string& fileName );
  void  SaveToFile      ( const std::string& fileName = "" );
  inline const std::string& GetFileName() const {
    return this->fileName;
  }
};

};
