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
  struct ObjectForce  //������ ����
  {
    long  id;
    Vec3  vec;

    ObjectForce( long newId, const Vec3& newVec ){ this->id = newId; this->vec = newVec; }
  };
  typedef std::deque< ObjectForce > ObjectForceList;
private:
  Vec3            position;     //������������ ������� ������� � �����. ���� ���, ������, ������� ��� ���, ������� ���������� - ���. � ����� �������������� ���������, ��������
  Vec3            positionSrc;  //�������� ������� �������. ���� ���������� �������� �� positionSrc � position
  Mat4            matrixTransform;  //������� ������������� ��� ��������� ������� �������

  Object          *_parent; //������-��������
  ObjectChilds    *_childs; //�������� �������
  std::string     name;     //��� ������� (root, item0001, player02...)
  std::string     nameFull; //������ ��� � ������ �������� (/root/player02/weapon0)
  ObjectForceList forces;   //�������� ���, ����������� �� ������

  CoreRenderableList  *_renderableList; //������ �� ������ ������������, � ������� ��������� ������
  CoreRenderableListIndicies  *_renderableIndicies; //������ �� ������ �������� ������������

  struct ObjectRenderableInfo
  {
    GLushort         num;    //������ ����������� � ������� (�.�. � __coreRenderableList �� ������, � �������, �� �� ��������� ����� ��������)
    RenderableType  type;   //��� �����������

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