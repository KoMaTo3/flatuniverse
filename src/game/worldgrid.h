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
#include <deque>


class WorldGrid;
typedef std::deque< WorldGrid* > WorldGridList;
typedef std::deque< Object* > WorldGridObjectList;
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

private:
  WorldGridPosition         position;   //координаты мира
  WorldGridObjectList       objects;    //все объекты грида
  //WorldGridObjectList activeObjects;    //список активных объектов, запрещающих гриду выгрузиться. грид должен сам следить за этим списком при добавлении/удалении объектов

public:
  WorldGrid( const Pos< short >& newPosition );
  virtual ~WorldGrid();

};
