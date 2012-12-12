#pragma once

#include "collision.h"
#include <vector>


typedef std::vector< Collision* > CollisionList;
extern CollisionList *__collisionList;


//частота обновления
#define COLLISION_UPDATE_FREQUENCY ( 1.0f / 100.0f )



class CollisionManager
{
private:
  float time;

public:
  CollisionManager();
  virtual ~CollisionManager();

  bool  Update        ( float dt );

  bool  CheckDistance ( const Collision &item0, const Collision &item1 );
};
