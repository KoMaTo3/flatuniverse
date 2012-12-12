#pragma once

#include "core/object.h"


class WorldGrid
{
private:
  Object  *rootGridObject;  //корневой объект грида, ему принадлежат ВСЕ объекты грида
  struct WorldGridCoordinate
  {
    Short x, y;
  };
  union WorldGridPosition
  {
    WorldGridCoordinate pos;
    long longPos;
  };
public:
  WorldGrid();
  virtual ~WorldGrid();

};
