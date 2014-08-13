#pragma once
/*
  Т.к. движок 2D, и оперирует только спрайтами, то Renderable оперирует тупо точками, которые в шейдере преобразуются в спрайты, ничего лишнего
  Renderable - базовый виртуальный класс для всех рендерейблов
*/

#include <vector>
#include "klib.h"
#include "textureatlas.h"
#include "memorywriter.h"
#include "memoryreader.h"

/*
  Храним не указатели на квады, а сами квады + храним в vector, чтобы все элементы шли один за другим
  Это необходимо для рендера: весь этот массив заливается в GPU (точнее, только 16 float-элементов в виде Vertex Attribute)
*/
class RenderableQuad;
typedef std::vector< RenderableQuad > CoreRenderableList; //список рендерейблов (точки прямиком отправляются шейдеру)
typedef std::vector< GLushort > CoreRenderableListIndicies;   //индексный массив рендерейблов
#define RENDERABLE_INDEX_UNDEFINED  ( 0xFFFF )
#define RENDERABLE_DEFAULT_TEXTURE  ( "data/textures/null.png" )


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
  virtual bool IsHasPoint( const Vec2& pos ) = NULL;
  virtual bool IsIntersectRect( const Vec2& leftTop, const Vec2& rightBottom ) = NULL;
  virtual void SetEnabled( bool isEnabled ) = NULL;
  virtual void SetIsSaveable( bool isSaveable ) = NULL;
};

#pragma pack( push, 4 )


//Квад заданного цвета. Без текстуры
class RenderableQuad: public Renderable
{
public:
  //RenderableQuadInfo
  struct RenderableQuadInfo
  {
    std::string textureName;
    bool textureChangedFlag;
    GLshort indexInRenderableList;
    Vec4  textureCoordsPrev,
          textureCoordsNew;
    Vec2  sizePrev,
          sizeNew;
    float rotationPrev,
          rotationNew;
    Vec4  colorNew,
          colorPrev;
    bool isSaveable;
  };
  //

private:
  Vec3  position; //4 floats
  float rotation;

  Vec4  color;    //4 floats

  Vec2  size;     //4 floats
  Vec2  scale;

  Vec4  texCoords;  //4 floats

  RenderableQuadInfo *info; //дополнительные данные квада (указатель на них в ISprite)

public:
  RenderableQuad();
  RenderableQuad( RenderableQuad &src );
  virtual ~RenderableQuad();
  RenderableQuad& operator=( RenderableQuad &src );

  RenderableQuad* SetPosition     ( const Vec3& newPosition );
  RenderableQuad* SetPosition2D   ( const Vec2& newPosition );
  RenderableQuad* SetZ            ( const float newZ );
  RenderableQuad* SetSize         ( const Vec2& newSize );
  RenderableQuad* SetColor        ( const Vec4& newColor );
  RenderableQuad* SetScale        ( const Vec2& newScale );
  RenderableQuad* SetRotation     ( const float newAngle );
  RenderableQuad* SetTexture      ( const std::string& textureFileName, const Vec2& texCoordsLeftTop = Vec2( 0.0f, 0.0f ), const Vec2& texCoordsRightBottom = Vec2( 1.0f, 1.0f ) );
  RenderableQuad* SetTextureCoords( const Vec2& texCoordsLeftTop = Vec2( 0.0f, 0.0f ), const Vec2& texCoordsRightBottom = Vec2( 1.0f, 1.0f ) );
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
    std::string     GetTextureFileName(){ return ( static_cast< RenderableQuadInfo* >( this->info ) ? static_cast< RenderableQuadInfo* >( static_cast< RenderableQuadInfo* >( this->info ) )->textureName : "" ); }

  inline void* GetPointerToVertex () { return &this->position.x; }
  inline void* GetPointerToColor  () { return &this->color.x; }
  virtual bool            IsHasPoint      ( const Vec2& pos );
  virtual bool            IsIntersectRect ( const Vec2& leftTop, const Vec2& rightBottom );
  virtual void SetEnabled( bool isEnabled );
  inline GLshort  GetIndexInList() { return ( this->info ? this->info->indexInRenderableList : 0 ); }
  virtual void SetIsSaveable( bool isSaveable ) { if( this->info ) { this->info->isSaveable = isSaveable; } }
  inline bool  IsSaveable() { return this->info ? this->info->isSaveable : true; }

  bool  Render();

  void SaveToBuffer               ( MemoryWriter &writer );
  void LoadFromBuffer             ( MemoryReader &reader );

  void CalculateRect              ( Vec2 &leftTop, Vec2 &rightBottom );
  inline Vec4& GetTexCoordsModifier() {
    return this->info->textureCoordsNew;
  }
  inline Vec2& GetSizeModifier() {
    return this->info->sizeNew;
  }
  inline Vec4& GetTextureCoordsModifier() {
    return this->info->textureCoordsNew;
  }
  inline std::string& GetTextureNameModifier() {
    return this->info->textureName;
  }
  inline bool& GetTextureChangedFlag() {
    return this->info->textureChangedFlag;
  }
  inline float* GetRotationModifier() {
    return &this->info->rotationNew;
  }
  inline Vec4& GetColorModifier() {
    return this->info->colorNew;
  }
  void CheckChanges();
  inline void SetIndexInRenderableList( GLshort index ) {
    if( this->info ) {
      this->info->indexInRenderableList = index;
    }
  }
};

#pragma pack( pop )
