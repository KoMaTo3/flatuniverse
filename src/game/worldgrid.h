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
#include "core/objectpointer.h"
#include <deque>


class WorldGrid;
typedef ObjectPointer< Object > ObjectPointerType;
typedef std::deque< WorldGrid* > WorldGridList;
typedef std::deque< ObjectPointerType > WorldGridObjectList;
extern WorldGridList *__worldGridList;

#define WORLD_GRID_BLOCK_SIZE ( 16 )


class WorldGrid
{
public:
  struct WorldGridPosition
  {
    Short
        x,  //�������
        y;  //������
    bool operator==( const WorldGridPosition& item ) const
    {
      return this->x == item.x && this->y == item.y;
    }
    WorldGridPosition( Short newX, Short newY )
      :x( newX ), y( newY )
    { }
  };

private:
  WorldGridPosition         position;   //���������� ����
  WorldGridObjectList       objects;    //��� ������� �����
  //WorldGridObjectList activeObjects;    //������ �������� ��������, ����������� ����� �����������. ���� ������ ��� ������� �� ���� ������� ��� ����������/�������� ��������

public:
  WorldGrid( const WorldGrid::WorldGridPosition& newPosition );
  virtual ~WorldGrid();

  void  AttachObject( Object *object );
  void  DetachObject( Object *object );
  bool  IsThisObject( Object *object );
  const WorldGridPosition&
        GetPosition () { return this->position; }
  bool  GetGridDump ( FU_OUT memory& dump );
  bool  LoadFromDump( FU_IN memory& dump, Object *rootObject );

  void  Update();
};
