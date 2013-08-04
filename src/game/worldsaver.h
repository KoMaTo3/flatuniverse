#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


//¬ерси€ файла сохранений
const Dword WOLDSAVER_FILE_VERSION = 0x00000007;

class WorldSaver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct Grid   //информаци€ о гридах
  {
    Short x, y;
    //Dword blocksCount;    //число блоков, занимаемых гридом
    GridNumList blocksNums; //номера блоков
    Dword version;          //верси€ грида
  };
  typedef std::deque< Grid* > GridsList;
  GridNumList freeBlocks;    //номера свободных блоков
private:
  GridsList grids;
  const Dword blockSize;
  memory worldData;     //данные о мире
  std::string fileName; //файл, в который сохран€етс€ мир

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
