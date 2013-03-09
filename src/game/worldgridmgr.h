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
  //Pos< Short>         blocksPerGrid;      //����� ������ � ����� �����
  //float               blockSize;          //������ ������ ����� � �����
  float               gridSize;           //������ ����� � ��������
  Short               gridsAroundObject;  //����� ������, ������� ������ ���� �������, � ������ ������� �� �������
  WorldSaver          worldSaver;         //��������� ����

public:
  WorldGridManager( Object* newRootGridObject, Short setGridsAroundObject = 2, float setGridSize = 1024.0f );
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
  WorldGrid::WorldGridPosition
              GetGridPositionByCoords ( const Vec3& coords );
  void        LoadFromFile            ( const std::string& fileName );
  void        SaveToFile              ( const std::string& fileName );
  void        ClearWorld              ();
  inline float  GetGridSize           () { return this->gridSize; }
};