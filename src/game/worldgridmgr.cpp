#include "worldgridmgr.h"
#include "core/tools.h"
#include "core/timer.h"
#include "core/file.h"



WorldGridManager::WorldGridManager( Object* newRootGridObject, Short setGridsAroundObject, float setGridSize )
:currentTime( 0.0f ), gridSize( setGridSize ), gridsAroundObject( setGridsAroundObject ), rootGridObject( newRootGridObject )
{
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
void WorldGridManager::Update( bool forceLoadGrids )
{
  this->currentTime += sTimer.GetDeltaF();
  while( this->currentTime > WORLD_GRID_UPDATE_INTERVAL )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update" );

    //Update grids
    WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
    for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
      ( *iterGrid )->Update();

    WorldGridObjectList::iterator iter, iterEnd;
    Short x, y;
    WorldGridList greedsToUnload = *__worldGridList;  //список гридов, которые можно выгрузить
    WorldGrid *grid;
    bool oneGridLoaded = false;

    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => grids to unload %d", greedsToUnload.size() );
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => active objects %d", this->activeObjects.size() );

    //проверка на валидность
    bool isActiveObjectsValid;
    do
    {
      isActiveObjectsValid = true;
      iterEnd = this->activeObjects.end();
      for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter )
        if( !( *iter )->GetValid() )
        {
          this->activeObjects.erase( iter );
          isActiveObjectsValid = false;
          break;
        }
    } while( !isActiveObjectsValid );


    iterEnd = this->activeObjects.end();
    for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter )
    {
      //__log.PrintInfo( Filelevel_DEBUG, ". object: x%X", iter->GetObject() );
      //__log.PrintInfo( Filelevel_DEBUG, ". valid: %d", iter->GetValid() );
      WorldGrid::WorldGridPosition pos = this->GetGridPositionByObject( *( ( *iter )->GetObject< Object >() ) );

      for( y = pos.y - this->gridsAroundObject; y <= pos.y + this->gridsAroundObject; ++y )
      for( x = pos.x - this->gridsAroundObject; x <= pos.x + this->gridsAroundObject; ++x )
      {
        WorldGrid::WorldGridPosition newPos( x, y );
        grid = this->IsGridLoaded( newPos );
        if( !grid )
        {
          //подгружаем новый грид
          if( !oneGridLoaded || forceLoadGrids )
          {
            this->LoadGrid( newPos );
            oneGridLoaded = true;
          }
        }
        else
        {
          //убираем грид из списка на выгрузку т.к. грид ещё нам нужен
          WorldGridList::iterator iterGrid, iterGridEnd = greedsToUnload.end();
          for( iterGrid = greedsToUnload.begin(); iterGrid != iterGridEnd; ++iterGrid )
            if( *iterGrid == grid )
            {
              greedsToUnload.erase( iterGrid );
              break;
            }
        }
      }//x,y
      //for( int q =  )
    }//iter

    __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => grids to unload %d", greedsToUnload.size() );
    iterGrid = greedsToUnload.begin();

    while( iterGrid != greedsToUnload.end() )
    {
      this->UnloadGrid( ( *iterGrid )->GetPosition() );
      greedsToUnload.erase( iterGrid );
      iterGrid = greedsToUnload.begin();
    }

    /*
    WorldGridList::iterator iter, iterEnd = __worldGridList->end();
    for( iter = __worldGridList->begin(); iter != iterEnd; ++iter )
    {
    }//for __worldGridList
    */

    this->currentTime -= WORLD_GRID_UPDATE_INTERVAL;
  }//while time
}//Update




/*
=============
  IsGridLoaded
=============
*/
WorldGrid* WorldGridManager::IsGridLoaded( const WorldGrid::WorldGridPosition& gridPos )
{
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
    if( ( *iterGrid )->GetPosition() == gridPos )
      return *iterGrid;

  return NULL;
}//IsGridLoaded




/*
=============
  GetGridPositionByCoords
=============
*/
WorldGrid::WorldGridPosition WorldGridManager::GetGridPositionByCoords( const Vec3& coords ) {
  return WorldGrid::WorldGridPosition(
    Short( Math::Floor( coords.x / this->gridSize ) ),
    Short( Math::Floor( coords.y / this->gridSize ) )
    );
}//GetGridPositionByCoords





/*
=============
  GetGridPositionByObject
=============
*/
WorldGrid::WorldGridPosition WorldGridManager::GetGridPositionByObject( const Object& obj )
{
  return this->GetGridPositionByCoords( obj.GetPosition() );
}//GetGridPositionByObject



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
    if( ( *iter )->GetObject() == obj )
    {
      __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::AddActiveObject => object '%s' already in list", obj->GetNameFull().c_str() );
      return;
    }

  //this->activeObjects.push_back( WorldGridObjectList::value_type( obj ) );
  //this->activeObjects.push_back( WorldGridObjectList::value_type() );
  //( *this->activeObjects.rbegin() )->Init( obj );
  ObjectPointerType *pointer = new ObjectPointerType( obj );
  this->activeObjects.push_back( pointer );

  __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::AddActiveObject => object '%s' added to list", obj->GetNameFull().c_str() );
}//AddActiveObject



/*
=============
  AttachObjectToGrid
=============
*/
void WorldGridManager::AttachObjectToGrid( const WorldGrid::WorldGridPosition& gridPos, Object *obj )
{
  if( !obj )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::AttachObjectToGrid => object is NULL" );
    return;
  }

  bool attached = false;
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
  {
    if( ( *iterGrid )->GetPosition() == gridPos )
    {
      ( *iterGrid )->AttachObject( obj );
      attached = true;
      break;
    }
  }//for iterGrid

  if( !attached )
  {
    WorldGrid *grid = this->LoadGrid( gridPos );
    if( grid )
      grid->AttachObject( obj );
  }
}//AttachObjectToGrid



/*
=============
  AttachObjectToGrid
=============
*/
void WorldGridManager::AttachObjectToGrid( Short gridX, Short gridY, Object *obj )
{
  this->AttachObjectToGrid( WorldGrid::WorldGridPosition( gridX, gridY ), obj );
}//AttachObjectToGrid




/*
=============
  LoadGrid
=============
*/
WorldGrid* WorldGridManager::LoadGrid( const WorldGrid::WorldGridPosition& gridPos )
{
  WorldGrid *grid = this->IsGridLoaded( gridPos );
  if( grid )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::LoadGrid => grid[%d; %d] already loaded", gridPos.x, gridPos.y );
    return grid;
  }

  grid = new WorldGrid( gridPos );

  memory gridData;
  if( this->worldSaver.LoadGrid( gridPos.x, gridPos.y, gridData ) )
    grid->LoadFromDump( gridData, this->rootGridObject );

  __worldGridList->push_back( grid );
  __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::LoadGrid => grid[%d; %d] loaded", gridPos.x, gridPos.y );

  return grid;
}//LoadGrid



/*
=============
  UnloadGrid
=============
*/
bool WorldGridManager::UnloadGrid( const WorldGrid::WorldGridPosition gridPos )
{
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
  {
    if( ( *iterGrid )->GetPosition() == gridPos )
    {
      __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::UnloadGrid => grid[%d; %d] unloading...", gridPos.x, gridPos.y );
      memory gridDump;
      ( *iterGrid )->GetGridDump( gridDump );
      __log.PrintInfo( Filelevel_DEBUG, "grid dump size %d", gridDump.getLength() );
      //tools::Dump( gridDump.getData(), gridDump.getLength(), "gridDump" );

      __log.PrintInfo( Filelevel_DEBUG, ". delete grid..." );
      delete *iterGrid;
      __worldGridList->erase( iterGrid );

      __log.PrintInfo( Filelevel_DEBUG, ". save grid dump..." );
      this->worldSaver.SaveGrid( gridPos.x, gridPos.y, gridDump );

      __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::UnloadGrid => grid[%d; %d] unloaded", gridPos.x, gridPos.y );
      return true;
    }
  }
  __log.PrintInfo( Filelevel_WARNING, "WorldGridManager::UnloadGrid => grid[%d; %d] not found", gridPos.x, gridPos.y );
  return false;
}//UnloadGrid



/*
=============
  SaveToFile
=============
*/
void WorldGridManager::SaveToFile( const std::string& fileName )
{
  while( __worldGridList->size() )
    this->UnloadGrid( (  *__worldGridList->begin() )->GetPosition() );
  this->worldSaver.SaveToFile( fileName );
}//SaveToFile



/*
=============
  LoadFromFile
=============
*/
void WorldGridManager::LoadFromFile( const std::string& fileName )
{
  this->worldSaver.LoadFromFile( fileName );
  this->LoadGrid( WorldGrid::WorldGridPosition( 0, 0 ) );
}//LoadFromFile



/*
=============
  ClearWorld
  Очистка текущих гридов от объектов. Сами гриды при этом остаются
=============
*/
void WorldGridManager::ClearWorld()
{
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
    ( *iterGrid )->DetachAll();
}//ClearWorld
