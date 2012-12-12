#pragma once
/*
  �.�. ������ 2D, � ��������� ������ ���������, �� Renderable ��������� ���� �������, ������� � ������� ������������� � �������, ������ �������
  Renderable - ������� ����������� ����� ��� ���� ������������
*/

#include <vector>
#include "klib.h"
#include "textureatlas.h"

/*
  ������ �� ��������� �� �����, � ���� ����� + ������ � vector, ����� ��� �������� ��� ���� �� ������
  ��� ���������� ��� �������: ���� ���� ������ ���������� � GPU (������, ������ 16 float-��������� � ���� Vertex Attribute)
*/
class RenderableQuad;
typedef std::vector< RenderableQuad > CoreRenderableList; //������ ������������ (����� �������� ������������ �������)
typedef std::vector< GLushort > CoreRenderableListIndicies;   //��������� ������ ������������
#define RENDERABLE_INDEX_UNDEFINED  ( 0xFFFF )


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
};

#pragma pack( push, 4 )


//���� ��������� �����. ��� ��������
class RenderableQuad: public Renderable
{
private:
  Vec3  position; //4 floats
  float rotation;

  Vec4  color;    //4 floats

  Vec2  size;     //4 floats
  Vec2  scale;

  Vec4  texCoords;  //4 floats

public:
  RenderableQuad();
  virtual ~RenderableQuad();

  RenderableQuad* SetPosition     ( const Vec3& newPosition );
  RenderableQuad* SetSize         ( const Vec2& newSize );
  RenderableQuad* SetColor        ( const Vec4& newColor );
  RenderableQuad* SetScale        ( const Vec2& newScale );
  RenderableQuad* SetRotation     ( const float newAngle );
  RenderableQuad* SetTexture      ( const std::string& textureFileName, const Vec2& texCoordsLeftTop = Vec2( 0.0f, 0.0f ), const Vec2& texCoordsRightBottom = Vec2( 1.0f, 1.0f ) );
  const Vec3&     GetPosition     ();

  inline void* GetPointerToVertex () { return &this->position.x; }
  inline void* GetPointerToColor  () { return &this->color.x; }

  bool  Render();
};

#pragma pack( pop )
