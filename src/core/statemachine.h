#pragma once

#include "state.h"
#include "stateaction.h"
#include <deque>


class StateMachine {
public:
  StateMachine();
  virtual ~StateMachine();
  void    Apply( const StateAction &action );

private:
  std::deque< State* > states;

};
