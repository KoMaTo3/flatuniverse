/*
  ����� � ����.
  ���� ��� - ����� ������ (WorldGrid)
  ���������� ��������� - ������� �������� ������.
  ��� ����� ������ ����������� ��� ������ �����������.
  � ����� ���� ������ �������� ��������.
  - ������ ���-� �������� ����������� - ���������� ���
  ���� ���������, ��� �� � ����� �������� �������� �������� �� �������:
  - ���� ���� - ����������� ������ � ������ ����. ���� ����� ��� - ���������
  ����, ������, ��� ������ ��������� ���������, � ��� ����� � ���������-���������
*/

#pragma once

#include "worldgrid.h"


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 10.0f )    //������ ���������� ��������� ������



class WorldGridManager
{
private:
  const Object        *rootObject;    //�������� ������. � �� �������� gridc#/gridr#...
  float               currentTime;    //�����, ��������� � ������� ���������� ����������
  WorldGridObjectList activeObjects;  //�������� �������� ��������

public:
  WorldGridManager( const Object *newRootObject );
  virtual ~WorldGridManager();

  void        Update            ();
  void        AddActiveObject   ( Object *obj );
  void        AttachObjectToGrid( const WorldGrid::WorldGridPosition& gridPos, Object *obj );
  WorldGrid*  LoadGrid          ( const WorldGrid::WorldGridPosition& gridPos );
  bool        UnloadGrid        ( const WorldGrid::WorldGridPosition& gridPos );
};