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
#include "worldsaver.h"


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 60.0f )    //������ ���������� ��������� ������



class WorldGridManager
{
private:
  Object*             rootGridObject;     //�������� _���_����_������_ ������
  float               currentTime;        //�����, ��������� � ������� ���������� ����������
  WorldGridObjectList activeObjects;      //�������� �������� ��������
  Pos< Short>         blocksPerGrid;      //����� ������ � ����� �����
  float               blockSize;          //������ ������ ����� � �����
  Short               gridsAroundObject;  //����� ������, ������� ������ ���� �������, � ������ ������� �� �������
  WorldSaver          worldSaver;         //��������� ����

public:
  WorldGridManager( Object* newRootGridObject, Short setGridsAroundObject = 2, Pos< Short> setBlocksPerGrid = Pos< Short>( 8, 8 ) );
  virtual ~WorldGridManager();

  void        Update                  ( bool forceLoadGrids = false );
  void        AddActiveObject         ( Object *obj );
  void        AttachObjectToGrid      ( const WorldGrid::WorldGridPosition& gridPos, Object *obj );
  void        AttachObjectToGrid      ( Short gridX, Short gridY, Object *obj );
  WorldGrid*  IsGridLoaded            ( const WorldGrid::WorldGridPosition& gridPos );
  WorldGrid*  LoadGrid                ( const WorldGrid::WorldGridPosition& gridPos );
  bool        UnloadGrid              ( const WorldGrid::WorldGridPosition gridPos );
  WorldGrid::WorldGridPosition
              GetGridPositionByObject ( const Object& obj );
  void        LoadFromFile            ( const std::string& fileName );
  void        SaveToFile              ( const std::string& fileName );
  void        ClearWorld              ();
};