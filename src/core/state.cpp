#include "state.h"


State::State( const Dword setId, const float setLifeTime, const int setStacks, const float setSpeed )
:id( setId ), stacks( setStacks ), lifeTime( setLifeTime ), maxLifeTime( setLifeTime ), speed( setSpeed )
{
}//constructor



State::~State() {
}//destructor



/*
=============
  Update
=============
*/
bool State::Update( float dt ) {
  this->lifeTime -= dt * this->speed;
  if( this->lifeTime <= 0.0f ) {
    --this->stacks;
    this->lifeTime += this->maxLifeTime;
    if( this->stacks < 0 ) {
      return false;
    }
  }
  return true;
}//Update
