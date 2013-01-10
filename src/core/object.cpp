#include "object.h"
#include "file.h"
#include "tools.h"

extern CoreRenderableList *__coreRenderableList;
extern CoreRenderableListIndicies *__coreRenderableListIndicies;
extern CoreRenderableListIndicies *__coreRenderableListFreeIndicies;
extern CoreRenderableList *__coreGUI;
extern CoreRenderableListIndicies *__coreGUIIndicies;
extern CoreRenderableListIndicies *__coreGUIFreeIndicies;

ObjectByCollisionList *__objectByCollision  = NULL;
ObjectByTriggerList   *__objectByTrigger    = NULL;


/*
=============
  ObjectByCollision
=============
*/
ObjectByCollision::ObjectByCollision()
:object(), collision( NULL )
{
}//constructor

ObjectByCollision::ObjectByCollision( Object *setObject, Collision *setCollision )
:object( setObject ), collision( setCollision )
{
  if( setObject )
    setObject->PointerAdd( &this->object );
}//constructor

ObjectByCollision::ObjectByCollision( const ObjectByCollision& copyFrom )
{
  this->collision = copyFrom.collision;
  this->object.Init( copyFrom.object.GetObject() );
}//copy-constructor


ObjectByCollision::~ObjectByCollision()
{
  if( this->object.GetValid() )
    this->object.GetObject()->PointerRemove( &this->object );
}//destructor



/*
=============
  ObjectByTrigger
=============
*/
ObjectByTrigger::ObjectByTrigger()
:object(), trigger( NULL )
{
}//constructor


ObjectByTrigger::ObjectByTrigger( Object *setObject, ObjectTrigger *setTrigger )
:object( setObject ), trigger( setTrigger )
{
  if( setObject )
    setObject->PointerAdd( &this->object );
}//constructor

ObjectByTrigger::ObjectByTrigger( const ObjectByTrigger& copyFrom )
{
  this->trigger = copyFrom.trigger;
  this->object.Init( copyFrom.object.GetObject() );
}//copy-constructor

ObjectByTrigger::~ObjectByTrigger()
{
  if( this->object.GetValid() )
    this->object.GetObject()->PointerRemove( &this->object );
}//destructor



/*
=============
  ObjectRenderableInfo
=============
*/
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









/*
=============
  Object
=============
*/
Object::Object()
:name( "" ), nameFull( "" ), _parent( NULL ), _childs( NULL ), renderable( -1, RENDERABLE_TYPE_UNKNOWN ), collision( NULL )
,position( 0.0f, 0.0f, 0.0f ), positionSrc( 0.0f, 0.0f, 0.0f ), _renderableList( NULL ), trigger( NULL ), _isLockedToDelete( false )
{
  __log.PrintInfo( Filelevel_DEBUG, "Object dummy +1 => this[x%X]", this );
}//constructor


Object::Object( const std::string &objectName, Object* parentObject )
:name( objectName ), _parent( parentObject ), _childs( NULL ), renderable( -1, RENDERABLE_TYPE_UNKNOWN ), collision( NULL )
,position( 0.0f, 0.0f, 0.0f ), positionSrc( 0.0f, 0.0f, 0.0f ), _renderableList( NULL ), trigger( NULL ), _isLockedToDelete( false )
{
  if( this->_parent )
  {
    this->nameFull = ( this->_parent->_parent ? this->_parent->GetNameFull() + "/" : "" ) + this->name;
    this->_parent->AttachChildObject( this );
  }
  else
    this->nameFull = "/" + this->name;
  __log.PrintInfo( Filelevel_DEBUG, "Object +1 => this[x%X] parent[x%X] name['%s']", this, this->_parent, this->nameFull.c_str() );
}//constructor




Object::~Object()
{
  __log.PrintInfo( Filelevel_DEBUG, "Object -1 => this[x%X] name['%s'] pointers[%d]", this, this->GetNameFull().c_str(), this->pointers.size() );
  //if( !this->_parent )
  //  __log.PrintInfo( Filelevel_DEBUG, "is a root object" );

  if( this->pointers.size() )
  {
    __log.PrintInfo( Filelevel_DEBUG, ". object[x%X] name['%s'] pointers[%d]", this, this->GetNameFull().c_str(), this->pointers.size() );
    ObjectPointers::iterator iter, iterEnd = this->pointers.end();
    for( iter = this->pointers.begin(); iter != iterEnd; ++iter )
    {
      __log.PrintInfo( Filelevel_DEBUG, ". pointer[x%X]", *iter );
      ( *iter )->SetValid( false );
    }
  }

  this->DisableRenderable();
  this->DisableCollision();
  this->DisableTrigger();
  this->ClearChilds();
  this->UnAttachThisFromParent();

  __log.PrintInfo( Filelevel_DEBUG, "Object x%X deleted", this );
}//destructor



/*
=============
  ClearChilds
  �������� ���������� ���� ��������
  forceDelete - ������������ ���� _isLockedToDelete
=============
*/
void Object::ClearChilds( bool forceDelete )
{
  if( !this->_childs )
    return;

  __log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => objects[%d]", this->_childs->size() );
  if( forceDelete ) //������ �������
  {
    ObjectChilds::iterator iter;
    while( this->_childs->size() )
      delete *this->_childs->begin();

    DEF_DELETE( this->_childs );
  }
  else  //�������� ������ ������������
  {
    __log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => not forced" );
    bool deleted;
    ObjectChilds::iterator iter, iterEnd;
    do
    {
      deleted = false;

      iterEnd = this->_childs->end();
      __log.PrintInfo( Filelevel_DEBUG, ". iteration +1" );
      for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      {
        __log.PrintInfo( Filelevel_DEBUG, ".   child[x%X]", *iter );
        if( !( *iter )->IsLockedToDelete() )
        {
          __log.PrintInfo( Filelevel_DEBUG, ". deleted '%s'", ( *iter )->GetNameFull().c_str() );
          deleted = true;
          Object *tmpChild = *iter;
          this->_childs->erase( iter );
          delete tmpChild;
          break;
        }
      }
    } while( deleted );
    if( !this->_childs->size() )
      DEF_DELETE( this->_childs );
  }
  __log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => Done" );
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
  ��������� ������ � ������, ��� ���� ���������� ������� ��� �� ����� ����������� - ����, ������, �����, �����, �����, ��� � �.�.
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
  ��������� ������ � ������ GUI
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

  //__objectByCollision->push_back( ObjectByCollision( this, this->collision ) );
  /*
  __objectByCollision->push_back( ObjectByCollision() );
  ObjectByCollision *object = &( *__objectByCollision->rbegin() );
  object->collision = this->collision;
  object->object.Init( this );
  */

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

  __log.PrintInfo( Filelevel_DEBUG, "Object::DisableCollision..." );
  for( CollisionList::iterator iter = __collisionList->begin(); iter != __collisionList->end(); ++iter )
    if( *iter == this->collision )
    {
      __collisionList->erase( iter );
      this->collision = NULL;
      break;
    }

    /*
  ObjectByCollisionList::iterator iter, iterEnd = __objectByCollision->end();
  for( iter = __objectByCollision->begin(); iter != iterEnd; ++iter )
    if( iter->object.GetObject() == this )
    {
      __objectByCollision->erase( iter );
      break;
    }
    */
  __log.PrintInfo( Filelevel_DEBUG, "Object::DisableCollision done" );
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
  //������ ������ ��������������� ������������ ��������
  if( this->_parent )
  {
    //TODO: ����� ���� ���������� ���������������� ������ �������� � ������ �������, �������� � �������� ��������
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

  //��������� ������
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

  //��������� ����������� ���� � ���������
  if( this->collision )
  {
    Vec3 force( 0.0f, 0.0f, 0.0f );
    ObjectForceList::iterator iter, iterEnd = this->forces.end();
    for( iter = this->forces.begin(); iter != iterEnd; ++iter )
      force += iter->vec;
    //__log.PrintInfo( Filelevel_DEBUG, "force: %3.3f; %3.3f; %3.3f", force.x, force.y, force.z );
    this->collision->SetForce( force );
  }

  //��������� �������� �������
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

  writer << childsCount;
  if( childsCount )
  {
    ObjectChilds::iterator iter, iterEnd = this->_childs->end();
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      ( *iter )->SaveToBuffer( writer );
  }
}//SaveToBuffer




/*
=============
  LoadFromBuffer
=============
*/
void Object::LoadFromBuffer( MemoryReader &reader, Object *rootObject )
{
  bool isRenderable, isCollision;
  std::string tmpName, parentName;
  Vec3 position;

  reader >> isRenderable;
  reader >> isCollision;
  reader >> tmpName;
  reader >> parentName;
  reader >> position;
  __log.PrintInfo( Filelevel_DEBUG, "Object::LoadFromBuffer => name['%s'] parent['%s']", tmpName.c_str(), parentName.c_str() );

  this->name = tmpName;
  this->_parent = ( rootObject->GetNameFull() == parentName ? rootObject: rootObject->GetObject( parentName ) );

  if( this->_parent )
  {
    this->nameFull = ( this->_parent->_parent ? this->_parent->GetNameFull() + "/" : "" ) + this->name;
    this->_parent->AttachChildObject( this );
  }
  else
    this->nameFull = "/" + this->name;

  this->SetPosition( position );

  if( isRenderable )
    ( ( RenderableQuad* ) this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->LoadFromBuffer( reader );

  if( isCollision )
    this->EnableCollision()->LoadFromBuffer( reader );

  Dword childsCount;
  reader >> childsCount;
  if( childsCount )
  {
    __log.PrintInfo( Filelevel_WARNING, "Object::LoadFromBuffer => childs %d", childsCount );
  }
}//LoadFromBuffer




/*
=============
  GetObject
=============
*/
Object* Object::GetObject( const std::string& name, Object *parent )
{
  if( !name.length() )
  {
    __log.PrintInfo( Filelevel_WARNING, "Object::GetObject => name is NULL" );
    return NULL;
  }

  if( !parent )
  {
    parent = this->_parent;
    while( parent->_parent )
      parent = parent->_parent;
  }

  long slashPos = name.find_first_of( "/" );
  if( slashPos == 0 ) //���������� �� ����� => 1 ��� 3
    return this->GetObject( &name[ 1 ], parent );
  else
  {
    if( slashPos < 0 )  //��� ������ => 2 ��� 4
    {
      return parent->GetChild( name );
    }
    else  //����� ���� => 1 ��� 3
    {
      std::string nextLevel = &name[ slashPos ];
      std::string currentLevel = name.substr( 0, slashPos );
      if( !parent )
      {
        parent = this->_parent;
        while( parent->_parent )
          parent = parent->_parent;
      }
      Object *obj = parent->GetChild( currentLevel );
      if( !obj )
        return NULL;
      return this->GetObject( nextLevel, obj );
    }
  }

  return NULL;
}//GetObject



/*
=============
  GetObjectInPoint
=============
*/
Object* Object::GetObjectInPoint( const Vec2& pos )
{
  if( this->IsRenderable() && this->GetRenderable()->IsHasPoint( pos ) )
    return this;

  if( this->_childs )
  {
    ObjectChilds::iterator iter, iterEnd = this->_childs->end();
    Object *obj;
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
    {
      obj = ( *iter )->GetObjectInPoint( pos );
      if( obj )
        return obj;
    }
  }

  return NULL;
}//GetObjectInPoint




/*
=============
  PointerAdd
=============
*/
void Object::PointerAdd( ObjectPointer *pointer )
{
  __log.PrintInfo( Filelevel_DEBUG, "Object::PointerAdd => this[x%X] pointer[x%X]", this, pointer );
  bool pointerExists = false;
  ObjectPointers::iterator iter, iterEnd = this->pointers.end();
  for( iter = this->pointers.begin(); iter != iterEnd; ++iter )
    if( *iter == pointer )
    {
      pointerExists = true;
      __log.PrintInfo( Filelevel_WARNING, ". pointer already in list" );
      break;
    }
  if( !pointerExists )
    this->pointers.push_back( pointer );
}//PointerAdd



/*
=============
  PointerRemove
=============
*/
void Object::PointerRemove( ObjectPointer *pointer )
{
  bool removed = false;
  ObjectPointers::iterator iter, iterEnd = this->pointers.end();
  for( iter = this->pointers.begin(); iter != iterEnd; ++iter )
    if( *iter == pointer )
    {
      this->pointers.erase( iter );
      removed = true;
      break;
    }
  if( !removed )
    __log.PrintInfo( Filelevel_WARNING, "Object::PointerRemove => pointer[x%X] not found in list of object[x%X]", pointer, this );
}//PointerRemove




/*
=============
  EnableTrigger
=============
*/
ObjectTrigger* Object::EnableTrigger()
{
  if( this->trigger )
    return this->trigger;

  if( !__triggerList )
    return NULL;

  this->trigger = new ObjectTrigger( &this->position );
  __triggerList->push_back( this->trigger );

  //__objectByTrigger->push_back( ObjectByTrigger( this, this->trigger ) );
  __objectByTrigger->push_back( ObjectByTrigger() );
  ObjectByTrigger *object = &( *__objectByTrigger->rbegin() );
  object->trigger = this->trigger;
  object->object.Init( this );

  __log.PrintInfo( Filelevel_DEBUG, "Object::EnableTrigger => x%X", this->trigger );

  return this->trigger;
}//EnableTrigger




/*
=============
  DisableTrigger
=============
*/
void Object::DisableTrigger()
{
  if( !this->trigger )
    return;

  if( !__triggerList )
    return;

  ObjectTriggerList::iterator iterEnd = __triggerList->end();
  for( ObjectTriggerList::iterator iter = __triggerList->begin(); iter != iterEnd; ++iter )
    if( *iter == this->trigger )
    {
      __triggerList->erase( iter );
      //this->trigger = NULL;
      break;
    }

  DEF_DELETE( this->trigger );

  ObjectByTriggerList::iterator iterTrigger, iterEndTrigger = __objectByTrigger->end();
  for( iterTrigger = __objectByTrigger->begin(); iterTrigger != iterEndTrigger; ++iterTrigger )
    if( iterTrigger->object.GetObject() == this )
    {
      __objectByTrigger->erase( iterTrigger );
      break;
    }
}//DisableTrigger



/*
=============
  GetTrigger
=============
*/
ObjectTrigger* Object::GetTrigger()
{
  return this->trigger;
}//GetTrigger
