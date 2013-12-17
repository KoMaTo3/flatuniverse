#pragma once

#include "klib.h"
#include "memory.h"
#include "squareatlas.h"
#include "gl/gl3.h"
#include <vector>


class TextureAtlas;
extern TextureAtlas *__textureAtlas;


class TextureAtlas
{
private:
  GLuint      textureId;      //opengl-handle
  Size        size;
  Byte        borderPerItem;  //размер бордюра для каждой текстуры

  //Чтобы туда-сюда не гонять текстуру - храним её и в RAM (скорость не тестировалась, а надо бы)
  memory      textureData;
  SquareAtlas atlas;
  bool        needWriteToGPU;

  struct TextureAtlasItem
  {
    std::string   textureFileName;  //файл текстуры
    Mat4          matTransform;     //матрица преобразования текстурных координат в координаты в атласе
    Mat4          matInvTransform;  //матрица преобразования координат в атласе в текстурные координаты
    Rect< Dword>  rect;             //координаты блока в атласе
  };
  typedef std::vector< TextureAtlasItem* > TextureAtlasItemsList;
  TextureAtlasItemsList textures;

  struct ThreadDataLoadTexture {
    memory  *data;
    Size    size;
    TextureAtlas *atlas;
    TextureAtlasItem *item;
  };

public:
  TextureAtlas();
  virtual ~TextureAtlas();

  bool                Init                  ( const Size& maxTextureSize, Byte borderSize = 0 );
  void                Bind                  ();
  void                Unbind                ();
  TextureAtlasItem*   LoadTexture           ( const std::string& textureFileName, const bool forceLoad = false );
  Vec4                GetTextureCoords      ( const std::string& textureFileName, const Vec4& textureCoords );
  Vec2                GetTextureSize        ( const std::string& textureFileName );
  Vec4                GetInvTextureCoords   ( const std::string& textureFileName, const Vec4& textureCoords );
  const std::string&  GetTextureNameByCoords( Vec2 texCoords );
  TextureAtlasItem*   IsTextureLoaded       ( const std::string& textureFileName );
  bool                CheckGLError          ( int line, const std::string& fileName );
  inline const Size   GetAtlasSize          () { return this->size; }
  inline void         FlushToGPU            () { this->needWriteToGPU = true; }

  static unsigned int __stdcall ThreadLoadTexture( void* data );

  //debug
  void              __Dump( const std::string& fileName );
};
