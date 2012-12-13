#include "worldgridmgr.h"
#include "core/tools.h"
#include "core/timer.h"
#include "core/file.h"



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



/*
=============
  AddActiveObject
=============
*/
void WorldGridManager::AddActiveObject( Object *obj )
{
  if( !obj )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::AddActiveObject => object is NULL" );
    return;
  }

  WorldGridObjectList::iterator iter, iterEnd = this->activeObjects.end();

  for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter )
    if( ( *iter ) == obj )
    {
      __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::AddActiveObject => object '%s' already in list", obj->GetNameFull().c_str() );
      return;
    }

  this->activeObjects.push_back( obj );
  __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::AddActiveObject => object '%s' added to list", obj->GetNameFull().c_str() );
}//AddActiveObject
