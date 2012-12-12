#pragma once

#include <list>
#include <deque>
#include <string>
#include "klib.h"
#include "renderable.h"
#include "collisionmgr.h"
#include "textureatlas.h"



class Object
{
public:
  typedef std::list< Object* > ObjectChilds;
  struct ObjectForce  //вектор силы
  {
    long  id;
    Vec3  vec;

    ObjectForce( long newId, const Vec3& newVec ){ this->id = newId; this->vec = newVec; }
  };
  typedef std::deque< ObjectForce > ObjectForceList;
private:
  Vec3            position;     //рассчитанная позиция объекта в сцене. звук это, скрипт, частица или меш, главная координата - эта. её может модифицировать физдвижок, напрямую
  Vec3            positionSrc;  //исходная позиция объекта. идет постоянный пересчет из positionSrc в position
  Mat4            matrixTransform;  //матрица трансформации для получения позиции объекта

  Object          *_parent; //объект-родитель
  ObjectChilds    *_childs; //дочерние объекты
  std::string     name;     //имя объекта (root, item0001, player02...)
  std::string     nameFull; //полное имя с учетом иерархии (/root/player02/weapon0)
  ObjectForceList forces;   //перечень сил, действующих на объект

  CoreRenderableList  *_renderableList; //ссылка на список рендерейблов, в котором находится объект
  CoreRenderableListIndicies  *_renderableIndicies; //ссылка на список индексов рендерейблов

  struct ObjectRenderableInfo
  {
    GLushort         num;    //индекс рендерейбла в массиве (т.к. в __coreRenderableList не ссылки, а объекты, то их адресация может меняться)
    RenderableType  type;   //тип рендерейбла

    ObjectRenderableInfo();
    ObjectRenderableInfo( const ObjectRenderableInfo& from );
    ObjectRenderableInfo( GLushort newNum, RenderableType newType );
  } renderable;

  Collision       *collision;

public:
  Object( const std::string &objectName, Object* parentObject );
  virtual ~Object();

  const std::string&  GetName             ();
  const std::string&  GetNameFull         ();
  bool                IsChild             ( const Object* obj );
  void                AttachChildObject   ( Object* newChild );
  void                UnAttachChildObject ( Object* child );
  void                UnAttachThisFromParent();
  void                ClearChilds         ();
  Object*             SetPosition         ( const Vec3& newPosition );
  Object*             SetPosition2D       ( const Vec2& newPosition );
  inline const Vec3&  GetPosition         () { return this->position; }
  Object*             GetChild            ( const std::string& name );
  void                SetForce            ( long forceId, const Vec3& vec );
  void                RemoveForce         ( long forceId );
  const Mat4&         GetMatrixTransform  ();

  Renderable*         EnableRenderable    ( RenderableType renderType, float zIndex );
  bool                DisableRenderable   ();
  Renderable*         GetRenderable       ();
  inline bool         IsRenderable        () { return this->renderable.num >= 0; }
  RenderableQuad*     EnableRenderableGUI ( float zIndex );

  Collision*          EnableCollision     ();
  void                DisableCollision    ();
  Collision*          GetCollision        ();
  inline
    bool              IsCollision         () { return this->collision != NULL; }

  void                Update              ( float dt );
};