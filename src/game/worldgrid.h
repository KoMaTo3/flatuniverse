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

namespace World {
  class Grid;

  typedef Pointer ObjectPointerType;
  typedef std::vector< Grid* > GridList;
  typedef std::vector< ObjectPointerType* > GridObjectList;
  extern GridList *__worldGridList;


//#define WORLD_GRID_BLOCK_SIZE ( 16 )
enum WORLDGRID_STATE {
  WORLDGRID_STATE_UNKNOWN,
  WORLDGRID_STATE_LOADING,
  WORLDGRID_STATE_DONE,
  WORLDGRID_STATE_UNLOADING,
};


class Grid
{
public:
  struct Position
  {
    Short
        x,  //�������
        y;  //������
    bool operator==( const Position& item ) const
    {
      return this->x == item.x && this->y == item.y;
    }
    Position( Short newX, Short newY )
      :x( newX ), y( newY )
    { }
    bool operator != ( const Position &with ) {
      return this->x != with.x || this->y != with.y;
    }
  };

private:
  Position                  position;       //���������� ����
  GridObjectList            objects;        //��� ������� �����
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
  Grid( const Position& newPosition );
  virtual ~Grid();

  void  AttachObject( Object *object );
  void  DetachObject( Object *object );
  void  DetachAll   ();
  bool  IsThisObject( Object *object );
  const Position&
        GetPosition () { return this->position; }
  bool  GetGridDump ( FU_OUT memory& dump );
  bool  LoadFromDump( FU_IN memory& dump, Object *rootObject, const Dword fileVersion, const bool forceLoad = false );
  bool  GetFirstMovableObject( GridObjectList::iterator &iter );
  bool  GetNextMovableObject( GridObjectList::iterator &iter );
  inline bool IsFullyLoaded() { return this->state == WORLDGRID_STATE_DONE || this->state == WORLDGRID_STATE_UNLOADING; }
  inline WORLDGRID_STATE GetState() { return this->state; }

  void  Update();

private:
  Grid();
  Grid( const Grid& );
  Grid& operator=( const Grid& );
};

};
