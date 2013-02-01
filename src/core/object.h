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
class g2Controller;
typedef std::deque< g2Controller* > GuiList;


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

//object by gui
struct ObjectByGui
{
  ObjectPointer object;
  g2Controller  *gui;

  ObjectByGui( Object *setObject, g2Controller *setGui );
  ~ObjectByGui();

private:
  ObjectByGui();
  ObjectByGui( const ObjectByGui& copyFrom );
};
typedef std::deque< ObjectByGui* > ObjectByGuiList;
extern ObjectByGuiList *__objectByGui;



enum ObjectGuiType {
  OBJECT_GUI_UNKNOWN  = 0,
  OBJECT_GUI_BUTTON   = 1,
  OBJECT_GUI_EDIT     = 2,
  OBJECT_GUI_CHECKBOX = 3
};




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

  class GuiConstructor { //конструктор создания glui-объектов
  public:
    ObjectGuiType type;
    Pos< int >    position;
    std::string   label;
    int           width;
    void          (*funCallback)( Object* object );

    GuiConstructor() {
      this->Reset();
    }
    void Reset() {
      this->type = OBJECT_GUI_UNKNOWN;
      this->position.x = this->position.y = 0;
      this->label = "";
      this->funCallback = NULL;
      this->width = 0;
    }
  };

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
  ObjectPointers  pointers;   //перечень указателей на этот объект. объект обязан делать их невалидными перед своим уничтожением
  ObjectTrigger   *trigger;

  struct {
    g2Controller  *guiController;
    ObjectGuiType type;
    void          (*funCallback)( Object* object );
  } gui;

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
  static void         _GuiCallback( g2Controller *controller );

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

  g2Controller*       EnableGui           ( const GuiConstructor *info );
  void                DisableGui          ();
  g2Controller*       GetGui              ();
  inline
  ObjectGuiType       GetGuiType          () { return this->gui.type; }

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