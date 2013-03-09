/*
  Мысли в слух.
  Весь мир - набор гридов (WorldGrid)
  Нормальное состояние - минимум активных гридов.
  Все гриды должны выгружаться при первой возможности.
  У грида есть список активных объектов.
  - Список акт-х объектов формируется - неизвестно как
  Грид проверяет, нет ли в своих активных объектах вышедших за пределы:
  - если есть - переместить объект в другой грид. если грида нет - загрузить
  Грид, видимо, сам должен управлять объектами, в том числе и загрузкой-выгрузкой
*/

#pragma once

#include "worldgrid.h"
#include "worldsaver.h"


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 60.0f )    //период обновления состояния гридов



class WorldGridManager
{
private:
  Object*             rootGridObject;     //корневой _для_всех_гридов_ объект
  float               currentTime;        //время, прошедшее с момента последнего обновления
  WorldGridObjectList activeObjects;      //перечень активных объектов
  //Pos< Short>         blocksPerGrid;      //число блоков в одном гриде
  //float               blockSize;          //размер одного блока в гриде
  float               gridSize;           //размер грида в пикселях
  Short               gridsAroundObject;  //число гридов, которые должны быть активны, в каждую сторону от объекта
  WorldSaver          worldSaver;         //хранилище мира

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