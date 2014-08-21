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


#define WORLD_GRID_UPDATE_INTERVAL        ( 1.0f / 10.0f )  //период обновления состояния гридов
#define WORLD_GRID_LOADER_UPDATE_INTERVAL ( 1.0f / 50.0f )  //интервал загрузки объектов грида из дампа


namespace World {


class GridManager
{
private:
  Object*             rootGridObject;     //корневой _для_всех_гридов_ объект
  float               currentTime;        //время, прошедшее с момента последнего обновления
  float               currentTimeLoader;  //время подгрузки объектов из дампа
  World::GridObjectList activeObjects;      //перечень активных объектов
  //Pos< Short>         blocksPerGrid;      //число блоков в одном гриде
  //float               blockSize;          //размер одного блока в гриде
  float               gridSize;           //размер грида в пикселях
  Short               gridsAroundObject;  //число гридов, которые должны быть активны, в каждую сторону от объекта
  World::Saver        worldSaver;         //хранилище мира
  Dword               version;            //версия загруженного файла

public:
  GridManager( Object* newRootGridObject, Short setGridsAroundObject = 2, float setGridSize = 1024.0f );
  virtual ~GridManager();

  void        Update                  ( bool forceLoadGrids = false );
  void        AddActiveObject         ( Object *obj, const bool forceLoadGrid = false );
  void        RemoveActiveObject      ( Object *obj );
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
