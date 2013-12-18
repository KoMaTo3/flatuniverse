#pragma once

#include "core/klib.h"
#include "core/memory.h"
#include <deque>


namespace World {


//¬ерси€ файла сохранений
const Dword VERSION = 0x0000000B;


class Saver
{
public:
  typedef std::deque< Dword > GridNumList;
  struct GridInfo   //информаци€ о гридах
  {
    Short x, y;
    //Dword blocksCount;    //число блоков, занимаемых гридом
    GridNumList blocksNums; //номера блоков
    Dword version;          //верси€ грида
  };
  typedef std::deque< GridInfo* > GridsList;
  GridNumList freeBlocks;    //номера свободных блоков
private:
  GridsList grids;
  const Dword blockSize;
  memory worldData;     //данные о мире
  std::string fileName; //файл, в который сохран€етс€ мир

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
