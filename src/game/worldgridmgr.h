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


#define WORLD_GRID_UPDATE_INTERVAL        ( 1.0f / 10.0f )  //������ ���������� ��������� ������
#define WORLD_GRID_LOADER_UPDATE_INTERVAL ( 1.0f / 50.0f )  //�������� �������� �������� ����� �� �����


namespace World {


class GridManager
{
private:
  Object*             rootGridObject;     //�������� _���_����_������_ ������
  float               currentTime;        //�����, ��������� � ������� ���������� ����������
  float               currentTimeLoader;  //����� ��������� �������� �� �����
  World::GridObjectList activeObjects;      //�������� �������� ��������
  //Pos< Short>         blocksPerGrid;      //����� ������ � ����� �����
  //float               blockSize;          //������ ������ ����� � �����
  float               gridSize;           //������ ����� � ��������
  Short               gridsAroundObject;  //����� ������, ������� ������ ���� �������, � ������ ������� �� �������
  World::Saver        worldSaver;         //��������� ����
  Dword               version;            //������ ������������ �����

public:
  GridManager( Object* newRootGridObject, Short setGridsAroundObject = 2, float setGridSize = 1024.0f );
  virtual ~GridManager();

  void        Update                  ( bool forceLoadGrids = false );
  void        AddActiveObject         ( Object *obj, const bool forceLoadGrid = false );
  void        AttachObjectToGrid      ( const World::Grid::Position& gridPos, Object *obj );
  void        AttachObjectToGrid      ( Short gridX, Short gridY, Object *obj );
  World::Grid*  IsGridLoaded            ( const World::Grid::Position& gridPos );
  World::Grid*  LoadGrid                ( const World::Grid::Position& gridPos, const bool forceLoad = false );
  bool        UnloadGrid              ( const World::Grid::Position gridPos, const bool forceUnload = false );
  World::Grid::Position
              GetGridPositionByObject ( const Object& obj );
  World::Grid::Position
              GetGridPositionByCoords ( const Vec3& coords );
  void        LoadFromFile            ( const std::string& fileName );
  void        SaveToFile              ( const std::string& fileName );
  void        ClearWorld              ();
  inline float  GetGridSize           () { return this->gridSize; }
};

};
