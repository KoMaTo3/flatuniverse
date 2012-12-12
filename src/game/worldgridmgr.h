#pragma once

#include "worldgrid.h"


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 10.0f )    //период обновления состояния гридов



class WorldGridManager
{
private:
  Object *rootObject; //корневой объект. в нём хранятся gridc#/gridr#...
  float currentTime;  //время, прошедшее с момента последнего обновления

public:
  WorldGridManager( Object *newRootObject );
  virtual ~WorldGridManager();

  void Update();
};