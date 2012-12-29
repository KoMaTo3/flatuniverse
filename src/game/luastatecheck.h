#pragma once

struct lua_State;


class LuaStateCheck
{
private:
  lua_State *luaState;
  int parmsCount;

public:
  LuaStateCheck( lua_State *setLuaState );
  virtual ~LuaStateCheck();
};
