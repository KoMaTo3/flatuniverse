#pragma once

#include <list>
#include <deque>
#include <string>
#include "klib.h"
#include "renderable.h"
#include "collisionmgr.h"
#include "textureatlas.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include "objectpointer.h"
#include "objectpointerinterface.h"
#include "objecttriggermgr.h"



class Object;

//object by collision
struct ObjectByCollision
{
  ObjectPointer object;
  Collision     *collision;

  ObjectByCollision( Object *setObject, Collision *setCollision );
  ~ObjectByCollision();

private:
  ObjectByCollision();
  ObjectByCollision( const ObjectByCollision& copyFrom );
};
typedef std::deque< ObjectByCollision* > ObjectByCollisionList;
extern ObjectByCollisionList *__objectByCollision;

//object by object trigger
struct ObjectByTrigger
{
  ObjectPointer object;
  ObjectTrigger *trigger;

  ObjectByTrigger ( Object *setObject, ObjectTrigger *setTrigger );
  ~ObjectByTrigger();

private:
  ObjectByTrigger ();
  ObjectByTrigger ( const ObjectByTrigger& copyFrom );
};
typedef std::deque< ObjectByTrigger* > ObjectByTriggerList;
extern ObjectByTriggerList *__objectByTrigger;





class Object: public IObjectPointer
{
public:
  typedef std::list< Object* > ObjectChilds;
  typedef std::list< ObjectPointer* > ObjectPointers;
  struct ObjectForce  //вектор силы
  {
    long  id;
    Vec3  vec;

    ObjectForce( long newId, const Vec3& newVec ){ this->id = newId; this->vec = newVec; }
  };
  typedef std::deque< ObjectForce > ObjectForceList;
protected:
  Vec3            position;     //рассчитанная позиция объекта в сцене. звук это, скрипт, частица или меш, главная координата - эта. её может модифицировать физдвижок, напрямую
private:
  Vec3            positionSrc;  //исходная позиция объекта. идет постоянный пересчет из positionSrc в position
  Mat4            matrixTransform;  //матрица трансформации для получения позиции объекта

  Object          *_parent; //объект-родитель
  ObjectChilds    *_childs; //дочерние объекты
  std::string     name;     //имя объекта (root, item0001, player02...)
  std::string     nameFull; //полное имя с учетом иерархии (/root/player02/weapon0)
  ObjectForceList forces;   //перечень сил, действующих на объект

  Collision       *collision;
  ObjectPointers  pointers;   //перечень указателей на этот объект. объект обязан делать их невалидными беред своим уничтожением
  ObjectTrigger   *trigger;

  bool            _isLockedToDelete;  //запрет на удаление объекта посредством Core::ClearScene

protected:
  CoreRenderableList  *_renderableList; //ссылка на список рендерейблов, в котором находится объект
  CoreRenderableListIndicies  *_renderableIndicies; //ссылка на список индексов рендерейблов
  CoreRenderableListIndicies  *_renderableFreeIndicies; //ссылка на список свободных индексов рендерейблов

  class ObjectRenderableInfo
  {
  public:
    GLushort         num;    //индекс рендерейбла в массиве (т.к. в __coreRenderableList не ссылки, а объекты, то их адресация может меняться)
    RenderableType  type;   //тип рендерейбла

  public:
    ObjectRenderableInfo();
    ObjectRenderableInfo( const ObjectRenderableInfo& from );
    ObjectRenderableInfo( GLushort newNum, RenderableType newType );
  } renderable;

protected:
  void                _RecalculatePosition();

public:
  Object();
  Object( const std::string &objectName, Object* parentObject );
  virtual ~Object();

  const std::string&  GetName             ();
  const std::string&  GetNameFull         ();
  const std::string&  GetParentNameFull   ();
  bool                IsChild             ( const Object* obj );
  void                AttachChildObject   ( Object* newChild );
  void                UnAttachChildObject ( Object* child );
  void                UnAttachThisFromParent();
  void                ClearChilds         ( bool forceDelete = true );
  Object*             SetPosition         ( const Vec3& newPosition );
  Object*             SetPosition2D       ( const Vec2& newPosition );
  inline const Vec3&  GetPosition         () const { return this->position; }
  Object*             GetChild            ( const std::string& name );
  void                SetForce            ( long forceId, const Vec3& vec );
  void                RemoveForce         ( long forceId );
  const Mat4&         GetMatrixTransform  ();
  Object*             GetObjectInPoint    ( const Vec2& pos );

  Renderable*         EnableRenderable    ( RenderableType renderType );
  bool                DisableRenderable   ();
  Renderable*         GetRenderable       ();
  inline bool         IsRenderable        () { return this->renderable.type != RENDERABLE_TYPE_UNKNOWN; }
  RenderableQuad*     EnableRenderableGUI ();

  Collision*          EnableCollision     ();
  void                DisableCollision    ();
  Collision*          GetCollision        ();
  inline
    bool              IsCollision         () { return this->collision != NULL; }

  ObjectTrigger*      EnableTrigger       ();
  void                DisableTrigger      ();
  ObjectTrigger*      GetTrigger          ();
  inline
    bool              IsTrigger           () { return this->trigger != NULL; }

  void                Update              ( float dt );

  void                SaveToBuffer        ( MemoryWriter &writer );
  void                LoadFromBuffer      ( MemoryReader &reader, Object *rootObject );

  Object*             GetObject           ( const std::string& name, Object *parent = NULL );

  void                PointerAdd          ( ObjectPointer *pointer );
  void                PointerRemove       ( ObjectPointer *pointer );
  inline
    Object*           SetLockToDelete     ( bool lock ) { this->_isLockedToDelete = lock; return this; }
  inline
    bool              IsLockedToDelete    () { return this->_isLockedToDelete; }
};