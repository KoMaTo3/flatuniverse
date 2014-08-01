/*
  Структура хранения грида:
  |НЕТ| :rootGridObject
  |НЕТ|  - gridr#   <= где # - номер строки грида
  |НЕТ|   - gridc#  <= где # - номер столбца грида
  новая структура:
  грид сам по себе
  у грида есть deque-список объектов

  Усовия выгрузки грида:
    - отсутствие активных объектов (activeObjects)
*/

#pragma once

#include "core/object.h"
#include "core/objectpointer.h"
#include <list>
#include <vector>

namespace World {
  class Grid;

  typedef Pointer ObjectPointerType;
  typedef std::vector< Grid* > GridList;
  typedef std::vector< ObjectPointerType* > GridObjectList;
  extern GridList *__worldGridList;


//#define WORLD_GRID_BLOCK_SIZE ( 16 )
enum WORLDGRID_STATE {
  WORLDGRID_STATE_UNKNOWN,
  WORLDGRID_STATE_LOADING,
  WORLDGRID_STATE_DONE,
  WORLDGRID_STATE_UNLOADING,
};


class Grid
{
public:
  struct Position
  {
    Short
        x,  //колонка
        y;  //строка
    bool operator==( const Position& item ) const
    {
      return this->x == item.x && this->y == item.y;
    }
    Position( Short newX, Short newY )
      :x( newX ), y( newY )
    { }
    bool operator != ( const Position &with ) {
      return this->x != with.x || this->y != with.y;
    }
  };

private:
  Position                  position;       //координаты мира
  GridObjectList            objects;        //все объекты грида
  Dword                     version;        //версия
  //bool                      isFullyLoaded;  //флаг полной загруженности грида из дампа
  WORLDGRID_STATE           state;
  struct {
    memory                  dump;
    MemoryReader            reader;
    Dword                   objectsCount;
    Object                  *rootObject;
  } loader;
  //WorldGridObjectList activeObjects;    //список активных объектов, запрещающих гриду выгрузиться. грид должен сам следить за этим списком при добавлении/удалении объектов

public:
  Grid( const Position& newPosition );
  virtual ~Grid();

  void  AttachObject( Object *object );
  void  DetachObject( Object *object );
  void  DetachAll   ();
  bool  IsThisObject( Object *object );
  const Position&
        GetPosition () { return this->position; }
  bool  GetGridDump ( FU_OUT memory& dump );
  bool  LoadFromDump( FU_IN memory& dump, Object *rootObject, const Dword fileVersion, const bool forceLoad = false );
  bool  GetFirstMovableObject( GridObjectList::iterator &iter );
  bool  GetNextMovableObject( GridObjectList::iterator &iter );
  inline bool IsFullyLoaded() { return this->state == WORLDGRID_STATE_DONE || this->state == WORLDGRID_STATE_UNLOADING; }
  inline WORLDGRID_STATE GetState() { return this->state; }

  void  Update();

private:
  Grid();
  Grid( const Grid& );
  Grid& operator=( const Grid& );
};

};
