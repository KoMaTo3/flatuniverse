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
  float               currentTime;        //�����, ��������� � ������� ���������� ����������
  WorldGridObjectList activeObjects;      //�������� �������� ��������
  Pos< Short>         blocksPerGrid;      //����� ������ � ����� �����
  float               blockSize;          //������ ������ ����� � �����
  Short               gridsAroundObject;  //����� ������, ������� ������ ���� �������, � ������ ������� �� �������

public:
  WorldGridManager();
  virtual ~WorldGridManager();

  void        Update                  ();
  void        AddActiveObject         ( Object *obj );
  void        AttachObjectToGrid      ( const WorldGrid::WorldGridPosition& gridPos, Object *obj );
  void        AttachObjectToGrid      ( Short gridX, Short gridY, Object *obj );
  WorldGrid*  IsGridLoaded            ( const WorldGrid::WorldGridPosition& gridPos );
  WorldGrid*  LoadGrid                ( const WorldGrid::WorldGridPosition& gridPos );
  bool        UnloadGrid              ( const WorldGrid::WorldGridPosition& gridPos );
  WorldGrid::WorldGridPosition
              GetGridPositionByObject ( const Object& obj );
};