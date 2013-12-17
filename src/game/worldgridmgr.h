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


#define WORLD_GRID_UPDATE_INTERVAL        ( 1.0f / 10.0f )    //������ ���������� ��������� ������
#define WORLD_GRID_LOADER_UPDATE_INTERVAL ( 1.0f / 50.0f )   //�������� �������� �������� ����� �� �����



class WorldGridManager
{
private:
  Object*             rootGridObject;     //�������� _���_����_������_ ������
  float               currentTime;        //�����, ��������� � ������� ���������� ����������
  float               currentTimeLoader;  //����� ��������� �������� �� �����
  WorldGridObjectList activeObjects;      //�������� �������� ��������
  //Pos< Short>         blocksPerGrid;      //����� ������ � ����� �����
  //float               blockSize;          //������ ������ ����� � �����
  float               gridSize;           //������ ����� � ��������
  Short               gridsAroundObject;  //����� ������, ������� ������ ���� �������, � ������ ������� �� �������
  WorldSaver          worldSaver;         //��������� ����
  Dword               version;            //������ ������������ �����

public:
  WorldGridManager( Object* newRootGridObject, Short setGridsAroundObject = 2, float setGridSize = 1024.0f );
  virtual ~WorldGridManager();

  void        Update                  ( bool forceLoadGrids = false );
  void        AddActiveObject         ( Object *obj, const bool forceLoadGrid = false );
  void        AttachObjectToGrid      ( const WorldGrid::WorldGridPosition& gridPos, Object *obj );
  void        AttachObjectToGrid      ( Short gridX, Short gridY, Object *obj );
  WorldGrid*  IsGridLoaded            ( const WorldGrid::WorldGridPosition& gridPos );
  WorldGrid*  LoadGrid                ( const WorldGrid::WorldGridPosition& gridPos, const bool forceLoad = false );
  bool        UnloadGrid              ( const WorldGrid::WorldGridPosition gridPos, const bool forceUnload = false );
  WorldGrid::WorldGridPosition
              GetGridPositionByObject ( const Object& obj );
  WorldGrid::WorldGridPosition
              GetGridPositionByCoords ( const Vec3& coords );
  void        LoadFromFile            ( const std::string& fileName );
  void        SaveToFile              ( const std::string& fileName );
  void        ClearWorld              ();
  inline float  GetGridSize           () { return this->gridSize; }
};