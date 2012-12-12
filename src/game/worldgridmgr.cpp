#include "worldgridmgr.h"
#include "core/tools.h"
#include "core/timer.h"



WorldGridManager::WorldGridManager( Object *newRootObject )
:rootObject( newRootObject ), currentTime( 0.0f )
{
  if( !this->rootObject )
    __log.PrintInfo( Filelevel_ERROR, "WorldGridManager initialization => rootObject is NULL" );

  if( !__worldGridList )
    __worldGridList = new WorldGridList();
}//constructor



WorldGridManager::~WorldGridManager()
{
  DEF_DELETE( __worldGridList );
}//destructor




/*
=============
  Update
=============
*/
void WorldGridManager::Update()
{
  this->currentTime += sTimer.GetDeltaF();
  while( this->currentTime > WORLD_GRID_UPDATE_INTERVAL )
  {
    __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update" );

    WorldGridList::iterator iter, iterEnd = __worldGridList->end();
    for( iter = __worldGridList->begin(); iter != iterEnd; ++iter )
    {
    }//for __worldGridList

    this->currentTime -= WORLD_GRID_UPDATE_INTERVAL;
  }//while time
}//Update
