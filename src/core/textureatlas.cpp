#include "textureatlas.h"
#include "file.h"
#include "filemanager.h"
#include "tools.h"
#include "imageloader.h"
#include "gl/gl.h"


TextureAtlas *__textureAtlas = NULL;



TextureAtlas::TextureAtlas()
:textureId( NULL ), size( 0, 0 ), borderPerItem( 0 )
{
}//constructor



TextureAtlas::~TextureAtlas()
{
}//destructor



/*
=============
  Init
=============
*/
bool TextureAtlas::Init( const Size& maxTextureSize, Byte borderSize )
{
  if( !maxTextureSize.width || !maxTextureSize.height )
  {
    __log.PrintInfo( Filelevel_ERROR, "TextureAtlas::Init => bad size" );
    return false;
  }

  this->borderPerItem = borderSize;
  this->size = maxTextureSize;
  this->atlas.Init( this->size );
  this->textureData.alloc( this->size.width * this->size.height * 4 );
  memset( this->textureData.getData(), 0xFF, this->textureData.getLength() );

  Dword pixelsCount = this->size.width * this->size.height;
  Byte *alpha = ( Byte* ) this->textureData.getData() + 3;
  for( Dword q = 0; q < pixelsCount; ++q, alpha += 4 )
    *alpha = 0;

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glGenTextures( 1, &this->textureId );
  glBindTexture( GL_TEXTURE_2D, this->textureId );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  return true;
}//Init



/*
=============
  Bind
=============
*/
void TextureAtlas::Bind()
{
  glEnable      ( GL_TEXTURE_2D );
  glBindTexture ( GL_TEXTURE_2D, this->textureId );
}//Bind



/*
=============
  Unbind
=============
*/
void TextureAtlas::Unbind()
{
  glDisable     ( GL_TEXTURE_2D );
  glBindTexture ( GL_TEXTURE_2D, NULL );
}//Unbind



/*
=============
  GetTextureCoords
  Загрузка текстуры, размещение в атласе, выдача реальных текстурных координат
=============
*/
Vec4 TextureAtlas::GetTextureCoords( const std::string& textureFileName, const Vec4& textureCoords )
{
  TextureAtlasItem *item = this->IsTextureLoaded( textureFileName );
  if( item )
  {
    Vec3 leftTop, rightBottom;
    leftTop = Vec3( textureCoords.x, textureCoords.y, 0.0f ) * item->matTransform;
    rightBottom = Vec3( textureCoords.z, textureCoords.w, 0.0f ) * item->matTransform;
    return Vec4( leftTop.x, leftTop.y, rightBottom.x, rightBottom.y );
  }
  else
  {
    ImageLoader image;
    if( !image.LoadFromFile( textureFileName ) )
    {
      __log.PrintInfo( Filelevel_ERROR, "TextureAtlas::GetTextureCoords => file not found '%s'", textureFileName.c_str() );
      return Vec4();
    }

    Size size = *image.GetImageSize();
    if( !this->atlas.HasPlace( size ) )
    {
      __log.PrintInfo( Filelevel_ERROR, "TextureAtlas::GetTextureCoords => atlas can't find needed place [ %d; %d ]", size.width, size.height );
      return Vec4();
    }

    __log.PrintInfo( Filelevel_DEBUG, "TextureAtlas::GetTextureCoords => load new texture from file '%s'", textureFileName.c_str() );

    item = new TextureAtlasItem();
    Rect< Dword > fullRect;
    this->atlas.Cut( size + Size( this->borderPerItem * 2, this->borderPerItem * 2 ), &fullRect );
    item->rect = Rect< Dword >( fullRect.left + 1, fullRect.top + 1, fullRect.right - 1, fullRect.bottom - 1 );
    item->textureFileName = textureFileName;

    Vec2 scale( float( item->rect.right - item->rect.left + 1 ) / float( this->size.width ), float( item->rect.bottom - item->rect.top + 1 ) / float( this->size.height ) );
    Vec2 translate( float( item->rect.left ) / float( this->size.width ), float( item->rect.top ) / float( this->size.height ) );

    item->matTransform.Identity();
    Mat4 matTranslate, matScale;
    matTranslate.Identity();
    matScale.Identity();

    matScale[ 0 ][ 0 ] = scale.x;
    matScale[ 1 ][ 1 ] = scale.y;

    matTranslate[ 0 ][ 3 ] = translate.x;
    matTranslate[ 1 ][ 3 ] = translate.y;

    __log.PrintInfo( Filelevel_DEBUG, "matrix translate: [ %3.3f; %3.3f ]", translate.x, translate.y );
    __log.PrintInfo( Filelevel_DEBUG, "matrix scale: [ %3.3f; %3.3f ]", scale.x, scale.y );

    item->matTransform = matTranslate * matScale;

    //размещение картинки в текстуре
    this->Bind();
    Dword *dst = ( Dword* ) this->textureData.getData(),
      *src = ( Dword* ) image.GetImageData();
    for( Dword y = 0; y < image.GetImageSize()->height; ++y )
      memcpy( dst + ( item->rect.top + y ) * this->size.width + item->rect.left, src + y * image.GetImageSize()->width, image.GetImageSize()->width * 4 );
    glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
    this->Unbind();

    this->textures.push_back( item );
    Vec3 tex0( textureCoords.x, textureCoords.y, 0 ), tex1( textureCoords.z, textureCoords.w, 0 );
    tex0 *= item->matTransform;
    tex1 *= item->matTransform;
    return Vec4( tex0.x, tex0.y, tex1.x, tex1.y );
  }
}//GetTextureCoords



/*
=============
  IsTextureLoaded
=============
*/
TextureAtlas::TextureAtlasItem* TextureAtlas::IsTextureLoaded( const std::string& textureFileName )
{
  TextureAtlasItemsList::iterator iter, iterEnd = this->textures.end();
  for( iter = this->textures.begin(); iter != iterEnd; ++iter )
    if( ( *iter )->textureFileName == textureFileName )
      return *iter;
  return NULL;
}//IsTextureLoaded





/*
=============
  __Dump
  Дамп текстуры в файл
=============
*/
void TextureAtlas::__Dump( const std::string& fileName )
{
  File f;
  if( !f.Open( fileName, File_mode_WRITE ) )
  {
    glBindTexture( GL_TEXTURE_2D, this->textureId );
    memory data( this->size.width * this->size.height * 4 );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.getData() );
    glBindTexture( GL_TEXTURE_2D, NULL );

    f.Write( "\x00\x00\x02\x00\x00\x00\x00\x00", 8 );  //TrueColor without compression
    f.Write( "\x00\x00\x00\x00", 4 );
    Word w = this->size.width;
    Word h = this->size.height;
    f.Write( &w, sizeof( w ) );
    f.Write( &h, sizeof( h ) );
    f.Write( "\x20\x00", 2 ); //bpp

    Byte *buff;
    Dword color;
    for( Dword y = 0; y < h; ++y )
    for( Dword x = 0; x < w; ++x )
    {
      buff = ( Byte* ) ( ( ( Dword* ) data.getData() ) + ( h - y - 1 ) * w + x );
      color = RGBA( buff[ 2 ], buff[ 1 ], buff[ 0 ], buff[ 3 ] );
      f.Write( &color, sizeof( color ) );
    }

    f.Close();
  }
}//__DumpToFile
