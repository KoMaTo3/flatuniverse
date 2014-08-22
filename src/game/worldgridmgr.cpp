#include "worldgridmgr.h"
#include "core/tools.h"
#include "core/timer.h"
#include "core/file.h"



World::GridManager::GridManager( Object* newRootGridObject, Short setGridsAroundObject, float setGridSize )
:currentTime( 0.0f ), gridSize( setGridSize ), gridsAroundObject( setGridsAroundObject ), rootGridObject( newRootGridObject ), version( 0 ), currentTimeLoader( 0.0f )
{
  if( !__worldGridList )
    __worldGridList = new World::GridList();
}//constructor



World::GridManager::~GridManager()
{
  DEF_DELETE( __worldGridList );
}//destructor




/*
=============
  Update
=============
*/
void World::GridManager::Update( bool forceLoadGrids )
{
  bool forceUpdate = ( sTimer.GetDeltaF() == 0.0f );
  float updateInterval = ( forceUpdate ? 0.0f : WORLD_GRID_UPDATE_INTERVAL );
  this->currentTime += sTimer.GetDeltaF();
  this->currentTimeLoader += sTimer.GetDeltaF();

  if( this->currentTimeLoader > WORLD_GRID_LOADER_UPDATE_INTERVAL || forceUpdate ) {
    World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
    for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid ) {
      if( !( *iterGrid )->IsFullyLoaded() ) {
        ( *iterGrid )->Update();
      }
    }
    this->currentTimeLoader -= WORLD_GRID_LOADER_UPDATE_INTERVAL;
  }

  while( this->currentTime > WORLD_GRID_UPDATE_INTERVAL || forceUpdate )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => __worldGridList[%d]", __worldGridList->size() );

    //Update grids
    World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
    for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid ) {
      ( *iterGrid )->Update();
    }
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => __worldGridList updated" );

    World::GridObjectList::iterator iter, iterEnd;
    Short x, y;
    World::GridList greedsToUnload = *__worldGridList;  //список гридов, которые можно выгрузить
    World::Grid *grid;

    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => grids to unload %d", greedsToUnload.size() );
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => active objects %d", this->activeObjects.size() );

    //проверка на валидность
    bool isActiveObjectsValid;
    do
    {
      isActiveObjectsValid = true;
      iterEnd = this->activeObjects.end();
      for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter ) {
        if( !( *iter )->GetIsValid() ) {
          this->activeObjects.erase( iter );
          isActiveObjectsValid = false;
          break;
        }
      }
    } while( !isActiveObjectsValid );


    iterEnd = this->activeObjects.end();
    for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter )
    {
      //__log.PrintInfo( Filelevel_DEBUG, ". object: x%X", iter->GetObject() );
      //__log.PrintInfo( Filelevel_DEBUG, ". valid: %d", iter->GetValid() );
      World::Grid::Position pos = this->GetGridPositionByObject( *( ( *iter )->GetObject< Object >() ) );

      for( y = pos.y - this->gridsAroundObject; y <= pos.y + this->gridsAroundObject; ++y )
      for( x = pos.x - this->gridsAroundObject; x <= pos.x + this->gridsAroundObject; ++x )
      {
        World::Grid::Position newPos( x, y );
        grid = this->IsGridLoaded( newPos );
        if( !grid )
        {
          //подгружаем новый грид
          this->LoadGrid( newPos, forceLoadGrids );
        }
        else
        {
          //убираем грид из списка на выгрузку т.к. грид ещё нам нужен
          World::GridList::iterator iterGrid, iterGridEnd = greedsToUnload.end();
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

    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => grids to unload %d", greedsToUnload.size() );
    iterGrid = greedsToUnload.begin();

    while( iterGrid != greedsToUnload.end() )
    {
      this->UnloadGrid( ( *iterGrid )->GetPosition() );
      greedsToUnload.erase( iterGrid );
      iterGrid = greedsToUnload.begin();
    }

    //перенос объектов из грида в грид
    iterGridEnd = __worldGridList->end();
    bool  finded,
          keepChecking = true;
    Object *object;
    for( iterGrid = __worldGridList->begin(); keepChecking && iterGrid != iterGridEnd; ++iterGrid ) {
      World::Grid::Position gridPos( ( *iterGrid )->GetPosition() );
      for( finded = ( *iterGrid )->GetFirstMovableObject( iter ); keepChecking && finded; finded = ( *iterGrid )->GetNextMovableObject( iter ) ) {
        object = ( *iter )->GetObject< Object >();
        World::Grid::Position pos( this->GetGridPositionByObject( *object ) );
        if( pos != gridPos ) {
          //__log.PrintInfo( Filelevel_WARNING, "WorldGridManager::Update => move object '%s' from grid[%d; %d] to grid[%d; %d]", object->GetNameFull().c_str(), gridPos.x, gridPos.y, pos.x, pos.y );
          ( *iterGrid )->DetachObject( object );
          grid = this->IsGridLoaded( pos );
          if( grid ) {
            grid->AttachObject( object );
          } else {
            //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => load grid[%d; %d]...", pos.x, pos.y );
            this->LoadGrid( pos )->AttachObject( object );
            //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update => load grid[%d; %d] done", pos.x, pos.y );
          }
          __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::Update => object '%s' x%p moved to other grid", object->GetNameFull().c_str(), object );
          keepChecking = false;
          break;
        }
      }//for
    }//for
    //Проверка на выход объектов за пределы гридов: проверяем все динамические коллизии
    /*
    if( __objectByCollision && __objectByCollision->size() ) {
      ObjectByCollisionList::iterator iter, iterTmp, iterEnd;
      bool needRepear;
      do {
        needRepear = false;
        iterEnd = __objectByCollision->end();
        Object *object;
        Vec3 pos;
        for( iter = __objectByCollision->begin(); iter != iterEnd; ++iter ) {
          if( ( *iter )->object.GetValid() ) {
            object = ( *iter )->object.GetObject< Object >();
            if( object->GetCollision() && !object->GetCollision()->IsStatic() ) {
              pos = object->GetPosition();
            }
          }
        }
      } while( needRepear );
    }
    */

    this->currentTime -= updateInterval;
    if( forceUpdate ) {
      break;
    }
    //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::Update done" );
  }//while time
}//Update




/*
=============
  IsGridLoaded
=============
*/
World::Grid* World::GridManager::IsGridLoaded( const World::Grid::Position& gridPos )
{
  World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
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
World::Grid::Position World::GridManager::GetGridPositionByCoords( const Vec3& coords ) {
  return World::Grid::Position(
    Short( Math::Floor( coords.x / this->gridSize ) ),
    Short( Math::Floor( coords.y / this->gridSize ) )
    );
}//GetGridPositionByCoords





/*
=============
  GetGridPositionByObject
=============
*/
World::Grid::Position World::GridManager::GetGridPositionByObject( const Object& obj )
{
  return this->GetGridPositionByCoords( obj.GetPosition() );
}//GetGridPositionByObject



/*
=============
  AddActiveObject
=============
*/
void World::GridManager::AddActiveObject( Object *obj, const bool forceLoadGrid )
{
  if( !obj )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::GridManager::AddActiveObject => object is NULL" );
    return;
  }

  World::GridObjectList::iterator iter, iterEnd = this->activeObjects.end();

  for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetObject< Object >() == obj ) {
      __log.PrintInfo( Filelevel_WARNING, "World::GridManager::AddActiveObject => object '%s' already in list", obj->GetNameFull().c_str() );
      return;
    }
  }

  //this->activeObjects.push_back( WorldGridObjectList::value_type( obj ) );
  //this->activeObjects.push_back( WorldGridObjectList::value_type() );
  //( *this->activeObjects.rbegin() )->Init( obj );
  ObjectPointerType *pointer = new ObjectPointerType( obj );
  this->activeObjects.push_back( pointer );
  obj->SetIsActiveObject( true );

  for( auto &grid: *__worldGridList ) {
    if( grid->IsThisObject( obj ) ) {
      grid->DetachObject( obj );
      break;
    }
  }

  //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::AddActiveObject => object '%s' added to list", obj->GetNameFull().c_str() );
}//AddActiveObject



/*
=============
  RemoveActiveObject
=============
*/
void World::GridManager::RemoveActiveObject( Object *obj )
{
  if( !obj ) {
    __log.PrintInfo( Filelevel_WARNING, "World::GridManager::RemoveActiveObject => object is NULL" );
    return;
  }

  World::GridObjectList::iterator iter, iterEnd = this->activeObjects.end();

  for( iter = this->activeObjects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetObject< Object >() == obj ) {
      delete *iter;
      this->activeObjects.erase( iter );
      return;
    }
  }

  __log.PrintInfo( Filelevel_ERROR, "World::GridManager::RemoveActiveObject => object '%s' not found in active-list", obj->GetNameFull().c_str() );
}//RemoveActiveObject



/*
=============
  AttachObjectToGrid
=============
*/
void World::GridManager::AttachObjectToGrid( const World::Grid::Position& gridPos, Object *obj )
{
  if( !obj )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::GridManager::AttachObjectToGrid => object is NULL" );
    return;
  }

  __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::AttachObjectToGrid => object '%s' x%p attaching to grid", obj->GetNameFull().c_str(), obj );
  bool attached = false;
  World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
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
    World::Grid *grid = this->LoadGrid( gridPos );
    if( grid ) {
      grid->AttachObject( obj );
    }
  }
}//AttachObjectToGrid



/*
=============
  AttachObjectToGrid
=============
*/
void World::GridManager::AttachObjectToGrid( Short gridX, Short gridY, Object *obj )
{
  this->AttachObjectToGrid( World::Grid::Position( gridX, gridY ), obj );
}//AttachObjectToGrid




/*
=============
  LoadGrid
=============
*/
World::Grid* World::GridManager::LoadGrid( const World::Grid::Position& gridPos, const bool forceLoad )
{
  __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::LoadGrid => pos[%d; %d]", gridPos.x, gridPos.y );
  World::Grid *grid = this->IsGridLoaded( gridPos );
  if( grid )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::GridManager::LoadGrid => grid[%d; %d] already loaded", gridPos.x, gridPos.y );
    return grid;
  }

  grid = new World::Grid( gridPos );

  memory gridData;
  Dword gridVersion = 0;
  World::GridCoreObjectList newActiveObjectsList;
  if( this->worldSaver.LoadGrid( gridPos.x, gridPos.y, gridData, gridVersion ) ) {
    grid->LoadFromDump( gridData, this->rootGridObject, gridVersion, forceLoad );
    for( auto &object: newActiveObjectsList ) {
      this->AddActiveObject( object );
    }
    __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::LoadGrid ok" );
  }

  __worldGridList->push_back( grid );
  //__log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::LoadGrid => grid[%d; %d] loaded", gridPos.x, gridPos.y );

  return grid;
}//LoadGrid



/*
=============
  UnloadGrid
=============
*/
bool World::GridManager::UnloadGrid( const World::Grid::Position gridPos, const bool forceUnload )
{
  World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
  {
    if( ( *iterGrid )->GetPosition() == gridPos )
    {
      __log.PrintInfo( Filelevel_DEBUG, "WorldGridManager::UnloadGrid => grid[%d; %d] unloading...", gridPos.x, gridPos.y );
      if( ( *iterGrid )->GetState() != WORLDGRID_STATE_DONE && !forceUnload ) {
        break;
      }
      memory gridDump;
      while( !( *iterGrid )->GetGridDump( gridDump ) ) {
        ( *iterGrid )->Update();
      }
      //__log.PrintInfo( Filelevel_DEBUG, "grid dump size %d", gridDump.getLength() );
      //tools::Dump( gridDump.getData(), gridDump.getLength(), "gridDump" );

      //__log.PrintInfo( Filelevel_DEBUG, ". delete grid..." );
      delete *iterGrid;
      __worldGridList->erase( iterGrid );

      //__log.PrintInfo( Filelevel_DEBUG, ". save grid dump..." );
      this->worldSaver.SaveGrid( gridPos.x, gridPos.y, gridDump );

      __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::UnloadGrid => grid[%d; %d] unloaded", gridPos.x, gridPos.y );
      return true;
    }
  }
  __log.PrintInfo( Filelevel_WARNING, "World::GridManager::UnloadGrid => grid[%d; %d] not found or is not fully loaded", gridPos.x, gridPos.y );
  return false;
}//UnloadGrid



/*
=============
  SaveToFile
=============
*/
void World::GridManager::SaveToFile( const std::string& fileName )
{
  __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::SaveToFile => begin" );
  while( !__worldGridList->empty() ) {
    this->UnloadGrid( (  *__worldGridList->begin() )->GetPosition(), true );
  }
  __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::SaveToFile => grids unloaded" );
  this->worldSaver.SaveToFile( &this->activeObjects, fileName );
  __log.PrintInfo( Filelevel_DEBUG, "World::GridManager::SaveToFile => done" );
}//SaveToFile



/*
=============
  LoadFromFile
=============
*/
void World::GridManager::LoadFromFile( const std::string& fileName )
{
  this->version = this->worldSaver.LoadFromFile( fileName, this->rootGridObject );
}//LoadFromFile



/*
=============
  ClearWorld
  Очистка текущих гридов от объектов. Сами гриды при этом остаются
=============
*/
void World::GridManager::ClearWorld()
{
  World::GridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
    ( *iterGrid )->DetachAll();
  __worldGridList->clear();
}//ClearWorld
