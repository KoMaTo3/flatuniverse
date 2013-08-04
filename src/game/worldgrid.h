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


class WorldGrid;
typedef Pointer ObjectPointerType;
typedef std::deque< WorldGrid* > WorldGridList;
typedef std::deque< ObjectPointerType* > WorldGridObjectList;
extern WorldGridList *__worldGridList;

//#define WORLD_GRID_BLOCK_SIZE ( 16 )


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
  WorldGridPosition         position;   //координаты мира
  WorldGridObjectList       objects;    //все объекты грида
  Dword                     version;    //версия
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
  bool  LoadFromDump( FU_IN memory& dump, Object *rootObject, const Dword fileVersion );
  bool  GetFirstMovableObject( WorldGridObjectList::iterator &iter );
  bool  GetNextMovableObject( WorldGridObjectList::iterator &iter );

  void  Update();
};
