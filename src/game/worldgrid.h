/*
  ��������� �������� �����:
  :rootGridObject
   - gridr#   <= ��� # - ����� ������ �����
    - gridc#  <= ��� # - ����� ������� �����

  ������ �������� �����:
    - ���������� �������� �������� (activeObjects)
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
  Object                    *rootGridObject;  //�������� ������ ����, ��� ����������� ��� �����
  Object                    *thisGridObject;  //�����-��������� ���� ���������� �� ���� �����
  WorldGridPosition         position;
  WorldGridActiveObjectList activeObjects;    //������ �������� ��������, ����������� ����� �����������. ���� ������ ��� ������� �� ���� ������� ��� ����������/�������� ��������

public:
  WorldGrid( const Pos< short >& newPosition , Object *rootObject );
  virtual ~WorldGrid();

};
