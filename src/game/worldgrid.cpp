#include "worldgrid.h"
#include "core/tools.h"
#include "core/file.h"


WorldGridList *__worldGridList = NULL;



/*
  [in] rootObject: объект, в котором будет храниться грид
*/
WorldGrid::WorldGrid( const WorldGrid::WorldGridPosition &newPosition )
:position( newPosition )
{
  WorldGridList::iterator iter, iterEnd = __worldGridList->end();

  bool finded = false;
  for( iter = __worldGridList->begin(); iter != iterEnd; ++iter )
    if( ( *iter )->position.x == newPosition.x && ( *iter )->position.y == newPosition.y )
    {
      finded = true;
      break;
    }
  if( finded )
    __log.PrintInfo( Filelevel_ERROR, "WorldGrid constructor: grid[%d; %d] already exists" );
  else
    __log.PrintInfo( Filelevel_DEBUG, "WorldGrid: new grid [%d; %d]", newPosition.x, newPosition.y );
}//constructor



WorldGrid::~WorldGrid()
{
  WorldGridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter )
    delete *iter;
  this->objects.clear();

  __log.PrintInfo( Filelevel_DEBUG, "WorldGrid deleted: pos[%d; %d]", this->position.x, this->position.y );
}//destructor



/*
=============
  AttachObject
=============
*/
void WorldGrid::AttachObject( Object *object )
{
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGrid::AttachObject => object is NULL" );
    return;
  }

  if( this->IsThisObject( object ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGrid::AttachObject => object already in this grid" );
    return;
  }

  //детачим от других гридов
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
  {
    if( ( *iterGrid )->IsThisObject( object ) )
    {
      ( *iterGrid )->DetachObject( object );
      break;
    }
  }//for iterGrid

  this->objects.push_back( object );
  __log.PrintInfo( Filelevel_DEBUG, "WorldGrid::AttachObject => object x%X attached to grid[%d; %d]", object, this->position.x, this->position.y );
}//AttachObject



/*
=============
  DetachObject
=============
*/
void WorldGrid::DetachObject( Object *object )
{
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGrid::AttachObject => object is NULL" );
    return;
  }

  WorldGridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter )
    if( ( *iter ) == object )
    {
      this->objects.erase( iter );
      break;
    }
}//DetachObject



/*
=============
  IsThisObject
=============
*/
bool WorldGrid::IsThisObject( Object *object )
{
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGrid::AttachObject => object is NULL" );
    return false;
  }

  WorldGridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter )
    if( ( *iter ) == object )
      return true;

  return false;
}//IsThisObject
