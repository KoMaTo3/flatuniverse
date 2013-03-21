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
  Byte        borderPerItem;  //������ ������� ��� ������ ��������

  //����� ����-���� �� ������ �������� - ������ � � � RAM (�������� �� �������������, � ���� ��)
  memory      textureData;
  SquareAtlas atlas;

  struct TextureAtlasItem
  {
    std::string   textureFileName;  //���� ��������
    Mat4          matTransform;     //������� �������������� ���������� ��������� � ���������� � ������
    Mat4          matInvTransform;  //������� �������������� ��������� � ������ � ���������� ����������
    Rect< Dword>  rect;             //���������� ����� � ������
  };
  typedef std::deque< TextureAtlasItem* > TextureAtlasItemsList;
  TextureAtlasItemsList textures;

public:
  TextureAtlas();
  virtual ~TextureAtlas();

  bool                Init                  ( const Size& maxTextureSize, Byte borderSize = 0 );
  void                Bind                  ();
  void                Unbind                ();
  TextureAtlasItem*   LoadTexture           ( const std::string& textureFileName );
  Vec4                GetTextureCoords      ( const std::string& textureFileName, const Vec4& textureCoords );
  Vec2                GetTextureSize        ( const std::string& textureFileName );
  Vec4                GetInvTextureCoords   ( const std::string& textureFileName, const Vec4& textureCoords );
  const std::string&  GetTextureNameByCoords( Vec2 texCoords );
  TextureAtlasItem*   IsTextureLoaded       ( const std::string& textureFileName );
  bool                CheckGLError          ( int line, const std::string& fileName );

  //debug
  void              __Dump( const std::string& fileName );
};
