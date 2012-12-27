#pragma once

#include "core/core.h"
#include "core/object.h"
#include "worldgridmgr.h"
#include "lua.h"

class Lua;

class Game
{
public:
  struct
  {
    bool show;
    Object *obj[ 40 ];
  } gridTooltipUnderMouse;

  Core              *core;
  WorldGridManager  *world;
  Lua               *lua;

public:
  Game();
  ~Game();
  static void LUA_RemoveObject( const std::string &name );
  static Vec2 LUA_GetObjectPos( const std::string &name );

  Vec3 GetGridPosUnderCamera( float scale = 1.0f )
  {
    Vec3 pos( this->core->GetCamera()->GetPosition() );
    pos += Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0 ) * scale * 0.5f;
    pos /= float( WORLD_GRID_BLOCK_SIZE ) * scale;
    pos = Vec3( Math::Floor( pos.x ), Math::Floor( pos.y ), 0.0f );
    return pos;
  }

  Vec3 GetGridPosUnderCursor()
  {
    Vec3 pos( this->core->mouse.GetCursorPosition().x, this->core->mouse.GetCursorPosition().y, 0.0f );
    pos += ( this->core->GetCamera()->GetPosition() - Vec3( this->core->GetWindowHalfSize().x, this->core->GetWindowHalfSize().y, 0.0f ) ) + Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0 ) * 0.5f;
    pos /= float( WORLD_GRID_BLOCK_SIZE );
    pos = Vec3( Math::Floor( pos.x ), Math::Floor( pos.y ), 0.0f );
    return pos;
  }
};
