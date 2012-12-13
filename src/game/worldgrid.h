/*
  ��������� �������� �����:
  |���| :rootGridObject
  |���|  - gridr#   <= ��� # - ����� ������ �����
  |���|   - gridc#  <= ��� # - ����� ������� �����
  ����� ���������:
  ���� ��� �� ����
  � ����� ���� deque-������ ��������

  ������ �������� �����:
    - ���������� �������� �������� (activeObjects)
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
        x,  //�������
        y;  //������
  };
  union WorldGridPosition
  {
    WorldGridCoordinate pos;
    long longPos;
  };

private:
  WorldGridPosition         position;   //���������� ����
  WorldGridObjectList       objects;    //��� ������� �����
  //WorldGridObjectList activeObjects;    //������ �������� ��������, ����������� ����� �����������. ���� ������ ��� ������� �� ���� ������� ��� ����������/�������� ��������

public:
  WorldGrid( const Pos< short >& newPosition );
  virtual ~WorldGrid();

};
