#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


//������ ����� ����������
const Dword WOLDSAVER_FILE_VERSION = 0x00000007;

class WorldSaver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct Grid   //���������� � ������
  {
    Short x, y;
    //Dword blocksCount;    //����� ������, ���������� ������
    GridNumList blocksNums; //������ ������
    Dword version;          //������ �����
  };
  typedef std::deque< Grid* > GridsList;
  GridNumList freeBlocks;    //������ ��������� ������
private:
  GridsList grids;
  const Dword blockSize;
  memory worldData;     //������ � ����
  std::string fileName; //����, � ������� ����������� ���

public:
  WorldSaver();

  bool  LoadGrid        ( Short x, Short y, FU_OUT memory& data, FU_OUT Dword &version );
  void  SaveGrid        ( Short x, Short y, FU_IN memory& data );
  Grid* GridExists      ( Short x, Short y );
  void  FreeGrid        ( Short x, Short y );
  void  AllocFreeBlocks ( Dword blocksCount );

  Dword LoadFromFile    ( const std::string& fileName );
  void  SaveToFile      ( const std::string& fileName = "" );
  inline const std::string& GetFileName() const {
    return this->fileName;
  }
};
