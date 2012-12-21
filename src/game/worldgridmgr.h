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


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 10.0f )    //период обновления состояния гридов



class WorldGridManager
{
private:
  Object*             rootGridObject;     //корневой _для_всех_гридов_ объект
  float               currentTime;        //время, прошедшее с момента последнего обновления
  WorldGridObjectList activeObjects;      //перечень активных объектов
  Pos< Short>         blocksPerGrid;      //число блоков в одном гриде
  float               blockSize;          //размер одного блока в гриде
  Short               gridsAroundObject;  //число гридов, которые должны быть активны, в каждую сторону от объекта
  WorldSaver          worldSaver;         //хранилище мира

public:
  WorldGridManager( Object* newRootGridObject );
  virtual ~WorldGridManager();

  void        Update                  ();
  void        AddActiveObject         ( Object *obj );
  void        AttachObjectToGrid      ( const WorldGrid::WorldGridPosition& gridPos, Object *obj );
  void        AttachObjectToGrid      ( Short gridX, Short gridY, Object *obj );
  WorldGrid*  IsGridLoaded            ( const WorldGrid::WorldGridPosition& gridPos );
  WorldGrid*  LoadGrid                ( const WorldGrid::WorldGridPosition& gridPos );
  bool        UnloadGrid              ( const WorldGrid::WorldGridPosition gridPos );
  WorldGrid::WorldGridPosition
              GetGridPositionByObject ( const Object& obj );
};