#pragma once

#include "core/object.h"


class WorldGrid
{
private:
  Object  *rootGridObject;  //�������� ������ �����, ��� ����������� ��� ������� �����
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
