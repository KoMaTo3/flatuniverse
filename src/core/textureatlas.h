#pragma once

#include "klib.h"
#include "memory.h"
#include "squareatlas.h"
#include "gl/gl3.h"
#include <deque>


class TextureAtlas;
extern TextureAtlas *__textureAtlas;


class TextureAtlas
{
private:
  GLuint      textureId;      //opengl-handle
  Size        size;
  Byte        borderPerItem;  //размер бордюра дл€ каждой текстуры

  //„тобы туда-сюда не гон€ть текстуру - храним еЄ и в RAM (скорость не тестировалась, а надо бы)
  memory      textureData;
  SquareAtlas atlas;

  struct TextureAtlasItem
  {
    std::string   textureFileName;  //файл текстуры
    Mat4          matTransform;     //матрица преобразовани€ текстурных координат в координаты в атласе
    Rect< Dword>  rect;             //координаты блока в атласе
  };
  typedef std::deque< TextureAtlasItem* > TextureAtlasItemsList;
  TextureAtlasItemsList textures;

public:
  TextureAtlas();
  virtual ~TextureAtlas();

  bool              Init  ( const Size& maxTextureSize, Byte borderSize = 0 );
  void              Bind  ();
  void              Unbind();
  Vec4              GetTextureCoords( const std::string& textureFileName, const Vec4& textureCoords );
  TextureAtlasItem* IsTextureLoaded ( const std::string& textureFileName );

  //debug
  void              __Dump( const std::string& fileName );
};
