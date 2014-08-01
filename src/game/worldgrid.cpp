#include "worldgrid.h"
#include "core/tools.h"
#include "core/file.h"
#include "core/memorywriter.h"
#include "core/memoryreader.h"


World::GridList *World::__worldGridList = NULL;




World::Grid::Grid( const World::Grid::Position &newPosition )
:position( newPosition ), version( 0 ), state( WORLDGRID_STATE_DONE )
{
  World::GridList::iterator iter, iterEnd = __worldGridList->end();

  bool finded = false;
  for( iter = __worldGridList->begin(); iter != iterEnd; ++iter )
    if( ( *iter )->position.x == newPosition.x && ( *iter )->position.y == newPosition.y )
    {
      finded = true;
      break;
    }
  if( finded )
    __log.PrintInfo( Filelevel_ERROR, "World::Grid constructor: grid[%d; %d] already exists" );
  /* else
    __log.PrintInfo( Filelevel_DEBUG, "World::Grid: new grid [%d; %d]", newPosition.x, newPosition.y );*/

  this->loader.objectsCount = 0;
  this->loader.rootObject = nullptr;
}//constructor



World::Grid::~Grid()
{
  World::GridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter )
    if( ( *iter )->GetIsValid() )
    {
      //__log.PrintInfo( Filelevel_DEBUG, "~WorldGrid => deleting object x%X['%s']", ( *iter )->GetObject< Object >(), ( *iter )->GetObject< Object >()->GetNameFull().c_str() );
      ( *iter )->GetObject< Object >()->SetNeedToUnload( true );
      //this->DetachObject( ( *iter )->GetObject< Object >() );
      //delete ( *iter )->GetObject< Object >();
    }
  //__log.PrintInfo( Filelevel_DEBUG, "~WorldGrid => clear pointers list" );
  this->DetachAll();

  while( !this->objects.empty() )
  {
    delete *this->objects.rbegin();
    this->objects.pop_back();
  }
  //this->objects.clear();

  //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid deleted: pos[%d; %d]", this->position.x, this->position.y );
}//destructor



/*
=============
  AttachObject
=============
*/
void World::Grid::AttachObject( Object *object )
{
  //this->Update();
  //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid::AttachObject => object[x%X]", object );
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::Grid::AttachObject => object is NULL" );
    return;
  }

  /*
  if( this->IsThisObject( object ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "WorldGrid::AttachObject => object x%X already in this grid", object );
    return;
  }

  //детачим от других гридов
  WorldGridList::iterator iterGrid, iterGridEnd = __worldGridList->end();
  for( iterGrid = __worldGridList->begin(); iterGrid != iterGridEnd; ++iterGrid )
  {
    if( ( *iterGrid ) != this && ( *iterGrid )->IsThisObject( object ) )
    {
      ( *iterGrid )->DetachObject( object );
      break;
    }
  }//for iterGrid

  this->objects.push_back( ObjectPointerType() );
  this->objects.rbegin()->Init( object );
  __log.PrintInfo( Filelevel_DEBUG, "WorldGrid::AttachObject => object x%X attached to grid[%d; %d] pointer[x%X]", object, this->position.x, this->position.y, &( *this->objects.rbegin() ) );
  */

  ObjectPointerType *pointer = new ObjectPointerType( object );
  this->objects.push_back( pointer );
}//AttachObject



/*
=============
  DetachObject
=============
*/
void World::Grid::DetachObject( Object *object )
{
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::Grid::DetachObject => object is NULL" );
    return;
  }

  this->Update();
  __log.PrintInfo( Filelevel_DEBUG, "World::Grid::DetachObject => object[x%X]", object );
  World::GridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetObject< Object >() == object ) {
      delete ( *iter );
      this->objects.erase( iter );
      break;
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "World::Grid::DetachObject ok" );
}//DetachObject



/*
=============
  DetachAll
=============
*/
void World::Grid::DetachAll()
{
  while( !this->objects.empty() )
  {
    delete *this->objects.rbegin();
    this->objects.pop_back();
  }
  //this->objects.clear();
}//DetachAll




/*
=============
  IsThisObject
=============
*/
bool World::Grid::IsThisObject( Object *object )
{
  if( !object )
  {
    __log.PrintInfo( Filelevel_WARNING, "World::Grid::AttachObject => object is NULL" );
    return false;
  }

  World::GridObjectList::iterator iter, iterEnd = this->objects.end();
  for( iter = this->objects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetObject< Object >() == object ) {
      return true;
    }
  }

  return false;
}//IsThisObject



/*
=============
  GetGridDump
=============
*/
bool World::Grid::GetGridDump( FU_OUT memory& dump )
{
  if( !( this->state == WORLDGRID_STATE_DONE || this->state == WORLDGRID_STATE_UNLOADING ) ) {
    return false;
  }
  this->Update();
  dump.Free();
  MemoryWriter writer( dump );

  //кол-во объектов
  Dword objectsCount = this->objects.size();
  writer << objectsCount;
  //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid::GetGridDump => objects %d", objectsCount );

  //объекты
  World::GridObjectList::iterator iter, iterEnd = this->objects.end();
  //Object *obj;
  ////RenderableQuad *quad;
  //Collision *collision;
  //std::string textureName;
  for( iter = this->objects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetIsValid() ) {
      ( *iter )->GetObject< Object >()->SaveToBuffer( writer );
    }
  }

  return true;
}//GetGridDump



/*
=============
  LoadFromDump
=============
*/
bool World::Grid::LoadFromDump( FU_IN memory& dump, Object *rootObject, const Dword fileVersion, const bool forceLoad )
{
  __log.PrintInfo( Filelevel_DEBUG, "World::Grid::LoadFromDump => dump length %d byte(s) rootObject[x%X] forceLoad[%d]", dump.getLength(), rootObject, forceLoad );
  this->loader.dump = dump;
  this->loader.rootObject = rootObject;
  this->loader.reader.SetSource( this->loader.dump.getData(), this->loader.dump.getLength() );
  Dword q = 0;
  this->version = fileVersion;
  this->loader.reader >> this->loader.objectsCount;
  __log.PrintInfo( Filelevel_DEBUG, "World::Grid::LoadFromDump => %d objects in dump, readerPos[%d]", this->loader.objectsCount, this->loader.reader.GetCurPos() );

  //this->Update();
  if( this->loader.objectsCount ) {
    //this->isFullyLoaded = false;
    this->state = WORLDGRID_STATE_LOADING;
    if( forceLoad ) {
      while( this->loader.objectsCount ) {
        this->Update();
      }
    }
  } else {
    //this->isFullyLoaded = true;
    this->state = WORLDGRID_STATE_DONE;
  }
  /*
  Object *object;
  for( ; q < objectsNum; ++q )
  {
    //object = new Object( name, ( rootObject->GetNameFull() == parentName ? rootObject: rootObject->GetObject( parentName.c_str() ) ) );
    object = new Object();
    object->LoadFromBuffer( reader, rootObject, this->version );

    //this->objects.push_back( ObjectPointerType() );
    //this->objects.rbegin()->Init( object );

    this->AttachObject( object );
  }//for q
  */

  return true;
}//LoadFromDump



/*
=============
  Update
=============
*/
void World::Grid::Update()
{
  //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid::Update" );
  if( this->state == WORLDGRID_STATE_DONE ) {
    World::GridObjectList::iterator iter, iterEnd;
    bool changed;
    do
    {
      changed = false;
      iterEnd = this->objects.end();
      //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid::Update => pos[%d; %d] objects[%d]", this->position.x, this->position.y, this->objects.size() );
      for( iter = this->objects.begin(); iter != iterEnd; ++iter )
      {
        //__log.PrintInfo( Filelevel_DEBUG, ". pointer[x%X] object[x%X] valid[%d]", &( *iter ), iter->GetObject(), iter->GetValid() );
        if( !( *iter )->GetIsValid() ) {
          //__log.PrintInfo( Filelevel_DEBUG, "WorldGrid::Update => object[x%X] is not valid, delete from grid", ( *iter )->GetObjectSrc() );
          //this->DetachObject( iter->GetObjectSrc< Object >() );
          delete ( *iter );
          this->objects.erase( iter );
          changed = true;
          break;
        }
      }
    } while( changed );
  } else {
    //object = new Object( name, ( rootObject->GetNameFull() == parentName ? rootObject: rootObject->GetObject( parentName.c_str() ) ) );
    __log.PrintInfo( Filelevel_DEBUG, "World::Grid::Update => objects[%d]", this->loader.objectsCount );
    Object *object = new Object();
    __log.PrintInfo( Filelevel_DEBUG, "World::Grid::Update => readerPos[%d]", this->loader.reader.GetCurPos() );
    object->LoadFromBuffer( this->loader.reader, this->loader.rootObject, this->version );
    --this->loader.objectsCount;
    if( !this->loader.objectsCount ) {
      this->state = WORLDGRID_STATE_DONE;
      this->loader.dump.Free();
    }

    //this->objects.push_back( ObjectPointerType() );
    //this->objects.rbegin()->Init( object );

    this->AttachObject( object );
  }
}//Update



/*
=============
  GetFirstMovableObject
=============
*/
bool World::Grid::GetFirstMovableObject( World::GridObjectList::iterator &iter ) {
  World::GridObjectList::iterator iterEnd = this->objects.end();
  Object *object;
  for( iter = this->objects.begin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->GetIsValid() ) {
      object = ( *iter )->GetObject< Object >();
      if( object->GetCollision() && !object->GetCollision()->IsStatic() ) {
        return true;
      }
    }
  }
  return false;
}//GetFirstMovableObject



/*
=============
  GetNextMovableObject
=============
*/
bool World::Grid::GetNextMovableObject( World::GridObjectList::iterator &iter ) {
  World::GridObjectList::iterator iterEnd = this->objects.end();
  Object *object;
  for( ++iter; iter != iterEnd; ++iter ) {
    if( ( *iter )->GetIsValid() ) {
      object = ( *iter )->GetObject< Object >();
      if( object->GetCollision() && !object->GetCollision()->IsStatic() ) {
        return true;
      }
    }
  }
  return false;
}//GetNextMovableObject
