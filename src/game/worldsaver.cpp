#include "worldsaver.h"
#include "core/file.h"
#include "core/filemanager.h"
#include "core/memorywriter.h"
#include "core/memoryreader.h"




WorldSaver::WorldSaver()
  :blockSize( 1024 ), fileName( "" )
{
}//constructor




/*
=============
  SaveGrid
=============
*/
void WorldSaver::SaveGrid( Short x, Short y, memory& data )
{
  //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveGrid => pos[%d; %d] dataLength[%d]", x, y, data.getLength() );
  if( this->GridExists( x, y ) ) {
    this->FreeGrid( x, y );
  }

  if( !data.getLength() ) {
    //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveGrid => grid clear" );
    return;
  }

  Dword needBlocksCount = data.getLength() / this->blockSize;
  if( this->freeBlocks.size() < needBlocksCount ) {
    this->AllocFreeBlocks( needBlocksCount - this->freeBlocks.size() );
  }
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
  grid->version = WOLDSAVER_FILE_VERSION;
  this->grids.push_back( grid );
  for( Dword q = 0; q < needBlocksCount; ++q )
  {
    blockNum = *this->freeBlocks.rbegin();
    this->freeBlocks.pop_back();

    grid->blocksNums.push_back( blockNum );
    memcpy( this->worldData.getData() + blockNum * this->blockSize, bufferSrc, this->blockSize );
    bufferSrc += this->blockSize;
  }

  //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveGrid => done" );
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
    this->worldData.Alloc( tmp.getLength() + blocksCount * this->blockSize );
    memcpy( this->worldData.getData(), tmp.getData(), tmp.getLength() );
    for( Dword q = 0; q < blocksCount; ++q )
    {
      this->freeBlocks.push_back( startIndex + q );
      //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::AllocFreeBlocks => index[%d]", startIndex + q );
    }
  }
  else
  {
    this->worldData.Alloc( blocksCount * this->blockSize );
    for( Dword q = 0; q < blocksCount; ++q )
    {
      this->freeBlocks.push_back( q );
      //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::AllocFreeBlocks => index[%d]", q );
    }
  }
}//AllocFreeBlocks




/*
=============
  LoadGrid
=============
*/
bool WorldSaver::LoadGrid( Short x, Short y, FU_OUT memory& data, FU_OUT Dword &version )
{
  //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::LoadGrid => [%d; %d]", x, y );
  data.Free();
  Grid *grid = this->GridExists( x, y );
  if( !grid ) {
    //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::LoadGrid => grid not found in worldData" );
    return false;
  }

  Dword blocksNum = grid->blocksNums.size();
  //__log.PrintInfo( Filelevel_DEBUG, ". blocks per grid %d, version x%X", blocksNum, grid->version );
  data.Realloc( blocksNum * this->blockSize );
  char *bufferDest = data.getData();
  char *bufferSrc = this->worldData.getData();
  for( Dword q = 0; q < blocksNum; ++q )
  {
    //__log.PrintInfo( Filelevel_DEBUG, ". index[%d]", grid->blocksNums[ q ] );
    memcpy( bufferDest, bufferSrc + grid->blocksNums[ q ] * this->blockSize, this->blockSize );
    bufferDest += this->blockSize;
  }
  version = grid->version;
  //__log.PrintInfo( Filelevel_DEBUG, ". loaded" );

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
  for( iter = this->grids.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->x == x && ( *iter )->y == y ) {
      return *iter;
    }
  }
  return NULL;
}//GridExists




/*
=============
  FreeGrid
=============
*/
void WorldSaver::FreeGrid( Short x, Short y )
{
  //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::FreeGrid => [%d; %d]", x, y );
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
    //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::FreeGrid => added free grid %d", *iter );
    this->freeBlocks.push_back( *iter );
  }

  grid->blocksNums.clear();
  delete grid;
  this->grids.erase( gridIter );
}//FreeGrid



/*
=============
  SaveToFile
=============
*/
void WorldSaver::SaveToFile( const std::string& fileName )
{
  std::string resultFileName = ( fileName.empty() ? this->fileName : fileName );
  File f;
  if( f.Open( resultFileName, File_mode_WRITE ) ) {
    return;
  }

  f.Write( &WOLDSAVER_FILE_VERSION, sizeof( WOLDSAVER_FILE_VERSION ) );
  f.Write( this->worldData.getData(), this->worldData.getLength() );

  memory header;
  MemoryWriter headerWriter( header, 1 );

  //свободные блоки
  Dword freeBlocksCount = this->freeBlocks.size();
  headerWriter << freeBlocksCount;
  GridNumList::iterator iterFree, iterFreeEnd = this->freeBlocks.end();
  for( iterFree = this->freeBlocks.begin(); iterFree != iterFreeEnd; ++iterFree ) {
    headerWriter << *iterFree;
  }

  //гриды
  Dword gridsCount = this->grids.size();
  headerWriter << gridsCount;
  GridsList::iterator iter, iterEnd = this->grids.end();
  for( iter = this->grids.begin(); iter != iterEnd; ++iter )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::SaveToFile => grid[%d; %d] version[x%X]", ( *iter )->x, ( *iter )->y, ( *iter )->version );
    headerWriter << ( *iter )->version;
    headerWriter << ( *iter )->x;
    headerWriter << ( *iter )->y;
    Dword blocksCount = ( *iter )->blocksNums.size();
    headerWriter << blocksCount;
    for( Dword blockNum = 0; blockNum < blocksCount; ++blockNum ) {
      headerWriter << ( *iter )->blocksNums[ blockNum ];
    }
  }

  //заголовок и его длина
  Dword headerLength = header.getLength();
  f.Write( header.getData(), headerLength );
  f.Write( &headerLength, sizeof( headerLength ) );

  f.Close();
}//SaveToFile



/*
=============
  LoadFromFile
=============
*/
Dword WorldSaver::LoadFromFile( const std::string& fileName )
{
  memory data;
  if( !__fileManager->GetFile( fileName, data ) ) {
    File f;
    if( f.Open( fileName, File_mode_READ ) != aOK ) {
      return 0;
    }
    data.Alloc( f.GetLength() );
    f.Read( data.getData(), data.getLength(), 1 );
    f.Close();
  }
  //__log.PrintInfo( Filelevel_DEBUG, "WorldSaver::LoadFromFile => '%s'", fileName.c_str() );

  Dword headerLength, worldDataLength;
  MemoryReader headerReader( data );
  Dword version;
  headerReader >> version;
  //__log.PrintInfo( Filelevel_DEBUG, ". version[x%X]", version );
  if( version != WOLDSAVER_FILE_VERSION ) {
    //__log.PrintInfo( Filelevel_WARNING, "WorldSaver::LoadFromFile => file '%s' has bad version of data (x%X), needed version x%X", fileName.c_str(), version, WOLDSAVER_FILE_VERSION );
    //return 0;
  }

  headerReader.SeekFromStart( data.getLength() - sizeof( headerLength ) );
  headerReader >> headerLength;
  //__log.PrintInfo( Filelevel_DEBUG, ". headerLength[%d]", headerLength );
  worldDataLength = data.getLength() - sizeof( headerLength ) - headerLength - sizeof( WOLDSAVER_FILE_VERSION );
  headerReader.SeekFromStart( worldDataLength + sizeof( WOLDSAVER_FILE_VERSION ) );
  //__log.PrintInfo( Filelevel_DEBUG, ". worldDataLength[%d]", worldDataLength );

  this->worldData.Alloc( worldDataLength );
  memcpy( this->worldData.getData(), data.getData() + sizeof( WOLDSAVER_FILE_VERSION ), worldDataLength );

  //свободные блоки
  this->freeBlocks.clear();
  Dword freeBlocksCount;
  headerReader >> freeBlocksCount;
  //__log.PrintInfo( Filelevel_DEBUG, ". freeBlocksCount[%d]", freeBlocksCount );
  Dword blockNum;
  for( Dword q = 0; q < freeBlocksCount; ++q )
  {
    headerReader >> blockNum;
    this->freeBlocks.push_back( blockNum );
  }

  //гриды
  this->grids.clear();
  Dword gridsCount;
  headerReader >> gridsCount;
  //__log.PrintInfo( Filelevel_DEBUG, ". gridsCount[%d]", gridsCount );
  for( Dword q = 0; q < gridsCount; ++q )
  {
    Grid *grid = new Grid();
    if( version >= 0x00000004 ) {
      headerReader >> grid->version;
    } else {
      grid->version = version;
    }
    headerReader >> grid->x;
    headerReader >> grid->y;
    Dword blocksCount;
    headerReader >> blocksCount;
    //__log.PrintInfo( Filelevel_DEBUG, ". grid[%d; %d] version[x%X] blocks[%d]", grid->x, grid->y, grid->version, blocksCount );
    Dword blockNum;
    for( Dword w = 0; w < blocksCount; ++w ) {
      headerReader >> blockNum;
      //__log.PrintInfo( Filelevel_DEBUG, ". block[%d]", blockNum );
      grid->blocksNums.push_back( blockNum );
    }
    this->grids.push_back( grid );
  }
  this->fileName = fileName;

  return version;
}//LoadFromFile
