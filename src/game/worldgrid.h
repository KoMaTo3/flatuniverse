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


class WorldGrid
{
public:
  struct WorldGridCoordinate
  {
    Short x, y;
  };
  union WorldGridPosition
  {
    WorldGridCoordinate pos;
    long longPos;
  };
  typedef std::deque< Object* > WorldGridActiveObjectList;

private:
  Object                    *rootGridObject;  //корневой объект мира, ему принадлежат все гриды
  Object                    *thisGridObject;  //объек-хранитель всей информации об этом гриде
  WorldGridPosition         position;
  WorldGridActiveObjectList activeObjects;    //список активных объектов, запрещающих гриду выгрузиться. грид должен сам следить за этим списком при добавлении/удалении объектов

public:
  WorldGrid( const Pos< short >& newPosition , Object *rootObject );
  virtual ~WorldGrid();

};
