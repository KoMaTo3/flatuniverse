#include "object.h"
#include "file.h"
#include "tools.h"

extern CoreRenderableList *__coreRenderableList;
extern CoreRenderableListIndicies *__coreRenderableListIndicies;
extern CoreRenderableListIndicies *__coreRenderableListFreeIndicies;
extern CoreRenderableList *__coreGUI;
extern CoreRenderableListIndicies *__coreGUIIndicies;
extern CoreRenderableListIndicies *__coreGUIFreeIndicies;



Object::ObjectRenderableInfo::ObjectRenderableInfo()
{
}//constructor



Object::ObjectRenderableInfo::ObjectRenderableInfo( const ObjectRenderableInfo& from )
:num( from.num ), type( from.type )
{
}//constructor



Object::ObjectRenderableInfo::ObjectRenderableInfo( GLushort newNum, RenderableType newType )
:num( newNum ), type( newType )
{
}//constructor











Object::Object( const std::string &objectName, Object* parentObject )
:name( objectName ), _parent( parentObject ), _childs( NULL ), renderable( -1, RENDERABLE_TYPE_UNKNOWN ), collision( NULL )
,position( 0.0f, 0.0f, 0.0f ), positionSrc( 0.0f, 0.0f, 0.0f ), _renderableList( NULL )
{
  if( this->_parent )
  {
    this->nameFull = this->_parent->GetNameFull() + "/" + this->name;
    this->_parent->AttachChildObject( this );
  }
  else
    this->nameFull = "/" + this->name;
  __log.PrintInfo( Filelevel_DEBUG, "Object +1 => this[x%X] parent[x%X] name['%s']", this, this->_parent, this->nameFull.c_str() );
}//constructor




Object::~Object()
{
  //__log.PrintInfo( Filelevel_DEBUG, "Object -1 => this[x%X]", this );
  //if( !this->_parent )
  //  __log.PrintInfo( Filelevel_DEBUG, "is a root object" );

  this->DisableRenderable();
  this->DisableCollision();
  this->ClearChilds();
  this->UnAttachThisFromParent();

  //__log.PrintInfo( Filelevel_DEBUG, "Object x%X deleted", this );
}//destructor



/*
=============
  ClearChilds
  Удаление рекурсивно всех потомков
=============
*/
void Object::ClearChilds()
{
  if( !this->_childs )
    return;

  ObjectChilds::iterator iter;
  while( this->_childs->size() )
    delete *this->_childs->begin();

  DEF_DELETE( this->_childs );
}//ClearChilds



/*
=============
  GetName
=============
*/
const std::string& Object::GetName()
{
  return this->name;
}//GetName



/*
=============
  GetNameFull
=============
*/
const std::string& Object::GetNameFull()
{
  return this->nameFull;
}//GetNameFull



/*
=============
  GetParentNameFull
=============
*/
const std::string& Object::GetParentNameFull()
{
  if( !this->_parent )
  {
    __log.PrintInfo( Filelevel_WARNING, "Object::GetParentNameFull => parent is NULL" );
    static std::string badResult = "<NULL>";
    return badResult;
  }
  return this->_parent->GetNameFull();
}//GetParentNameFull



/*
=============
  AttachChildObject
=============
*/
void Object::AttachChildObject( Object* newChild )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Object::AttachChildObject => this[x%X] child[x%X]", this, newChild );
  if( newChild && !this->IsChild( newChild ) )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "Object::AttachChildObject => '%s' now is child of '%s'", newChild->GetNameFull().c_str(), this->GetNameFull().c_str() );
    if( !this->_childs )
      this->_childs = new ObjectChilds();
    this->_childs->push_back( newChild );
    newChild->_parent = this;
  }
  else
    __log.PrintInfo( Filelevel_WARNING, "Object::AttachChildObject => child is NULL or child already in list" );
}//AttachChildObject



/*
=============
  UnAttachThisFromParent
=============
*/
void Object::UnAttachThisFromParent()
{
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachThisFromParent => this[x%X]", this );
  if( !this->_parent )
    return;
  this->_parent->UnAttachChildObject( this );
  this->_parent = NULL;
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachThisFromParent => done" );
}//UnAttachThisFromParent




/*
=============
  UnAttachChildObject
=============
*/
void Object::UnAttachChildObject( Object* child )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachChildObject => this[x%X] child[x%X]", this, child );
  if( !child )
    return;
  if( this->_childs && this->IsChild( child ) )
  {
    child->_parent = NULL;
    ObjectChilds::iterator iter, iterEnd = this->_childs->end();
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      if( *iter == child )
      {
        this->_childs->erase( iter );
        break;
      }
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachChildObject => done" );
}//UnAttachChildObject




/*
=============
  IsChild
=============
*/
bool Object::IsChild( const Object* obj )
{
  if( !obj )
    return false;
  if( !this->_childs )
    return false;

  for( ObjectChilds::iterator iter = this->_childs->begin(); iter != this->_childs->end(); ++iter )
    if( *iter == obj )
      return true;

  return false;
}//IsChild




/*
=============
  GetChild
=============
*/
Object* Object::GetChild( const std::string& name )
{
  if( !name.length() )
  {
    __log.PrintInfo( Filelevel_WARNING, "Object::GetChild => name is NULL" );
    return NULL;
  }

  if( !this->_childs )
  {
    __log.PrintInfo( Filelevel_WARNING, "Object::GetChild => no childs in '%s'", this->GetNameFull().c_str() );
    return NULL;
  }

  //__log.PrintInfo( Filelevel_DEBUG, "Object::GetChild => name['%s'] thisName['%s']", name.c_str(), this->GetNameFull().c_str() );
  for( ObjectChilds::iterator iter = this->_childs->begin(); iter != this->_childs->end(); ++iter )
    if( ( *iter )->GetName() == name )
      return *iter;

  __log.PrintInfo( Filelevel_WARNING, "Object::GetChild => '%s' not found in '%s'", name.c_str(), this->GetNameFull().c_str() );

  return NULL;
}//GetChild




/*
=============
  EnableRenderable
  Добавляет объект в рендер, при этом необходимо указать как он будет рендериться - квад, спрайт, текст, точка, линия, меш и т.д.
=============
*/
Renderable* Object::EnableRenderable( RenderableType renderType )
{
  if( this->renderable.num != RENDERABLE_INDEX_UNDEFINED )
    return &( *( this->_renderableList->begin() + this->renderable.num ) );

  this->_renderableList = __coreRenderableList;
  this->_renderableIndicies = __coreRenderableListIndicies;
  this->_renderableFreeIndicies = __coreRenderableListFreeIndicies;

  this->_RecalculatePosition();
  float zIndex = this->position.z;
  Renderable *result = NULL;
  switch( renderType )
  {
  case RENDERABLE_TYPE_QUAD:
    {
      this->renderable.type = renderType;

      GLshort index = -1;
      __log.PrintInfo( Filelevel_DEBUG, "Free indicies in x%X = %d", this->_renderableFreeIndicies, this->_renderableFreeIndicies->size() );
      if( this->_renderableFreeIndicies->size() )
      {
        index = *this->_renderableFreeIndicies->rbegin();
        this->_renderableFreeIndicies->pop_back();
        result = &( *( this->_renderableList->begin() + index ) );
        *result = RenderableQuad();
        __log.PrintInfo( Filelevel_DEBUG, "Use free indicies: %d", index );
      }
      else
      {
        this->_renderableList->push_back( RenderableQuad() );
        result = &( *this->_renderableList->rbegin() );
        index = this->_renderableList->size() - 1;
      }

      //this->_renderableList->push_back( RenderableQuad() );
      //result = &( *this->_renderableList->rbegin() );

      RenderableQuad *quad = ( RenderableQuad* ) result;
      quad->SetPosition( Vec3( 0.0f, 0.0f, zIndex ) );
      float z = quad->GetPosition().z;
      this->renderable.num = index;

      //this->_renderableIndicies->push_back( this->renderable.num );
      CoreRenderableListIndicies::iterator iter, iterEnd = this->_renderableIndicies->end(), iterBegin = this->_renderableIndicies->begin();
      bool added = false;
      for( iter = iterBegin; iter != iterEnd; ++iter )
        if( z < ( *( this->_renderableList->begin() + *iter ) ).GetPosition().z )
        {
          this->_renderableIndicies->insert( iter, this->renderable.num );
          added = true;
          break;
        }
      if( !added )
        this->_renderableIndicies->push_back( this->renderable.num );
    }
  break;
  default:
    this->renderable.type = RENDERABLE_TYPE_UNKNOWN;
    return NULL;
  break;
  }

  return result;
}//EnableRenderable




/*
=============
  EnableRenderableGUI
  Добавляет объект в рендер GUI
=============
*/
RenderableQuad* Object::EnableRenderableGUI()
{
  if( this->renderable.num != RENDERABLE_INDEX_UNDEFINED )
    return &( *( this->_renderableList->begin() + this->renderable.num ) );

  this->_renderableList = __coreGUI;
  this->_renderableIndicies = __coreGUIIndicies;
  this->_renderableFreeIndicies = __coreGUIFreeIndicies;

  RenderableQuad *result = NULL;
  this->renderable.type = RENDERABLE_TYPE_QUAD;

  __log.PrintInfo( Filelevel_DEBUG, "Object::EnableRenderableGUI => current position[ %3.3f; %3.3f; %3.3f ]", this->position.x, this->position.y, this->position.z );
  this->_RecalculatePosition();
  float zIndex = this->position.z;
  GLshort index = -1;
  __log.PrintInfo( Filelevel_DEBUG, "Free indicies in x%X = %d", this->_renderableFreeIndicies, this->_renderableFreeIndicies->size() );
  if( this->_renderableFreeIndicies->size() )
  {
    index = *this->_renderableFreeIndicies->rbegin();
    this->_renderableFreeIndicies->pop_back();
    result = &( *( this->_renderableList->begin() + index ) );
    *result = RenderableQuad();
    __log.PrintInfo( Filelevel_DEBUG, "Use free indicies: %d", index );
  }
  else
  {
    this->_renderableList->push_back( RenderableQuad() );
    result = &( *this->_renderableList->rbegin() );
    index = this->_renderableList->size() - 1;
  }
  RenderableQuad *quad = ( RenderableQuad* ) result;
  quad->SetPosition( Vec3( 0.0f, 0.0f, zIndex ) );

  this->renderable.num = index;

  //this->_renderableIndicies->push_back( this->renderable.num );
  CoreRenderableListIndicies::iterator iter, iterEnd = this->_renderableIndicies->end(), iterBegin = this->_renderableIndicies->begin();
  bool added = false;
  for( iter = iterBegin; iter != iterEnd; ++iter )
    if( zIndex < ( *( this->_renderableList->begin() + *iter ) ).GetPosition().z )
    {
      __log.PrintInfo( Filelevel_DEBUG, "Object::EnableRenderableGUI => index[%d] z[%3.3f]", this->renderable.num, zIndex );
      this->_renderableIndicies->insert( iter, this->renderable.num );
      added = true;
      break;
    }
  if( !added )
  {
    __log.PrintInfo( Filelevel_DEBUG, "Object::EnableRenderableGUI => last index[%d] z[%3.3f]", this->renderable.num, zIndex );
    this->_renderableIndicies->push_back( this->renderable.num );
  }

  return result;
}//EnableRenderableGUI




/*
=============
  DisableRenderable
=============
*/
bool Object::DisableRenderable()
{
  if( this->renderable.num == RENDERABLE_INDEX_UNDEFINED || !this->_renderableList || !this->_renderableIndicies )
    return false;

  if( ( long ) this->_renderableList->size() <= this->renderable.num )
    return false;

  //__coreRenderableList->erase( __coreRenderableList->begin() + this->renderable.num );

  CoreRenderableListIndicies::iterator iter, iterEnd = this->_renderableIndicies->end();
  for( iter = this->_renderableIndicies->begin(); iter != iterEnd; ++iter )
    if( *iter == this->renderable.num )
    {
      __log.PrintInfo( Filelevel_DEBUG, "add free index %d in list x%X", *iter, this->_renderableFreeIndicies );
      this->_renderableFreeIndicies->push_back( *iter );
      this->_renderableIndicies->erase( iter );
      break;
    }
  __log.PrintInfo( Filelevel_DEBUG, "Object::DisableRenderable => index[%d]", this->renderable.num );

  this->renderable.num = RENDERABLE_INDEX_UNDEFINED;
  this->renderable.type = RENDERABLE_TYPE_UNKNOWN;

  return true;
}//DisableRenderable



/*
=============
  GetRenderable
=============
*/
Renderable* Object::GetRenderable()
{
  if( this->renderable.num == RENDERABLE_INDEX_UNDEFINED )
    return NULL;

  switch( this->renderable.type )
  {
  case RENDERABLE_TYPE_QUAD:
    {
      if( !this->_renderableList )
        return NULL;
      if( ( long ) this->_renderableList->size() <= this->renderable.num )
        return NULL;
      return &( *this->_renderableList )[ this->renderable.num ];
    }
  break;
  }

  return NULL;
}//GetRenderable



/*
=============
  EnableCollision
=============
*/
Collision* Object::EnableCollision()
{
  if( this->collision )
    return this->collision;

  if( !__collisionList )
    return NULL;

  this->collision = new Collision( &this->positionSrc );
  __collisionList->push_back( this->collision );

  return this->collision;
}//EnableCollision




/*
=============
  DisableCollision
=============
*/
void Object::DisableCollision()
{
  if( !this->collision )
    return;

  if( !__collisionList )
    return;

  for( CollisionList::iterator iter = __collisionList->begin(); iter != __collisionList->end(); ++iter )
    if( *iter == this->collision )
    {
      __collisionList->erase( iter );
      this->collision = NULL;
      break;
    }
}//DisableCollision



/*
=============
  GetCollision
=============
*/
Collision* Object::GetCollision()
{
  return this->collision;
}//GetCollision



/*
=============
  GetMatrixTransform
=============
*/
const Mat4& Object::GetMatrixTransform()
{
  return this->matrixTransform;
}//GetMatrixTransform




/*
=============
  _RecalculatePosition
=============
*/
void Object::_RecalculatePosition()
{
  //объект всегда позиционируется относительно родителя
  if( this->_parent )
  {
    //TODO: здесь надо матричными преобразованиями делать пересчет с учетом позиции, масштаба и поворота родителя
    /*
    this->matrixTransform = Mat4(
      1.0f, 0.0f, 0.0f, this->positionSrc.x,
      0.0f, 1.0f, 0.0f, this->positionSrc.y,
      0.0f, 0.0f, 1.0f, this->positionSrc.z,
      0.0f, 0.0f, 0.0f, 1.0f
      ) * this->_parent->GetMatrixTransform();
    this->position = this->positionSrc * this->matrixTransform;
    */
    this->position = this->_parent->GetPosition() + this->positionSrc;
  }
  else
  {
    /*
    this->matrixTransform = Mat4(
      1.0f, 0.0f, 0.0f, this->positionSrc.x,
      0.0f, 1.0f, 0.0f, this->positionSrc.y,
      0.0f, 0.0f, 1.0f, this->positionSrc.z,
      0.0f, 0.0f, 0.0f, 1.0f
      );
    this->position = this->positionSrc * this->matrixTransform;
    __log.PrintInfo( Filelevel_DEBUG, "root position: %3.3f; %3.3f; %3.3f", this->position.x, this->position.y, this->position.z );
    */
    this->position = this->positionSrc;
  }
}//_RecalculatePosition





/*
=============
  Update
=============
*/
void Object::Update( float dt )
{
  this->_RecalculatePosition();

  //обновляем спрайт
  switch( this->renderable.type )
  {
  case RENDERABLE_TYPE_QUAD:
    {
      RenderableQuad *renderable = ( RenderableQuad* ) this->GetRenderable();
      if( !renderable )
        break;
      renderable->SetPosition( this->position );
    }
  break;
  }

  //обновляем необходимую инфу в коллизиях
  if( this->collision )
  {
    Vec3 force( 0.0f, 0.0f, 0.0f );
    ObjectForceList::iterator iter, iterEnd = this->forces.end();
    for( iter = this->forces.begin(); iter != iterEnd; ++iter )
      force += iter->vec;
    //__log.PrintInfo( Filelevel_DEBUG, "force: %3.3f; %3.3f; %3.3f", force.x, force.y, force.z );
    this->collision->SetForce( force );
  }

  //обновляем дочерние объекты
  if( this->_childs && this->_childs->size() )
  {
    ObjectChilds::iterator iter, iterEnd = this->_childs->end();
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      ( *iter )->Update( dt );
  }
}//Update




/*
=============
  SetPosition
=============
*/
Object* Object::SetPosition( const Vec3& newPosition )
{
  this->positionSrc = newPosition;
  return this;
}//SetPosition




/*
=============
  SetPosition2D
=============
*/
Object* Object::SetPosition2D( const Vec2& newPosition )
{
  this->positionSrc.x = newPosition.x;
  this->positionSrc.y = newPosition.y;
  return this;
}//SetPosition2D




/*
=============
  SetForce
=============
*/
void Object::SetForce( long forceId, const Vec3& vec )
{
  ObjectForceList::iterator iter, iterEnd = this->forces.end();
  bool changed = false;

  for( iter = this->forces.begin(); iter != iterEnd; ++iter )
    if( iter->id == forceId )
    {
      iter->vec = vec;
      changed = true;
      break;
    }
  if( !changed )
    this->forces.push_back( ObjectForce( forceId, vec ) );
}//SetForce



/*
=============
  RemoveForce
=============
*/
void Object::RemoveForce( long forceId )
{
  ObjectForceList::iterator iter, iterEnd;
  bool done = false;

  while( !done )
  {
    done = true;
    iterEnd = this->forces.end();
    for( iter = this->forces.begin(); iter != iterEnd; ++iter )
      if( iter->id == forceId )
      {
        this->forces.erase( iter );
        done = false;
        break;
      }
  }
}//RemoveForce




/*
=============
  SaveToBuffer
=============
*/
void Object::SaveToBuffer( MemoryWriter &writer )
{
  bool isRenderable, isCollision;

  isRenderable = this->IsRenderable();
  writer << isRenderable;  //renderable true/false

  isCollision = this->IsCollision();
  writer << isCollision;  //collision true/false

  writer << this->GetName();
  writer << this->GetParentNameFull();
  writer << this->GetPosition();

  if( isRenderable )
    ( ( RenderableQuad* ) this->GetRenderable() )->SaveToBuffer( writer );

  if( isCollision )
    this->GetCollision()->SaveToBuffer( writer );

  Dword childsCount = ( this->_childs ? this->_childs->size() : 0 );

  if( childsCount )
  {
    ObjectChilds::iterator iter, iterEnd = this->_childs->end();
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      ( *iter )->SaveToBuffer( writer );
  }
}//SaveToBuffer
