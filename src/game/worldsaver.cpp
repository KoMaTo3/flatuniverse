#include "worldsaver.h"



/*
=============
  SaveGrid
=============
*/
void WorldSaver::SaveGrid( Short x, Short y, memory& data )
{
  if( this->GridExists( x, y ) )
    this->FreeGrid( x, y );
}//SaveGrid




/*
=============
  GridExists
=============
*/
WorldSaver::Grid* WorldSaver::GridExists( Short x, Short y )
{
  GridsList::iterator iter, iterEnd = this->grids.end();
  for( iter = this->grids.begin(); iter != iterEnd; ++iter )
    if( iter->x == x && iter->y == y )
      return &( *iter );
  return NULL;
}//GridExists




/*
=============
  FreeGrid
=============
*/
void WorldSaver::FreeGrid( Short x, Short y )
{
  Grid *grid = NULL;
  GridsList::iterator gridIter, gridIterEnd = this->grids.end();
  for( gridIter = this->grids.begin(); gridIter != gridIterEnd; ++gridIter )
    if( gridIter->x == x && gridIter->y == y )
    {
      grid = &( *gridIter );
      break;
    }
  if( !grid )
    return;

  GridNumList::iterator iter, iterEnd = grid->blocksNums.end();
  for( iter = grid->blocksNums.begin(); iter != iterEnd; ++iter )
    this->freeBlocks.push_back( *iter );

  this->grids.erase( gridIter );
}//FreeGrid
