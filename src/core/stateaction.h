#pragma once



enum StateActionType {
  STATE_ACTION_TYPE_ADD,
  STATE_ACTION_TYPE_REPLACE,
  STATE_ACTION_TYPE_REMOVE,
};



class StateAction {
public:

private:
  StateActionType actionType;

};