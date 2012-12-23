#include "worldsaver.h"
#include "core/file.h"




WorldSaver::WorldSaver()
:blockSize( 1024 )
{
}//constructor




/*
=============
  SaveGrid
=============
*/
void WorldSaver::SaveGrid( Short x, Short y, memory& data )
{
  __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveGrid => pos[%d; %d] dataLength[%d]", x, y, data.getLength() );
  if( this->GridExists( x, y ) )
    this->FreeGrid( x, y );

  if( !data.getLength() )
  {
    __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveGrid => grid clear" );
    return;
  }

  Dword needBlocksCount = data.getLength() / this->blockSize;
  if( this->freeBlocks.size() < needBlocksCount )
    this->AllocFreeBlocks( needBlocksCount - this->freeBlocks.size() );
  /*
  if( this->freeBlocks.size() < needBlocksCount )
  {
    memory tmpData( this->worldData );
    this->worldData.realloc( tmpData.getLength() + ( needBlocksCount - this->freeBlocks.size() ) * this->blockSize );
    __log.PrintInfo( Filelevel_DEBUG, ". realloc grid data by %d bytes", this->worldData.getLength() - tmpData.getLength() );
    memcpy( this->worldData.getData(), tmpData.getData(), tmpData.getLength() );
  }
  */
  /*
  Dword newIndex;
  while( this->freeBlocks.size() < needBlocksCount )
  {
    newIndex = this->freeBlocks.size() + this->worldData.getLength() / this->blockSize;
    __log.PrintInfo( Filelevel_DEBUG, ". added new free block index[%d]", newIndex );
    this->freeBlocks.push_back( newIndex );
  }
  */

  Dword blockNum;
  Dword bufferPos = 0;
  char *bufferSrc = data.getData();
  Grid *grid = new Grid();
  grid->x = x;
  grid->y = y;
  this->grids.push_back( grid );
  for( Dword q = 0; q < needBlocksCount; ++q )
  {
    blockNum = *this->freeBlocks.rbegin();
    this->freeBlocks.pop_back();

    grid->blocksNums.push_back( blockNum );
    memcpy( this->worldData.getData() + blockNum * this->blockSize, bufferSrc, this->blockSize );
    bufferSrc += this->blockSize;
  }
}//SaveGrid




/*
=============
  AllocFreeBlocks
=============
*/
void WorldSaver::AllocFreeBlocks( Dword blocksCount )
{
  if( this->worldData.getLength() )
  {
    Dword startIndex = this->worldData.getLength() / this->blockSize;
    memory tmp( this->worldData );
    this->worldData.realloc( tmp.getLength() + blocksCount * this->blockSize );
    memcpy( this->worldData.getData(), tmp.getData(), tmp.getLength() );
    for( Dword q = 0; q < blocksCount; ++q )
    {
      this->freeBlocks.push_back( startIndex + q );
      __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::AllocFreeBlocks => index[%d]", startIndex + q );
    }
  }
  else
  {
    this->worldData.realloc( blocksCount * this->blockSize );
    for( Dword q = 0; q < blocksCount; ++q )
    {
      this->freeBlocks.push_back( q );
      __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::AllocFreeBlocks => index[%d]", q );
    }
  }
}//AllocFreeBlocks




/*
=============
  LoadGrid
=============
*/
bool WorldSaver::LoadGrid( Short x, Short y, FU_OUT memory& data )
{
  __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::LoadGrid => [%d; %d]", x, y );
  data.free();
  Grid *grid = this->GridExists( x, y );
  if( !grid )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". grid not found in worldData" );
    return false;
  }

  Dword blocksNum = grid->blocksNums.size();
  __log.PrintInfo( Filelevel_DEBUG, ". blocks per grid %d", blocksNum );
  data.realloc( blocksNum * this->blockSize );
  char *bufferDest = data.getData();
  char *bufferSrc = this->worldData.getData();
  for( Dword q = 0; q < blocksNum; ++q )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". index[%d]", grid->blocksNums[ q ] );
    memcpy( bufferDest, bufferSrc + grid->blocksNums[ q ] * this->blockSize, this->blockSize );
    bufferDest += this->blockSize;
  }
  __log.PrintInfo( Filelevel_DEBUG, ". loaded" );

  this->FreeGrid( x, y );

  return true;
}//LoadGrid





/*
=============
  GridExists
=============
*/
WorldSaver::Grid* WorldSaver::GridExists( Short x, Short y )
{
  GridsList::iterator iter, iterEnd = this->grids.end();
  for( iter = this->grids.begin(); iter != iterEnd; ++iter )
    if( ( *iter )->x == x && ( *iter )->y == y )
      return *iter;
  return NULL;
}//GridExists




/*
=============
  FreeGrid
=============
*/
void WorldSaver::FreeGrid( Short x, Short y )
{
  __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::FreeGrid => [%d; %d]", x, y );
  Grid *grid = NULL;
  GridsList::iterator gridIter, gridIterEnd = this->grids.end();
  for( gridIter = this->grids.begin(); gridIter != gridIterEnd; ++gridIter )
    if( ( *gridIter )->x == x && ( *gridIter )->y == y )
    {
      grid = *gridIter;
      break;
    }
  if( !grid )
    return;

  GridNumList::iterator iter, iterEnd = grid->blocksNums.end();
  for( iter = grid->blocksNums.begin(); iter != iterEnd; ++iter )
  {
    __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::FreeGrid => added free grid %d", *iter );
    this->freeBlocks.push_back( *iter );
  }

  grid->blocksNums.clear();
  delete grid;
  this->grids.erase( gridIter );
}//FreeGrid
