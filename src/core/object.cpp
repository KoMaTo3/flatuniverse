#include "object.h"
#include "file.h"
#include "tools.h"
#include "animationpack.h"
#include "lightrenderer.h"
//#include "glui2/glui2.h"

extern CoreRenderableList *__coreRenderableList;
extern CoreRenderableListIndicies *__coreRenderableListIndicies;
extern CoreRenderableListIndicies *__coreRenderableListFreeIndicies;
extern CoreRenderableList *__coreGUI;
extern CoreRenderableListIndicies *__coreGUIIndicies;
extern CoreRenderableListIndicies *__coreGUIFreeIndicies;
extern LightRenderer *__lightLenderer;
//extern GuiList *__guiList;
//extern Glui2 *__coreGlui;

ObjectByCollisionList *__objectByCollision  = NULL;
ObjectByTriggerList   *__objectByTrigger    = NULL;
//ObjectByGuiList       *__objectByGui        = NULL;

Object::ObjectEvent *Object::OnLoad = NULL;
Object::ObjectEvent *Object::OnUnload = NULL;


/*
=============
  ObjectByCollision
=============
*/
ObjectByCollision::ObjectByCollision( Object *setObject, Collision *setCollision )
:object( setObject ), collision( setCollision )
{
  if( setObject ) {
    //__log.PrintInfo( Filelevel_DEBUG, "ObjectByCollision => PointerAdd[x%p] this[x%p]", &this->object, this );
    //setObject->PointerAdd( &this->object );
    __objectByCollision->push_back( this );
  }
}//constructor

ObjectByCollision::ObjectByCollision( const ObjectByCollision& copyFrom )
:object( copyFrom.object.GetObject< Object >() )
{
  __log.PrintInfo( Filelevel_WARNING, "ObjectByCollision::ObjectByCollision => copy-constructor" );
  this->collision = copyFrom.collision;
}//copy-constructor


ObjectByCollision::~ObjectByCollision()
{
  ObjectByCollisionList::iterator iter, iterEnd = __objectByCollision->end();
  for( iter = __objectByCollision->begin(); iter != iterEnd; ++iter ) {
    if( *iter == this ) {
      __objectByCollision->erase( iter );
      break;
    }
  }
}//destructor



/*
=============
  ObjectByTrigger
=============
*/
ObjectByTrigger::ObjectByTrigger( Object *setObject, ObjectTrigger *setTrigger )
:object( setObject ), trigger( setTrigger )
{
  if( setObject ) {
    //__log.PrintInfo( Filelevel_DEBUG, "ObjectByTrigger => PointerAdd[x%p] this[x%p]", &this->object, this );
    __objectByTrigger->push_back( this );
    //setObject->PointerAdd( &this->object );
  }
}//constructor

ObjectByTrigger::ObjectByTrigger( const ObjectByTrigger& copyFrom )
:object( copyFrom.object.GetObject< Object >() )
{
  this->trigger = copyFrom.trigger;
}//copy-constructor

ObjectByTrigger::~ObjectByTrigger()
{
  ObjectByTriggerList::iterator iter, iterEnd = __objectByTrigger->end();
  for( iter = __objectByTrigger->begin(); iter != iterEnd; ++iter ) {
    if( *iter == this ) {
      __objectByTrigger->erase( iter );
      break;
    }
  }

  //if( this->object.GetValid() )
  //  this->object.GetObject()->PointerRemove( &this->object );
}//destructor



/*
=============
  ObjectByGui
=============
*/
/*
ObjectByGui::ObjectByGui()
:object( NULL ), gui( NULL )
{
}//constructor

ObjectByGui::ObjectByGui( Object *setObject, g2Controller *setGui )
:object( setObject ), gui( setGui )
{
  if( setObject ) {
    __log.PrintInfo( Filelevel_DEBUG, "ObjectByGui => PointerAdd[x%p]", &this->object );
    setObject->PointerAdd( &this->object );
  }
}//constructor

ObjectByGui::ObjectByGui( const ObjectByGui& copyFrom )
:object( copyFrom.object.GetObject() )
{
  this->gui = copyFrom.gui;
}//copy-constructor


ObjectByGui::~ObjectByGui()
{
  if( this->object.GetValid() )
    this->object.GetObject()->PointerRemove( &this->object );
}//destructor
*/



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
,tags( NULL ), isEnabled( true ), isEnabledPrev( true ), isSaveable( true )
{
  //this->gui.type = OBJECT_GUI_UNKNOWN;
  this->PointerBind( this );
  //__log.PrintInfo( Filelevel_DEBUG, "Object dummy +1 => this[x%p]", this );
}//constructor


Object::Object( const std::string &objectName, Object* parentObject, bool setIsSaveable )
:name( objectName ), _parent( parentObject ), _childs( NULL ), renderable( -1, RENDERABLE_TYPE_UNKNOWN ), collision( NULL )
,position( 0.0f, 0.0f, 0.0f ), positionSrc( 0.0f, 0.0f, 0.0f ), _renderableList( NULL ), trigger( NULL ), _isLockedToDelete( false )
,tags( NULL ), isEnabled( true ), isEnabledPrev( true ), isSaveable( setIsSaveable )
{
  //this->gui.type = OBJECT_GUI_UNKNOWN;
  this->PointerBind( this );
  if( this->_parent )
  {
    this->nameFull = ( this->_parent->_parent ? this->_parent->GetNameFull() + "/" : "" ) + this->name;
    this->_parent->AttachChildObject( this );
  }
  else
    this->nameFull = "/" + this->name;
  //__log.PrintInfo( Filelevel_DEBUG, "Object +1 => this[x%p] parent[x%p] nameFull['%s']", this, this->_parent, this->nameFull.c_str() );
}//constructor




Object::~Object()
{
  //__log.PrintInfo( Filelevel_DEBUG, "Object -1 => this[x%p] name['%s']", this, this->GetNameFull().c_str() );
  //if( !this->_parent )
  //  __log.PrintInfo( Filelevel_DEBUG, "is a root object" );

  this->DisableRenderable();
  this->DisableCollision();
  this->DisableTrigger();
  //this->DisableGui();
  this->ClearChilds();
  this->UnAttachThisFromParent();

  DEF_DELETE( this->tags );

  //__log.PrintInfo( Filelevel_DEBUG, "Object x%p deleted", this );
}//destructor



/*
=============
  ClearChilds
  Удаление рекурсивно всех потомков
  forceDelete - игнорировать флаг _isLockedToDelete
=============
*/
void Object::ClearChilds( bool forceDelete )
{
  if( !this->_childs )
    return;

  //__log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => objects[%d]", this->_childs->size() );
  if( forceDelete ) //полная очистка
  {
    ObjectList::iterator iter;
    while( this->_childs->size() )
      delete *this->_childs->begin();

    DEF_DELETE( this->_childs );
  }
  else  //удаление только незалоченных
  {
    //__log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => not forced" );
    bool deleted;
    ObjectList::iterator iter, iterEnd;
    do
    {
      deleted = false;

      iterEnd = this->_childs->end();
      for( iter = this->_childs->begin(); iter != iterEnd; ++iter )
      {
        if( !( *iter )->IsLockedToDelete() )
        {
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
  //__log.PrintInfo( Filelevel_DEBUG, "Object::ClearChilds => Done" );
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
  //__log.PrintInfo( Filelevel_DEBUG, "Object::AttachChildObject => this[x%p] child[x%p]", this, newChild );
  if( newChild && !this->IsChild( newChild ) )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "Object::AttachChildObject => '%s' now is child of '%s'", newChild->GetNameFull().c_str(), this->GetNameFull().c_str() );
    if( !this->_childs )
      this->_childs = new ObjectList();
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
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachThisFromParent => this[x%p]", this );
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
  //__log.PrintInfo( Filelevel_DEBUG, "Object::UnAttachChildObject => this[x%p] child[x%p]", this, child );
  if( !child )
    return;
  if( this->_childs && this->IsChild( child ) )
  {
    child->_parent = NULL;
    ObjectList::iterator iter, iterEnd = this->_childs->end();
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

  for( ObjectList::iterator iter = this->_childs->begin(); iter != this->_childs->end(); ++iter )
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
    //__log.PrintInfo( Filelevel_WARNING, "Object::GetChild => no childs in '%s'", this->GetNameFull().c_str() );
    return NULL;
  }

  //__log.PrintInfo( Filelevel_DEBUG, "Object::GetChild => name['%s'] thisName['%s']", name.c_str(), this->GetNameFull().c_str() );
  for( ObjectList::iterator iter = this->_childs->begin(); iter != this->_childs->end(); ++iter )
    if( ( *iter )->GetName() == name )
      return *iter;

  //__log.PrintInfo( Filelevel_WARNING, "Object::GetChild => '%s' not found in '%s'", name.c_str(), this->GetNameFull().c_str() );

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
      result = Object::CreateExternalRenderableInList( zIndex, this->_renderableList, this->_renderableIndicies, this->_renderableFreeIndicies, &this->renderable.num );
      /*
      __log.PrintInfo( Filelevel_DEBUG, "Free indicies in x%p = %d", this->_renderableFreeIndicies, this->_renderableFreeIndicies->size() );
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
        */
    }
  break;
  default:
    this->renderable.type = RENDERABLE_TYPE_UNKNOWN;
    return NULL;
  break;
  }

  return result;
}//EnableRenderable



RenderableQuad* Object::CreateExternalRenderableInList( float zIndex, CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort *outIndex ) {
  //__log.PrintInfo( Filelevel_DEBUG, "Object::CreateExternalRenderableInList => pointers[%p; %p; %p] index[%3.3f]", inRenderableList, inRenderableIndicies, inRenderableFreeIndicies, zIndex );
  Renderable *result = NULL;
  if( !inRenderableList ) {
    inRenderableList = __coreRenderableList;
    inRenderableIndicies = __coreRenderableListIndicies;
    inRenderableFreeIndicies = __coreRenderableListFreeIndicies;
  }

  GLushort index = RENDERABLE_INDEX_UNDEFINED;
  if( inRenderableFreeIndicies->size() )
  {
    index = *inRenderableFreeIndicies->rbegin();
    inRenderableFreeIndicies->pop_back();
    result = &( *( inRenderableList->begin() + index ) );
    *result = RenderableQuad();
  }
  else
  {
    inRenderableList->push_back( RenderableQuad() );
    result = &( *inRenderableList->rbegin() );
    index = inRenderableList->size() - 1;
  }

  RenderableQuad *quad = ( RenderableQuad* ) result;
  quad->SetPosition( Vec3( 0.0f, 0.0f, zIndex ) );
  float z = quad->GetPosition().z;
  if( outIndex ) {
    *outIndex = index;
  }

  CoreRenderableListIndicies::iterator iter, iterEnd = inRenderableIndicies->end(), iterBegin = inRenderableIndicies->begin();
  bool added = false;
  for( iter = iterBegin; iter != iterEnd; ++iter )
    if( z < ( *( inRenderableList->begin() + *iter ) ).GetPosition().z )
    {
      inRenderableIndicies->insert( iter, index );
      added = true;
      break;
    }
  if( !added )
    inRenderableIndicies->push_back( index );
  quad->SetIndexInRenderableList( index );

  //__log.PrintInfo( Filelevel_DEBUG, "CreateExternalRenderableInList => Done" );
  return ( RenderableQuad* ) result;
}//CreateExternalRenderableInList



bool Object::DestroyExternalRenderableInList( CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort index ) {
  if( !inRenderableList ) {
    inRenderableList = __coreRenderableList;
    inRenderableIndicies = __coreRenderableListIndicies;
    inRenderableFreeIndicies = __coreRenderableListFreeIndicies;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Object::DestroyExternalRenderableInList => pointers[%p; %p; %p] index[%d]", inRenderableList, inRenderableIndicies, inRenderableFreeIndicies, index );

  if( index == RENDERABLE_INDEX_UNDEFINED ) {
    return false;
  }

  if( ( long ) inRenderableList->size() <= index ) {
    return false;
  }

  //__coreRenderableList->erase( __coreRenderableList->begin() + this->renderable.num );

  CoreRenderableListIndicies::iterator iter, iterEnd = inRenderableIndicies->end();
  for( iter = inRenderableIndicies->begin(); iter != iterEnd; ++iter ) {
    if( *iter == index ) {
      //__log.PrintInfo( Filelevel_DEBUG, "add free index %d in list x%p", *iter, inRenderableFreeIndicies );
      inRenderableFreeIndicies->push_back( *iter );
      inRenderableIndicies->erase( iter );
      break;
    }
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Object::DisableRenderable => index[%d]", index );
  return true;
}//DestroyExternalRenderableInList





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
  __log.PrintInfo( Filelevel_DEBUG, "Free indicies in x%p = %d", this->_renderableFreeIndicies, this->_renderableFreeIndicies->size() );
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

  /*
  if( ( long ) this->_renderableList->size() <= this->renderable.num )
    return false;

  //__coreRenderableList->erase( __coreRenderableList->begin() + this->renderable.num );

  CoreRenderableListIndicies::iterator iter, iterEnd = this->_renderableIndicies->end();
  for( iter = this->_renderableIndicies->begin(); iter != iterEnd; ++iter )
    if( *iter == this->renderable.num )
    {
      __log.PrintInfo( Filelevel_DEBUG, "add free index %d in list x%p", *iter, this->_renderableFreeIndicies );
      this->_renderableFreeIndicies->push_back( *iter );
      this->_renderableIndicies->erase( iter );
      break;
    }
  __log.PrintInfo( Filelevel_DEBUG, "Object::DisableRenderable => index[%d]", this->renderable.num );
  */
  Object::DestroyExternalRenderableInList( this->_renderableList, this->_renderableIndicies, this->_renderableFreeIndicies, this->renderable.num );

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

  //__log.PrintInfo( Filelevel_DEBUG, "Object::EnableCollision => object['%s'] collision[x%p]", this->GetNameFull().c_str(), this->collision );

  //__log.PrintInfo( Filelevel_DEBUG, "Object::EnableCollision => ObjectByCollision( x%p, x%p )", this, this->collision );
  new ObjectByCollision( this, this->collision );
  //__objectByCollision->push_back( byCollision);

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

  //__log.PrintInfo( Filelevel_DEBUG, "Object::DisableCollision..." );
  for( CollisionList::iterator iter = __collisionList->begin(); iter != __collisionList->end(); ++iter )
    if( *iter == this->collision )
    {
      __collisionList->erase( iter );
      this->collision = NULL;
      break;
    }

  ObjectByCollisionList::iterator iterCollision, iterEndCollision = __objectByCollision->end();
  for( iterCollision = __objectByCollision->begin(); iterCollision != iterEndCollision; ++iterCollision ) {
    if( ( *iterCollision )->object.GetObject< Object >() == this ) {
      delete *iterCollision;
      //__objectByCollision->erase( iterCollision );
      break;
    }
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
  //__log.PrintInfo( Filelevel_DEBUG, "Object::DisableCollision done" );
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
  EnableGui
=============
*/
/*
g2Controller* Object::EnableGui( const GuiConstructor *info )
{
  if( this->gui.guiController && this->gui.type != OBJECT_GUI_UNKNOWN || !info ) {
    this->gui.guiController->SetVisibility( true );
    //__log.PrintInfo( Filelevel_ERROR, "Object::EnableGui => unknown type x%p or already enabled[x%p]", info.type, this->gui.guiController );
    return this->gui.guiController;
  }

  if( !__guiList ) {
    __log.PrintInfo( Filelevel_ERROR, "Object::EnableGui => __guiList is NULL" );
    return NULL;
  }

  g2Panel *panel = NULL;
  if( info->panel.length() ) {
    Object *panelObject = this->GetObject( info->panel );
    if( !panelObject ) {
      __log.PrintInfo( Filelevel_WARNING, "Object::EnableGui => panel '%s' not found", info->panel.c_str() );
    } else {
      panel = ( g2Panel* ) panelObject->GetGui();
    }
  }

  switch( info->type ) {
    // !position
    // !label
    // ?width
    case OBJECT_GUI_BUTTON: {
      g2Button *item = NULL;
      if( panel ) {
        item = panel->AddButton( info->position.x, info->position.y, info->label.c_str(), Object::_GuiCallback );
      } else {
        item = __coreGlui->AddButton( info->position.x, info->position.y, info->label.c_str(), Object::_GuiCallback );
      }
      if( info->width ) {
        item->SetWidth( info->width );
      }
      this->gui.guiController = item;
      break;
    }//button

    // !position
    // !label
    case OBJECT_GUI_CHECKBOX: {
      g2CheckBox *item = NULL;
      if( panel ) {
        item = panel->AddCheckBox( info->position.x, info->position.y, info->label.c_str(), Object::_GuiCallback );
      } else {
        item = __coreGlui->AddCheckBox( info->position.x, info->position.y, info->label.c_str(), Object::_GuiCallback );
      }
      this->gui.guiController = item;
      break;
    }//checkbox

    // !position
    // !stringList
    // ?width
    case OBJECT_GUI_DROPDOWN: {
      g2DropDown *item = NULL;
      const char **list = NULL;
      int num = 0;
      if( info->stringList.size() ) {
        list = new const char*[ info->stringList.size() ];
        stdDequeString::const_iterator iter, iterEnd = info->stringList.end();
        for( iter = info->stringList.begin(); iter != iterEnd; ++iter, ++num ) {
          list[ num ] = &( *iter )[ 0 ];
        }
      }
      if( panel ) {
        item = panel->AddDropDown( info->position.x, info->position.y, list, info->stringList.size(), Object::_GuiCallback );
      } else {
        item = __coreGlui->AddDropDown( info->position.x, info->position.y, list, info->stringList.size(), Object::_GuiCallback );
      }
      DEF_DELETEARRAY( list );
      if( info->width ) {
        item->SetWidth( info->width );
      }
      this->gui.guiController = item;
      break;
    }//dropdown

    // !position
    // !label
    // ?color
    case OBJECT_GUI_LABEL: {
      g2Label *item = NULL;
      if( panel ) {
        item = panel->AddLabel( info->position.x, info->position.y, info->label.c_str() );
      } else {
        item = __coreGlui->AddLabel( info->position.x, info->position.y, info->label.c_str() );
      }
      item->SetColor( info->color.x, info->color.y, info->color.z, info->color.w );
      this->gui.guiController = item;
      break;
    }//label

    // !position
    // !label
    // ?width
    case OBJECT_GUI_PANEL: {
      g2Panel *item = __coreGlui->AddPanel( info->panelAnchor, info->label.c_str() );
      item->SetPanelPos( info->position.x, info->position.y );
      if( info->width ) {
        item->SetSize( info->width, info->height );
      }
      this->gui.guiController = item;
      break;
    }//panel

    // !position
    // !label
    // ?width
    case OBJECT_GUI_TEXTFIELD: {
      g2TextField *item = NULL;
      if( panel ) {
        item = panel->AddTextField( info->position.x, info->position.y, info->label.c_str() );
      } else {
        item = __coreGlui->AddTextField( info->position.x, info->position.y, info->label.c_str() );
      }
      if( info->width ) {
        item->SetWidth( info->width );
      }
      this->gui.guiController = item;
      break;
    }//textfield

    // !position
    // !spinnerType
    // ?width
    case OBJECT_GUI_SPINNER: {
      g2Spinner *item = NULL;
      if( panel ) {
        item = panel->AddSpinner( info->position.x, info->position.y, info->spinnerType, Object::_GuiCallback );
      } else {
        item = __coreGlui->AddSpinner( info->position.x, info->position.y, info->spinnerType, Object::_GuiCallback );
      }
      item->SetBounds( info->bounds.min, info->bounds.max );
      if( info->width ) {
        item->SetWidth( info->width );
      }
      this->gui.guiController = item;
      break;
    }//spinner

    default: {
      __log.PrintInfo( Filelevel_ERROR, "Object::EnableGui => unknown type x%p", info->type );
      return NULL;
    }
  }//switch
  this->gui.type = info->type;
  this->gui.funCallback = info->funCallback;
  __guiList->push_back( this->gui.guiController );
  __log.PrintInfo( Filelevel_DEBUG, "Object::EnableGui => controller[x%p]", this->gui.guiController );

  ObjectByGui *byGui = new ObjectByGui( this, this->gui.guiController );
  __objectByGui->push_back( byGui );
  this->_isLockedToDelete = true;

  return this->gui.guiController;
}//EnableGui
*/




/*
=============
  DisableGui
=============
*/
/*
void Object::DisableGui()
{
  if( !this->gui.guiController || this->gui.type == OBJECT_GUI_UNKNOWN )
    return;

  if( !__guiList )
    return;


  this->gui.guiController->SetVisibility( false );
}//DisableGui
*/



/*
=============
  GetGui
=============
*/
/*
g2Controller* Object::GetGui()
{
  return this->gui.guiController;
}//GetGui
*/



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
  if( this->isEnabled != this->isEnabledPrev ) {
    if( this->IsRenderable() ) {
      this->GetRenderable()->SetEnabled( this->isEnabled );
    }
    if( !this->isEnabled ) { //disable all
      if( this->IsCollision() ) {
        this->DisableCollision();
      }
      if( this->IsTrigger() ) {
        this->DisableTrigger();
      }
    }
  }
  this->isEnabledPrev = this->isEnabled;
  if( !this->isEnabled ) {
    return;
  }
  this->_RecalculatePosition();

  //обновляем спрайт
  switch( this->renderable.type )
  {
  case RENDERABLE_TYPE_QUAD:
    {
      RenderableQuad *renderable = ( RenderableQuad* ) this->GetRenderable();
      if( !renderable ) {
        break;
      }
      renderable->SetPosition( Vec3( Math::Floor( this->position.x ), Math::Floor( this->position.y ), this->position.z ) );
      renderable->CheckChanges();
      /*
      if( *renderable->GetEnabledPtr() != *renderable->GetPrevEnabledPtr() ) {
        if( *renderable->GetEnabledPtr() ) {
        } else {
        }
      }
      */
    }
  break;
  }

  //обновляем необходимую инфу в коллизиях
  if( this->collision )
  {
    Vec3 force( 0.0f, 0.0f, 0.0f );
    ObjectForceList::iterator iter, iterEnd = this->forces.end();
    for( iter = this->forces.begin(); iter != iterEnd; ++iter ) {
      force += iter->vec;
    }
    //__log.PrintInfo( Filelevel_DEBUG, "force: %3.3f; %3.3f; %3.3f", force.x, force.y, force.z );
    this->collision->SetForce( force );
  }

  this->widget->TouchEvent( ObjectWidget::OBJECT_WIDGET_EVENT_UPDATE );

  //обновляем дочерние объекты
  if( this->_childs && this->_childs->size() )
  {
    ObjectList::iterator iter, iterEnd = this->_childs->end();
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter ) {
      ( *iter )->Update( dt );
    }
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
  if( this->OnUnload ) {
    this->OnUnload( this );
  }

  bool isRenderable, isCollision, isTrigger, isAnimation, isLightBlockByCollision;

  isRenderable = this->IsRenderable();
  writer << isRenderable;  //renderable true/false

  isCollision = this->IsCollision();
  writer << isCollision;  //collision true/false

  isTrigger = this->IsTrigger();
  writer << isTrigger;    //trigger true/false

  writer << this->GetName();
  writer << this->GetParentNameFull();
  writer << this->GetPosition();

  if( isRenderable )
    ( ( RenderableQuad* ) this->GetRenderable() )->SaveToBuffer( writer );

  if( isCollision )
    this->GetCollision()->SaveToBuffer( writer );

  if( isTrigger )
    this->GetTrigger()->SaveToBuffer( writer );

  //tags
  if( !this->tags ) {
    writer << ( Dword ) 0;
  } else {
    this->tags->SaveToBuffer( writer );
  }

  //childs
  Dword childsCount = 0;
  if( !this->_childs ) {
    writer << childsCount;
  } else {
    for( auto &child: *this->_childs ) {
      if( child->isSaveable ) {
        ++childsCount;
      }
    }

    writer << childsCount;
    if( childsCount )
    {
      ObjectList::iterator iter, iterEnd = this->_childs->end();
      for( iter = this->_childs->begin(); iter != iterEnd; ++iter ) {
        if( ( *iter )->isSaveable ) {
          ( *iter )->SaveToBuffer( writer );
        }
      }
    }
  }

  //animation
  isAnimation = ( this->_animation != NULL );
  writer << isAnimation;
  if( isAnimation ) {
    this->_animation->SaveToBuffer( writer );
  }

  //scripts
  //тут нужна сериализация объекта посредством вызова lua-функции
  writer << this->luaScript;

  //Width::LightBlockByCollision
  isLightBlockByCollision = this->widget->WidgetExists( ObjectWidget::OBJECT_WIDGET_LIGHTBLOCKBYCOLLISION );
  writer << isLightBlockByCollision;
}//SaveToBuffer




/*
=============
  LoadFromBuffer
=============
*/
void Object::LoadFromBuffer( MemoryReader &reader, Object *rootObject, const Dword version )
{
  bool isRenderable, isCollision, isTrigger, isAnimation, isLightBlockByCollision;
  std::string tmpName, parentName;
  Vec3 position;

  reader >> isRenderable;
  reader >> isCollision;
  reader >> isTrigger;
  reader >> tmpName;
  reader >> parentName;
  reader >> position;
  //__log.PrintInfo( Filelevel_DEBUG, "Object::LoadFromBuffer => name['%s'] parent['%s']", tmpName.c_str(), parentName.c_str() );

  this->name = tmpName;
  this->_parent = ( rootObject->GetNameFull() == parentName ? rootObject: rootObject->GetObject( parentName ) );

  if( this->_parent ) {
    this->nameFull = ( this->_parent->_parent ? this->_parent->GetNameFull() + "/" : "" ) + this->name;
    this->_parent->AttachChildObject( this );
  } else {
    this->nameFull = "/" + this->name;
  }

  this->SetPosition( position );

  if( isRenderable ) {
    ( ( RenderableQuad* ) this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->LoadFromBuffer( reader );
  }

  if( isCollision ) {
    this->EnableCollision()->LoadFromBuffer( reader, this->nameFull, version );

    //test light block
    //this->widget->AddWidget( new ObjectWidget::WidgetLightBlockByCollision( this, __lightLenderer, this->GetCollision() ) );
  }

  if( isTrigger ) {
    this->EnableTrigger()->LoadFromBuffer( reader, this->nameFull, version );
  }

  //tags
  Dword tagsCount;
  reader >> tagsCount;
  if( tagsCount ) {
    reader.SeekFromCur( -1 * long( sizeof( tagsCount ) ) );
    this->tags = new Tags();
    this->tags->LoadFromBuffer( reader );
  }

  //childs
  Dword childsCount;
  reader >> childsCount;
  if( childsCount ) //TODO: надо допиливать подгрузку дочерних объектов
  {
    for( Dword num = 0; num < childsCount; ++num ) {
      Object *child = new Object( "", this );
      child->LoadFromBuffer( reader, this, version );
    }
    //__log.PrintInfo( Filelevel_WARNING, "Object::LoadFromBuffer => childs %d", childsCount );
  }

  if( version >= 0x00000006 ) {
    //animation
    reader >> isAnimation;
    if( isAnimation ) {
      std::string templateName = "", animationName = "";
      float startTime;
      reader >> templateName;
      reader >> animationName;
      reader >> startTime;
      __log.PrintInfo( Filelevel_DEBUG, "LoadFromBuffer => animationTemplate['%s'] name['%s'] time[%3.3f]", templateName.c_str(), animationName.c_str(), startTime );
      this->ApplyAnimation( templateName, animationName, startTime )->SetEnabled( true );
    }

    if( version >= 0x00000007 ) {
      reader >> this->luaScript;

      if( version >= 0x00000008 ) {
        reader >> isLightBlockByCollision;
        if( isLightBlockByCollision ) {
          this->EnableLightBlockByCollision();
        }
      }//8
    }//7
  }//6

  if( this->OnLoad ) {
    this->OnLoad( this );
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
  if( slashPos == 0 ) //начинается со слеша => 1 или 3
    return this->GetObject( &name[ 1 ], parent );
  else
  {
    if( slashPos < 0 )  //нет слешей => 2 или 4
    {
      return parent->GetChild( name );
    }
    else  //слеши есть => 1 или 3
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
    ObjectList::iterator iter, iterEnd = this->_childs->end();
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
  GetObjectsInRect
=============
*/
void Object::GetObjectsInRect( int type, const Vec2 &leftTop, const Vec2 &rightBottom, ObjectList& result )
{
  if( !this->_childs || !this->_childs->size() ) {
    return;
  }

  for( auto &child: *this->_childs ) {
    if( child->TestInRect( type, leftTop, rightBottom, true ) ) {
      if( child->IsSaveable() ) {
        result.push_back( &( *child ) );
      } else {
        Object *saveableObject = this;
        while( saveableObject && !saveableObject->IsSaveable() ) {
          saveableObject = saveableObject->GetParent();
        }
        if( saveableObject && saveableObject->GetParent() ) {
          bool inList = false;
          for( auto &objectinList: result ) {
            if( &( *objectinList ) == saveableObject ) {
              inList = true;
              break;
            }
          }//for
          if( !inList ) {
            result.push_back( saveableObject );
          }
        }
      }//not saveable
    }//in rect
  }//for
}//GetObjectsInRect




/*
=============
  TestInRect
=============
*/
bool Object::TestInRect( int type, const Vec2 &leftTop, const Vec2 &rightBottom, bool recursive ) {
  switch( type ) {
  case 0: //all
    if( tools::TestPointInRect2( Vec2( this->position.x, this->position.y ), leftTop, rightBottom ) ) {
      return true;
    }
  case 1: //renderable
    if( this->IsRenderable() && this->GetRenderable()->IsIntersectRect( leftTop, rightBottom ) ) {
      return true;
    }
    if( type == 1 ) {
      return false;
    }
  case 2: //collision
    if( this->IsCollision() && this->GetCollision()->TestIntersect( Vec3( leftTop.x, leftTop.y, 0.0f ), Vec3( rightBottom.x, rightBottom.y, 0.0f ) ) ) {
      return true;
    }
    if( type == 2 ) {
      return false;
    }
  case 3: //trigger
    if( this->IsTrigger() && this->GetTrigger()->TestIntersectRect( leftTop, rightBottom ) ) {
      return true;
    }
    if( type == 2 ) {
      return false;
    }
  }//switch

  if( recursive && this->_childs && this->_childs->size() ) {
    for( auto &child: *this->_childs ) {
      if( child->TestInRect( type, leftTop, rightBottom, recursive ) ) {
        return true;
      }
    }
  }
  return false;
}//TestInRect



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
  new ObjectByTrigger( this, this->trigger );
  //__objectByTrigger->push_back( byTrigger );

  __log.PrintInfo( Filelevel_DEBUG, "Object::EnableTrigger => x%p", this->trigger );

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
  for( iterTrigger = __objectByTrigger->begin(); iterTrigger != iterEndTrigger; ++iterTrigger ) {
    if( ( *iterTrigger )->object.GetObject< Object >() == this ) {
      delete *iterTrigger;
      //__objectByTrigger->erase( iterTrigger );
      break;
    }
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



/*
=============
  GetObjectByRenderableIndex
=============
*/
Object* Object::GetObjectByRenderableIndex( CoreRenderableList *renderableList, GLushort index )
{
  if( this->renderable.type != RENDERABLE_TYPE_UNKNOWN && this->renderable.num == index && this->_renderableList == renderableList ) {
    return this;
  }
  if( this->_childs && this->_childs->size() ) {
    __log.PrintInfo( Filelevel_DEBUG, "Object::GetObjectByRenderableIndex => this[x%p] name['%s'] index[%d]", this, this->GetNameFull().c_str(), index );
    ObjectList::iterator iter, iterEnd = this->_childs->end();
    Object *obj;
    for( iter = this->_childs->begin(); iter != iterEnd; ++iter ) {
      obj = ( *iter )->GetObjectByRenderableIndex( renderableList, index );
      if( obj ) {
        return obj;
      }
    }
  }
  return NULL;
}//GetObjectByRenderableIndex




/*
=============
  _GuiCallback
  Callback-function from Glui2 to Object
=============
*/
/*
void Object::_GuiCallback( g2Controller *controller )
{
  if( !controller || controller->GetDisabled() ) {
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Object::_GuiCallback => controller[x%p] objects[%d]", controller, __objectByGui->size() );
  ObjectByGuiList::iterator iter, iterEnd = __objectByGui->end();
  for( iter = __objectByGui->begin(); iter != iterEnd; ++iter ) if( !controller->GetDisabled() ) {
    __log.PrintInfo( Filelevel_DEBUG, ". controller[x%p] function['%s']", ( *iter )->gui, ( *iter )->object.GetObject< Object >()->GetNameFull().c_str() );
    if( ( *iter )->gui == controller && ( *iter )->object.GetValid() && ( *iter )->object.GetObject< Object >()->gui.funCallback ) {
      ( ( Object* ) ( *iter )->object.GetObject() )->gui.funCallback( ( *iter )->object.GetObject< Object >() );
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "Object::_GuiCallback => done" );
}//_GuiCallback
*/





/*
=============
  IsHasTag
=============
*/
bool Object::IsHasTag( const std::string& tag ) const {
  return ( this->tags && this->tags->IsHasTag( tag ) ? true : false );
}//HasTag



/*
=============
  AddTag
=============
*/
void Object::AddTag( const std::string& tag ) {
  if( !this->tags ) {
    this->tags = new Tags();
  }
  this->tags->AddTag( tag );
}//AddTag




/*
=============
  RemoveTag
=============
*/
void Object::RemoveTag( const std::string& tag ) {
  if( !this->tags ) {
    return;
  }
  this->tags->RemoveTag( tag );
}//RemoveTag


IAnimationObject* Object::MakeInstance( const std::string& setName ) {
  Object *child = this->GetChild( setName );
  if( !child ) {
    child = new Object( setName, this, false );
  }
  child->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  return child;
}



/*
=============
  SetLuaScript
=============
*/
void Object::SetLuaScript( const std::string& setScript ) {
  this->luaScript = setScript;
}//SetLuaScript



/*
=============
  EnableLightBlockByCollision
=============
*/
ObjectWidget::WidgetLightBlockByCollision* Object::EnableLightBlockByCollision() {
  if( !this->GetCollision() ) {
    __log.PrintInfo( Filelevel_WARNING, "Object::EnableLightBlockByCollision => collision is NULL" );
    return NULL;
  }
  auto w = new ObjectWidget::WidgetLightBlockByCollision( this, __lightLenderer, this->GetCollision()->GetCollisionElement() );
  this->widget->AddWidget( w );
  return w;
}//EnableLightBlockByCollision


void Object::DisableLightBlockByCollision() {
  this->widget->DeleteWidget( ObjectWidget::OBJECT_WIDGET_LIGHTBLOCKBYCOLLISION );
}


void Object::__Test() {
  CollisionRect *rect = new CollisionRect();  //!!! memleak
  CollisionElementPolygon *elem = new CollisionElementPolygon( &this->position, rect );
  CollisionElementPolygon::PointList pointList;
  pointList.push_back( CollisionElementPolygon::Point(  3.0f, -10.0f ) );
  pointList.push_back( CollisionElementPolygon::Point(  8.0f, -7.0f ) );
  pointList.push_back( CollisionElementPolygon::Point(  8.0f,  12.0f ) );
  pointList.push_back( CollisionElementPolygon::Point(  6.0f,  14.0f ) );
  pointList.push_back( CollisionElementPolygon::Point( -6.0f,  14.0f ) );
  pointList.push_back( CollisionElementPolygon::Point( -8.0f,  12.0f ) );
  pointList.push_back( CollisionElementPolygon::Point( -8.0f, -7.0f ) );
  pointList.push_back( CollisionElementPolygon::Point( -3.0f, -10.0f ) );
  elem->SetPointList( pointList );
  auto w = new ObjectWidget::WidgetLightBlockByCollision( this, __lightLenderer, elem );
  this->widget->AddWidget( w );
}
