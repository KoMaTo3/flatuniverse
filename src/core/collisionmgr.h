#pragma once

#include "collision.h"
#include <vector>


typedef std::vector< Collision* > CollisionList;
extern CollisionList *__collisionList;


//частота обновления
#define COLLISION_UPDATE_FREQUENCY ( 1.0f / 100.0f )


struct CollisionPair {
  Collision
    *platform,
    *target;

  CollisionPair( Collision* setPlatform, Collision *setTarget )
  :platform( setPlatform ), target( setTarget )
  { }
};
typedef std::vector< CollisionPair > CollisionPairList;


class CollisionManager
{
private:
  float time;

public:
  CollisionManager();
  virtual ~CollisionManager();

  int   Update        ( float dt );

  bool  CheckDistance ( const Collision &item0, const Collision &item1 );

  typedef void OnUpdateCallbackProc();
  OnUpdateCallbackProc *OnUpdateCallback;
};
