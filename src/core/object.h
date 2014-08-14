#pragma once

#include <list>
#include <deque>
#include <string>
#include "klib.h"
#include "entity.h"
#include "renderable.h"
#include "collisionmgr.h"
#include "textureatlas.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include "objectpointer.h"
#include "objecttriggermgr.h"
#include "tags.h"
#include "animationobject.h"
#include "objectwidgetmgr.h"
#include "objectwidgetowner.h"
#include "widgets/lightblockbycollision.h"
#include "widgets/lightpoint.h"
//#include "glui2/g2Theme.h"
//#include "glui2/g2Spinner.h"



class Object;
class LightRenderer;
//class g2Controller;
//typedef std::deque< g2Controller* > GuiList;
typedef std::deque< std::string > stdDequeString;
typedef std::deque< Object* > ObjectsToRemoveList;
extern ObjectsToRemoveList __objectsToRemove;


//object by collision
struct ObjectByCollision
{
  Pointer   object;
  Collision *collision;

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
  Pointer       object;
  ObjectTrigger *trigger;

  ObjectByTrigger ( Object *setObject, ObjectTrigger *setTrigger );
  ~ObjectByTrigger();

private:
  ObjectByTrigger ();
  ObjectByTrigger ( const ObjectByTrigger& copyFrom );
};
typedef std::deque< ObjectByTrigger* > ObjectByTriggerList;
extern ObjectByTriggerList *__objectByTrigger;

typedef std::list< Object* > ObjectList;

//object by gui
/*
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
  OBJECT_GUI_UNKNOWN    = 0,
  OBJECT_GUI_BUTTON     = 1,
  OBJECT_GUI_TEXTFIELD  = 2,
  OBJECT_GUI_CHECKBOX   = 3,
  OBJECT_GUI_DROPDOWN   = 4,
  OBJECT_GUI_LABEL      = 5,
  OBJECT_GUI_PANEL      = 6,
  OBJECT_GUI_SPINNER    = 7,

  //OBJECT_GUI_DIALOG   = 5
};
*/




class Object: public Entity, public IPointerOwner, public ITags, public Animation::IObject, public ObjectWidget::WidgetOwner
{
public:
  struct ObjectForce  //вектор силы
  {
    long  id;
    Vec3  vec;

    ObjectForce( long newId, const Vec3& newVec ){ this->id = newId; this->vec = newVec; }
  };
  typedef std::deque< ObjectForce > ObjectForceList;
  typedef void (ObjectEvent)( Object* );

  static ObjectEvent *OnLoad;
  static ObjectEvent *OnUnload;
  static ObjectEvent *OnDestroy;
  /*
  class GuiConstructor { //конструктор создания glui-объектов
  public:
    ObjectGuiType type;
    Pos< int >    position;
    std::string   label;
    int           width;
    int           height;
    void          (*funCallback)( Object* object );
    stdDequeString stringList;
    //g2DialogType  dialogType;
    Vec4          color;
    std::string   panel;
    g2Anchor      panelAnchor;
    g2SpinnerType spinnerType;
    struct {
      float min, max;
    } bounds;

    GuiConstructor() {
      this->Reset();
    }
    void Reset() {
      this->type = OBJECT_GUI_UNKNOWN;
      this->position.x = this->position.y = 0;
      this->label = "";
      this->funCallback = NULL;
      this->width = 0;
      this->height = 0;
      this->stringList.clear();
      this->color.Set( 0.0f, 0.0f, 0.0f, 1.0f );
      this->panel = "";
      this->panelAnchor = g2Anchor_Center;
      this->spinnerType = g2SpinnerType_Float;
      this->bounds.min = -10000.0f;
      this->bounds.max = 10000.0f;
      //this->dialogType = g2DialogType_Notification;
    }
  };
  */

private:
  bool            isEnabled,    //вкл/выкл. при выключении удаляются все рендерейблы/коллизии/триггеры. при включении ничего не восстанавливается
                  isEnabledPrev;//предыдущее значение isEnabled
  bool            isSaveable;
  Vec3            positionSrc;  //исходная позиция объекта. идет постоянный пересчет из positionSrc в position
  Mat4            matrixTransform;  //матрица трансформации для получения позиции объекта

  Object          *_parent; //объект-родитель
  ObjectList      *_childs; //дочерние объекты
  std::string     name;     //имя объекта (root, item0001, player02...)
  std::string     nameFull; //полное имя с учетом иерархии (/root/player02/weapon0)
  ObjectForceList forces;   //перечень сил, действующих на объект

  Collision       *collision;
  ObjectTrigger   *trigger;
  Tags            *tags;    //список тегов
  std::string     luaScriptFileName;  //скрипты
  int             luaObjectId; //id объекта в реестре lua

  /*
  struct {
    g2Controller  *guiController;
    ObjectGuiType type;
    void          (*funCallback)( Object* object );
  } gui;
  */

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
  //static void         _GuiCallback( g2Controller *controller );

public:
  Object();
  Object( const std::string &objectName, Object* parentObject, bool setIsSaveable = true, bool setLuaUserdata = false );
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
  Object*             SetZ                ( const float z );
  inline const Vec3&  GetPosition         () const { return this->position; }
  Object*             GetChild            ( const std::string& name );
  void                GetChilds           ( ObjectList &result );
  void                SetForce            ( long forceId, const Vec3& vec );
  void                RemoveForce         ( long forceId );
  const Mat4&         GetMatrixTransform  ();
  Object*             GetObjectInPoint    ( const Vec2& pos );
  void                GetObjectsInRect    ( int type, const Vec2 &leftTop, const Vec2 &rightBottom, ObjectList& result );
  bool                TestInRect          ( int type, const Vec2 &leftTop, const Vec2 &rightBottom, bool recursive );

  Renderable*         EnableRenderable    ( RenderableType renderType );
  bool                DisableRenderable   ();
  Renderable*         GetRenderable       ();
  inline bool         IsRenderable        () { return this->renderable.type != RENDERABLE_TYPE_UNKNOWN; }
  RenderableQuad*     EnableRenderableGUI ();
  Object*             GetObjectByRenderableIndex( CoreRenderableList *renderableList, GLushort index );

  Collision*          EnableCollision     ();
  void                DisableCollision    ();
  Collision*          GetCollision        ();
  inline
    bool              IsCollision         () { return this->collision != NULL; }

  ObjectTrigger*      EnableTrigger       ();
  void                DisableTrigger      ();
  ObjectTrigger*      GetTrigger          ();
  inline bool         IsTrigger           () { return this->trigger != NULL; }
  ObjectWidget::WidgetLightBlockByCollision*  EnableLightBlockByCollision();
  inline bool         IsLightBlockByCollision () { return this->widget->WidgetExists( ObjectWidget::OBJECT_WIDGET_LIGHTBLOCKBYCOLLISION ) != NULL; }
  void                DisableLightBlockByCollision();
  ObjectWidget::WidgetLightPoint*         EnableLightPoint();
  ObjectWidget::WidgetLightPoint*         GetLightPoint();
  inline bool         IsLightPoint        () { return this->widget->WidgetExists( ObjectWidget::OBJECT_WIDGET_LIGHTPOINT ) != NULL; }
  void                DisableLightPoint   ();
  inline bool         IsSaveable          () { return this->isSaveable; }
  inline Object*      GetParent           () { return this->_parent; }
  void                SetLuaScript        ( const std::string& setScript );
  inline
  const std::string&  GetLuaScript        () { return this->luaScriptFileName; }
  void                RecalculateRenderableZ( const float z );
  void                SetNeedToUnload     ( const bool unload );
  inline
  int                 GetLuaObjectId      () { return this->luaObjectId; }
  inline
  void                SetLuaObjectId      ( const int setId ) { this->luaObjectId = setId; }

  static
    RenderableQuad*   CreateExternalRenderableInList( float zIndex, CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort *outIndex );
  static
    bool              DestroyExternalRenderableInList( CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort index );
  virtual bool* GetEnabledPtr() {
    return &this->isEnabled;
  }
  virtual Vec2& GetRenderableSize() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetSizeModifier() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetSizeModifier() );
  }
  virtual Vec4& GetTextureCoords() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetTextureCoordsModifier() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetTextureCoordsModifier() );
  }
  virtual std::string& GetTextureNamePtr() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetTextureNameModifier() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetTextureNameModifier() );
  }
  virtual bool& GetTextureChangedFlag() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetTextureChangedFlag() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetTextureChangedFlag() );
  }
  virtual Vec3& GetPositionPtr() {
    return this->positionSrc;
  }
  virtual float* GetRenderableRotationPtr() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetRotationModifier() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetRotationModifier() );
  }
  virtual Vec4& GetColorPtr() {
    return ( this->IsRenderable() ? static_cast< RenderableQuad* >( this->GetRenderable() )->GetColorModifier() : static_cast< RenderableQuad* >( this->EnableRenderable( RENDERABLE_TYPE_QUAD ) )->GetColorModifier() );
  }
  virtual Vec3& GetCollisionSquareSize() {
    return ( this->IsCollision() ? this->GetCollision()->GetSquareSizeModifier() : this->EnableCollision()->InitSquare( Vec3( 1.0f, 1.0f, 0.0f ) )->GetSquareSizeModifier() );
  }
  virtual Vec3& GetCollisionOffsetPtr() {
    return ( this->IsCollision() ? this->GetCollision()->GetOffsetModifier() : this->EnableCollision()->InitSquare( Vec3( 1.0f, 1.0f, 0.0f ) )->GetOffsetModifier() );
  }
  virtual Animation::IAnimationObject* MakeInstance( const std::string& setName );

  /*
  g2Controller*       EnableGui           ( const GuiConstructor *info );
  void                DisableGui          ();
  g2Controller*       GetGui              ();
  inline
  ObjectGuiType       GetGuiType          () { return this->gui.type; }
  */

  void                Update              ( float dt );

  void                SaveToBuffer        ( MemoryWriter &writer );
  void                LoadFromBuffer      ( MemoryReader &reader, Object *rootObject, const Dword version );

  Object*             GetObject           ( const std::string& name, Object *parent = NULL );

  inline Object*      SetLockToDelete     ( bool lock ) { this->_isLockedToDelete = lock; return this; }
  inline Object*      SetSaveable         ( bool saveable ) { this->isSaveable = saveable; return this; }
  inline bool         IsLockedToDelete    () { return this->_isLockedToDelete; }

  bool                IsHasTag            ( const std::string& tag ) const;
  void                AddTag              ( const std::string& tag );
  void                RemoveTag           ( const std::string& tag );
  void                InitLuaUserData     ();

  void __Test();
};