#pragma once

#include "klib.h"



class State {
public:
  State         ( const Dword setId, const float setLifeTime, const int setStacks = 1, const float setSpeed = 1.0f );
  virtual ~State();
  bool    Update( float dt );

private:
  Dword id;
  int   stacks;       //число "стаков" стейта
  float lifeTime,     //текущее время жизни, идёт от maxLifeTime до 0
        maxLifeTime;  //время жизни стейта. либо спадает стак, либо весь стейт
  float speed;        //скорость течения жизни стейта. 1 - норма, 0 - стейт бесконечен, >1 - ускорен, [0;1] - замедлен
};