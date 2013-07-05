#pragma once
/*
  �.�. ������ 2D, � ��������� ������ ���������, �� Renderable ��������� ���� �������, ������� � ������� ������������� � �������, ������ �������
  Renderable - ������� ����������� ����� ��� ���� ������������
*/

#include <vector>
#include "klib.h"
#include "textureatlas.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include "animationsprite.h"

/*
  ������ �� ��������� �� �����, � ���� ����� + ������ � vector, ����� ��� �������� ��� ���� �� ������
  ��� ���������� ��� �������: ���� ���� ������ ���������� � GPU (������, ������ 16 float-��������� � ���� Vertex Attribute)
*/
class RenderableQuad;
typedef std::vector< RenderableQuad > CoreRenderableList; //������ ������������ (����� �������� ������������ �������)
typedef std::vector< GLushort > CoreRenderableListIndicies;   //��������� ������ ������������
#define RENDERABLE_INDEX_UNDEFINED  ( 0xFFFF )
#define RENDERABLE_DEFAULT_TEXTURE  ( "data/textures/null.png" )


//���� ����������� (����, ������, �����, �����, ���, ����� � �.�.)
enum RenderableType
{
  RENDERABLE_TYPE_UNKNOWN = 0,
  RENDERABLE_TYPE_QUAD,

  RENDERABLE_TYPE_UNDEFINED
};


//������� ����� ��� ���� ������������
class Renderable
{
protected:
  //bool  isChanged;

public:
  Renderable();
  virtual ~Renderable();

  virtual bool Render() = NULL;
  virtual bool IsHasPoint( const Vec2& pos ) = NULL;
};

#pragma pack( push, 4 )


//���� ��������� �����. ��� ��������
class RenderableQuad: public Renderable, public ISprite
{
public:
  typedef RenderableQuad* (*CreateExternalRenderableInListProc)( float zIndex, CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort *outIndex );
  typedef bool (*DestroyExternalRenderableInListProc)( CoreRenderableList *inRenderableList, CoreRenderableListIndicies  *inRenderableIndicies, CoreRenderableListIndicies  *inRenderableFreeIndicies, GLushort index );
  struct RenderableQuadInfo
  {
    std::string textureName;
    bool textureChangedFlag;
    CreateExternalRenderableInListProc CreateExternalRenderableInListFunc;
    DestroyExternalRenderableInListProc DestroyExternalRenderableInListFunc;
    GLshort indexInRenderableList;
    bool isEnabled;
    bool isEnabledPrev;
  };

private:
  Vec3  position; //4 floats
  float rotation;

  Vec4  color;    //4 floats

  Vec2  size;     //4 floats
  Vec2  scale;

  Vec4  texCoords;  //4 floats

  RenderableQuadInfo *info; //�������������� ������ �����

public:
  RenderableQuad();
  RenderableQuad( const RenderableQuad &src );
  RenderableQuad( CreateExternalRenderableInListProc createProc, DestroyExternalRenderableInListProc destroyProc, GLshort index );
  virtual ~RenderableQuad();
  void operator=( const RenderableQuad &src );

  RenderableQuad* SetPosition     ( const Vec3& newPosition );
  RenderableQuad* SetSize         ( const Vec2& newSize );
  RenderableQuad* SetColor        ( const Vec4& newColor );
  RenderableQuad* SetScale        ( const Vec2& newScale );
  RenderableQuad* SetRotation     ( const float newAngle );
  RenderableQuad* SetTexture      ( const std::string& textureFileName, const Vec2& texCoordsLeftTop = Vec2( 0.0f, 0.0f ), const Vec2& texCoordsRightBottom = Vec2( 1.0f, 1.0f ) );
  inline
  const Vec3&     GetPosition     () { return this->position; }
  inline float    GetRotation     () { return this->rotation; }
  inline
  const Vec4&     GetColor        () { return this->color; }
  inline
  const Vec2&     GetSize         () { return this->size; }
  inline
  const Vec2&     GetScale        () { return this->scale; }
  inline
  const Vec4&     GetTexCoords    () { return this->texCoords; }
  Vec2            GetMiddleTextureCoords();
  inline
  std::string     GetTextureFileName(){ return ( this->info ? this->info->textureName : "" ); }

  inline void* GetPointerToVertex () { return &this->position.x; }
  inline void* GetPointerToColor  () { return &this->color.x; }
  virtual bool            IsHasPoint      ( const Vec2& pos );
  virtual Vec3& GetPositionPtr() {
    return this->position;
  }
  virtual Vec4& GetColorPtr() {
    return this->color;
  }
  virtual std::string& GetTextureNamePtr() {
    return this->info->textureName;
  }
  virtual IAnimationObject* MakeInstance() {
    return this->info->CreateExternalRenderableInListFunc( this->position.z, NULL, NULL, NULL, NULL );
  }
  virtual Vec4& GetTextureCoordsPtr() {
    return this->texCoords;
  }
  virtual Vec2& GetScalePtr() {
    return this->scale;
  }
  virtual float* GetRotationPtr() {
    return &this->rotation;
  }
  virtual bool& GetTextureChangedFlag() {
    return this->info->textureChangedFlag;
  }
  virtual Vec2& GetSizePtr() {
    return this->size;
  }
  virtual bool* GetEnabledPtr() {
    return &this->info->isEnabled;
  }

  bool  Render();

  void SaveToBuffer               ( MemoryWriter &writer );
  void LoadFromBuffer             ( MemoryReader &reader );

  void CalculateRect              ( Vec2 &leftTop, Vec2 &rightBottom );
};

#pragma pack( pop )
