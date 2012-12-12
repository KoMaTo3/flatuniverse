/*
  Структура хранения грида:
  :rootGridObject
   - gridr#   <= где # - номер строки грида
    - gridc#  <= где # - номер столбца грида

  Усовия выгрузки грида:
    - отсутствие активных объектов (activeObjects)
*/

#pragma once

#include "core/object.h"
#include <deque>


class WorldGrid;
typedef std::deque< WorldGrid* > WorldGridList;
extern WorldGridList *__worldGridList;

#define WORLD_GRID_BLOCK_SIZE ( 16 )


class WorldGrid
{
public:
  struct WorldGridCoordinate
  {
    Short
        x,  //колонка
        y;  //строка
  };
  union WorldGridPosition
  {
    WorldGridCoordinate pos;
    long longPos;
  };
  typedef std::deque< Object* > WorldGridObjectList;

private:
  WorldGridPosition         position;   //координаты мира
  WorldGridObjectList       objects;    //объекты грида
  //WorldGridObjectList activeObjects;    //список активных объектов, запрещающих гриду выгрузиться. грид должен сам следить за этим списком при добавлении/удалении объектов

public:
  WorldGrid( const Pos< short >& newPosition );
  virtual ~WorldGrid();

};
