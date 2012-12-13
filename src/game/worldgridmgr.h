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


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 10.0f )    //период обновления состояния гридов



class WorldGridManager
{
private:
  Object              *rootObject;    //корневой объект. в нём хранятся gridc#/gridr#...
  float               currentTime;    //время, прошедшее с момента последнего обновления
  WorldGridObjectList activeObjects;  //перечень активных объектов

public:
  WorldGridManager( Object *newRootObject );
  virtual ~WorldGridManager();

  void Update();
  void AddActiveObject( Object *obj );
};