#include "worldgrid.h"
#include "core/tools.h"
#include "core/file.h"


WorldGridList *__worldGridList = NULL;



/*
  [in] rootObject: объект, в котором будет храниться грид
*/
WorldGrid::WorldGrid( const Pos< short >& newPosition )
{
  WorldGridList::iterator iter, iterEnd = __worldGridList->end();

  bool finded = false;
  for( iter = __worldGridList->begin(); iter != iterEnd; ++iter )
    if( ( *iter )->position.pos.x == newPosition.x && ( *iter )->position.pos.y == newPosition.y )
    {
      finded = true;
      break;
    }
  if( finded )
    __log.PrintInfo( Filelevel_ERROR, "WorldGrid constructor: grid[%d; %d] already exists" );
  else
  {
    __log.PrintInfo( Filelevel_DEBUG, "WorldGrid: new grid [%d; %d]", newPosition.x, newPosition.y );
    this->position.pos.x = newPosition.x;
    this->position.pos.y = newPosition.y;
  }
}//constructor



WorldGrid::~WorldGrid()
{
  __log.PrintInfo( Filelevel_DEBUG, "WorldGrid deleted: pos[%d; %d]", this->position.pos.x, this->position.pos.y );
}//destructor
