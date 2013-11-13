#include "lbuffercache.h"
#include "file.h"


const int LBUFFER_CACHE_LIFE_PERIOD = 10;


LBufferCacheEntity::LBufferCacheEntity()
:lifeTime( 0 )
{
}


LBufferCacheEntity::LBufferCacheEntity( void* const setObject, const Vec2& setPosition, const Vec2& setSize )
:object( setObject ), position( setPosition ), size( setSize ) {
}


void LBufferCacheEntity::Reset( const Vec2& setPosition, const Vec2& setSize ) {
  this->position = setPosition;
  this->size = setSize;
  this->lifeTime = 0;
  this->values.clear();
}


LBufferCache::LBufferCache() {
}


LBufferCache::~LBufferCache() {
}


bool LBufferCacheEntity::operator==( const LBufferCacheEntity& item ) const {
  return this->object == item.object;
}


void LBufferCacheEntity::WriteToBuffer( float *buffer ) {
  for( auto &value: this->values ) {
    if( buffer[ value.index ] > value.value ) {
      buffer[ value.index ] = value.value;
    }
  }
}//WriteToBuffer


bool LBufferCache::CheckCache( void *object, const Vec2& position, const Vec2& size, LBufferCacheEntity **outCacheElement ) {
  auto element = this->cache.find( object );
  if( element == this->cache.end() ) {
    LBufferCacheEntity *newElement = this->AddElement( object, position, size );
    if( outCacheElement ) {
      *outCacheElement = newElement;
    }
    return false;
  }
  element->second->lifeTime = 0;
  if( outCacheElement ) {
    *outCacheElement = element->second;
  }
  return
    element->second->position == position &&
    element->second->size == size;
}//CheckCache


void LBufferCache::ClearCache() {
  for( auto &element: this->cache ) {
    delete element.second;
  }
  this->cache.clear();
}//ClearCache


void LBufferCache::ClearCache( void* object ) {
  auto element = this->cache.find( object );
  if( element != this->cache.end() ) {
    this->cache.erase( element );
  }
}//ClearCache


LBufferCacheEntity* LBufferCache::AddElement( void *object, const Vec2& position, const Vec2& size ) {
  LBufferCacheEntity *newEntity = new LBufferCacheEntity( object, position, size );
  this->cache.insert( std::make_pair( object, newEntity ) );
  return newEntity;
}//AddElement


void LBufferCache::Update() {
  ElementList::iterator
    iter,
    iterEnd = this->cache.end(),
    iterNext;
  for( iter = this->cache.begin(); iter != iterEnd; ) {
    if( ++iter->second->lifeTime > LBUFFER_CACHE_LIFE_PERIOD ) {
      iterNext = iter;
      ++iterNext;
      delete iter->second;
      this->cache.erase( iter );
      iter = iterNext;
    } else {
      ++iter;
    }
  }
}//Update
