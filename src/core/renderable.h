#pragma once
/*
  Т.к. движок 2D, и оперирует только спрайтами, то Renderable оперирует тупо точками, которые в шейдере преобразуются в спрайты, ничего лишнего
  Renderable - базовый виртуальный класс для всех рендерейблов
*/

#include <vector>
#include "klib.h"
#include "textureatlas.h"

/*
  Храним не указатели на квады, а сами квады + храним в vector, чтобы все элементы шли один за другим
  Это необходимо для рендера: весь этот массив заливается в GPU (точнее, только 16 float-элементов в виде Vertex Attribute)
*/
class RenderableQuad;
typedef std::vector< RenderableQuad > CoreRenderableList; //список рендерейблов (точки прямиком отправляются шейдеру)
typedef std::vector< GLushort > CoreRenderableListIndicies;   //индексный массив рендерейблов
#define RENDERABLE_INDEX_UNDEFINED  ( 0xFFFF )


//Типы рендерейбла (квад, спрайт, точка, линия, меш, текст и т.д.)
enum RenderableType
{
  RENDERABLE_TYPE_UNKNOWN = 0,
  RENDERABLE_TYPE_QUAD,

  RENDERABLE_TYPE_UNDEFINED
};


//Базовый класс для всех рендерейблов
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


//Квад заданного цвета. Без текстуры
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
