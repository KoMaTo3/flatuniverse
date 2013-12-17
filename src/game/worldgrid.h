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
        x,  //�������
        y;  //������
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
  WorldGridPosition         position;       //���������� ����
  WorldGridObjectList       objects;        //��� ������� �����
  Dword                     version;        //������
  //bool                      isFullyLoaded;  //���� ������ ������������� ����� �� �����
  WORLDGRID_STATE           state;
  struct {
    memory                  dump;
    MemoryReader            reader;
    Dword                   objectsCount;
    Object                  *rootObject;
  } loader;
  //WorldGridObjectList activeObjects;    //������ �������� ��������, ����������� ����� �����������. ���� ������ ��� ������� �� ���� ������� ��� ����������/�������� ��������

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
