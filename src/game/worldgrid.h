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


class WorldGrid;
typedef Pointer ObjectPointerType;
typedef std::vector< WorldGrid* > WorldGridList;
typedef std::vector< ObjectPointerType* > WorldGridObjectList;
extern WorldGridList *__worldGridList;

//#define WORLD_GRID_BLOCK_SIZE ( 16 )
enum WORLDGRID_STATE {
  WORLDGRID_STATE_UNKNOWN,
  WORLDGRID_STATE_LOADING,
  WORLDGRID_STATE_DONE,
  WORLDGRID_STATE_UNLOADING,
};


class WorldGrid
{
public:
  struct WorldGridPosition
  {
    Short
        x,  //колонка
        y;  //строка
    bool operator==( const WorldGridPosition& item ) const
    {
      return this->x == item.x && this->y == item.y;
    }
    WorldGridPosition( Short newX, Short newY )
      :x( newX ), y( newY )
    { }
    bool operator != ( const WorldGridPosition &with ) {
      return this->x != with.x || this->y != with.y;
    }
  };

private:
  WorldGridPosition         position;       //координаты мира
  WorldGridObjectList       objects;        //все объекты грида
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
  WorldGrid( const WorldGrid::WorldGridPosition& newPosition );
  virtual ~WorldGrid();

  void  AttachObject( Object *object );
  void  DetachObject( Object *object );
  void  DetachAll   ();
  bool  IsThisObject( Object *object );
  const WorldGridPosition&
        GetPosition () { return this->position; }
  bool  GetGridDump ( FU_OUT memory& dump );
  bool  LoadFromDump( FU_IN memory& dump, Object *rootObject, const Dword fileVersion, const bool forceLoad = false );
  bool  GetFirstMovableObject( WorldGridObjectList::iterator &iter );
  bool  GetNextMovableObject( WorldGridObjectList::iterator &iter );
  inline bool IsFullyLoaded() { return this->state == WORLDGRID_STATE_DONE || this->state == WORLDGRID_STATE_UNLOADING; }
  inline WORLDGRID_STATE GetState() { return this->state; }

  void  Update();
};
