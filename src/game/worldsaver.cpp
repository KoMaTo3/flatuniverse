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
  {
    memory tmpData( this->worldData );
    this->worldData.realloc( tmpData.getLength() + ( needBlocksCount - this->freeBlocks.size() ) * this->blockSize );
    __log.PrintInfo( Filelevel_DEBUG, ". realloc grid data by %d bytes", this->worldData.getLength() - tmpData.getLength() );
    memcpy( this->worldData.getData(), tmpData.getData(), tmpData.getLength() );
  }
  while( this->freeBlocks.size() < needBlocksCount )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". added new free block %d", this->freeBlocks.size() );
    this->freeBlocks.push_back( this->freeBlocks.size() );
  }

  Dword blockNum;
  Dword bufferPos = 0;
  char *bufferSrc = data.getData();
  void *bufferDest;
  Grid *grid = new Grid();
  grid->x = x;
  grid->y = y;
  this->grids.push_back( grid );
  for( Dword q = 0; q < needBlocksCount; ++q )
  {
    blockNum = *this->freeBlocks.rbegin();
    bufferDest = this->worldData.getData() + blockNum * this->blockSize;
    this->freeBlocks.pop_back();

    grid->blocksNums.push_back( blockNum );
    memcpy( bufferDest, bufferSrc, this->blockSize );
    bufferSrc += this->blockSize;
  }
}//SaveGrid




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
    false;
  }

  Dword blocksNum = grid->blocksNums.size();
  __log.PrintInfo( Filelevel_DEBUG, ". blocks per grid %d", blocksNum );
  data.realloc( blocksNum * this->blockSize );
  char *bufferDest = data.getData();
  char *bufferSrc = this->worldData.getData();
  for( Dword q = 0; q < blocksNum; ++q )
  {
    memcpy( bufferDest, bufferSrc + grid->blocksNums[ q ] * this->blockSize, this->blockSize );
    bufferDest += this->blockSize;
  }

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
    __log.PrintInfo( Filelevel_DEBUG, "WorldSaver::FreeGrid => added free greed %d", *iter );
    this->freeBlocks.push_back( *iter );
  }

  grid->blocksNums.clear();
  delete grid;
  this->grids.erase( gridIter );
}//FreeGrid
