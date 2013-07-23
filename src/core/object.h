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
#include "objecttriggermgr.h"
#include "tags.h"
#include "animationobject.h"
//#include "glui2/g2Theme.h"
//#include "glui2/g2Spinner.h"



class Object;
//class g2Controller;
//typedef std::deque< g2Controller* > GuiList;
typedef std::deque< std::string > stdDequeString;


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




class Object: public IPointerOwner, public ITags, public IObject
{
public:
  typedef std::list< Object* > ObjectChilds;
  struct ObjectForce  //������ ����
  {
    long  id;
    Vec3  vec;

    ObjectForce( long newId, const Vec3& newVec ){ this->id = newId; this->vec = newVec; }
  };
  typedef std::deque< ObjectForce > ObjectForceList;

  /*
  class GuiConstructor { //����������� �������� glui-��������
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

protected:
  Vec3            position;     //������������ ������� ������� � �����. ���� ���, ������, ������� ��� ���, ������� ���������� - ���. � ����� �������������� ���������, ��������
private:
  bool            isEnabled,    //���/����. ��� ���������� ��������� ��� �����������/��������/��������. ��� ��������� ������ �� �����������������
                  isEnabledPrev;//���������� �������� isEnabled
  Vec3            positionSrc;  //�������� ������� �������. ���� ���������� �������� �� positionSrc � position
  Mat4            matrixTransform;  //������� ������������� ��� ��������� ������� �������

  Object          *_parent; //������-��������
  ObjectChilds    *_childs; //�������� �������
  std::string     name;     //��� ������� (root, item0001, player02...)
  std::string     nameFull; //������ ��� � ������ �������� (/root/player02/weapon0)
  ObjectForceList forces;   //�������� ���, ����������� �� ������

  Collision       *collision;
  ObjectTrigger   *trigger;
  Tags            *tags;    //������ �����

  /*
  struct {
    g2Controller  *guiController;
    ObjectGuiType type;
    void          (*funCallback)( Object* object );
  } gui;
  */

  bool            _isLockedToDelete;  //������ �� �������� ������� ����������� Core::ClearScene

protected:
  CoreRenderableList  *_renderableList; //������ �� ������ ������������, � ������� ��������� ������
  CoreRenderableListIndicies  *_renderableIndicies; //������ �� ������ �������� ������������
  CoreRenderableListIndicies  *_renderableFreeIndicies; //������ �� ������ ��������� �������� ������������

  class ObjectRenderableInfo
  {
  public:
    GLushort         num;    //������ ����������� � ������� (�.�. � __coreRenderableList �� ������, � �������, �� �� ��������� ����� ��������)
    RenderableType  type;   //��� �����������

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
  Object*             GetObjectByRenderableIndex( CoreRenderableList *renderableList, GLushort index );

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
  virtual IAnimationObject* MakeInstance( const std::string& setName );

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

  inline
    Object*           SetLockToDelete     ( bool lock ) { this->_isLockedToDelete = lock; return this; }
  inline
    bool              IsLockedToDelete    () { return this->_isLockedToDelete; }

  bool                IsHasTag            ( const std::string& tag ) const;
  void                AddTag              ( const std::string& tag );
  void                RemoveTag           ( const std::string& tag );
};