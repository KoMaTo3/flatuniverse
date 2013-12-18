#include "textureatlas.h"
#include "file.h"
#include "filemanager.h"
#include "tools.h"
#include "imageloader.h"
#include "gl/gl.h"
#include "thread.h"


Texture::Atlas *__textureAtlas = NULL;



Texture::Atlas::Atlas()
:textureId( NULL ), size( 0, 0 ), borderPerItem( 0 ), needWriteToGPU( false )
{
}//constructor



Texture::Atlas::~Atlas()
{
  if( this->textureId ) {
    glDeleteTextures( 1, &this->textureId );
  }
}//destructor



/*
=============
  Init
=============
*/
bool Texture::Atlas::Init( const Size& maxTextureSize, Byte borderSize )
{
  if( !maxTextureSize.width || !maxTextureSize.height )
  {
    __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::Init => bad size" );
    return false;
  }

  this->borderPerItem = borderSize;
  this->size = maxTextureSize;
  this->atlas.Init( this->size );
  this->textureData.Alloc( this->size.width * this->size.height * 4 );
  memset( this->textureData.getData(), 0xFF, this->textureData.getLength() );

  Dword pixelsCount = this->size.width * this->size.height;
  Byte *alpha = ( Byte* ) this->textureData.getData() + 3;
  for( Dword q = 0; q < pixelsCount; ++q, alpha += 4 )
    *alpha = 0;

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  GL_CHECK_ERROR;
  glGenTextures( 1, &this->textureId );
  GL_CHECK_ERROR;
  glBindTexture( GL_TEXTURE_2D, this->textureId );
  GL_CHECK_ERROR;

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  GL_CHECK_ERROR;

  glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
  GL_CHECK_ERROR;
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  return true;
}//Init




/*
=============
  CheckGLError
=============
*/
bool Texture::Atlas::CheckGLError( int line, const std::string& fileName )
{
  GLenum error = glGetError();
  if( error )
  {
    __log.PrintInfo( Filelevel_ERROR, "OpenGL error at %s:%d: x%X [%d]", fileName.c_str(), line, error, error );
    return true;
  }
  return false;
}//CheckGLError



/*
=============
  Bind
=============
*/
void Texture::Atlas::Bind()
{
  glBindTexture( GL_TEXTURE_2D, this->textureId );
  if( this->needWriteToGPU ) {
    this->needWriteToGPU = false;
    glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
  }
}//Bind



/*
=============
  Unbind
=============
*/
void Texture::Atlas::Unbind()
{
  glBindTexture ( GL_TEXTURE_2D, NULL );
}//Unbind



/*
=============
  GetTextureCoords
  Загрузка текстуры, размещение в атласе, выдача реальных текстурных координат
=============
*/
Vec4 Texture::Atlas::GetTextureCoords( const std::string& textureFileName, const Vec4& textureCoords )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::GetTextureCoords => textureFileName['%s'] textureCoords[%3.3f; %3.3f]-[%3.3f; %3.3f]", textureFileName.c_str(), textureCoords.x, textureCoords.y, textureCoords.z, textureCoords.w );
  Texture::Atlas::Item *item = this->IsTextureLoaded( textureFileName );
  if( item )
  {
    Vec3 leftTop, rightBottom;
    leftTop = Vec3( textureCoords.x, textureCoords.y, 0.0f ) * item->matTransform;
    rightBottom = Vec3( textureCoords.z, textureCoords.w, 0.0f ) * item->matTransform;
    //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::GetTextureCoords => existing texture['%s'] t0[%3.5f; %3.5f] t1[%3.5f; %3.5f]", textureFileName.c_str(), leftTop.x, leftTop.y, rightBottom.x, rightBottom.y );
    return Vec4( leftTop.x, leftTop.y, rightBottom.x, rightBottom.y );
  }
  else
  {
    Texture::Atlas::Item *item = this->LoadTexture( textureFileName );
    if( !item ) {
      __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::GetTextureCoords => LoadTexture('%s') => failed", textureFileName.c_str() );
      return Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
    }
    Vec3 tex0( textureCoords.x, textureCoords.y, 0 ), tex1( textureCoords.z, textureCoords.w, 0 );
    tex0 *= item->matTransform;
    tex1 *= item->matTransform;
    //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::GetTextureCoords => loaded texture['%s'] t0[%3.5f; %3.5f] t1[%3.5f; %3.5f]", textureFileName.c_str(), tex0.x, tex0.y, tex1.x, tex1.y );
    return Vec4( tex0.x, tex0.y, tex1.x, tex1.y );
  }
}//GetTextureCoords




/*
=============
  GetInvTextureCoords
=============
*/
Vec4 Texture::Atlas::GetInvTextureCoords( const std::string& textureFileName, const Vec4& textureCoords )
{
  Texture::Atlas::Item *item = this->IsTextureLoaded( textureFileName );
  if( item )
  {
    Vec3 leftTop, rightBottom;
    leftTop = Vec3( textureCoords.x, textureCoords.y, 0.0f ) * item->matInvTransform;
    rightBottom = Vec3( textureCoords.z, textureCoords.w, 0.0f ) * item->matInvTransform;
    return Vec4( leftTop.x, leftTop.y, rightBottom.x, rightBottom.y );
  }
  else
  {
    __log.PrintInfo( Filelevel_WARNING, "Texture::Atlas::GetInvTextureCoords => texture '%s' not loaded", textureFileName.c_str() );
    return Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
  }
}//GetInvTextureCoords




/*
=============
  GetTextureNameByCoords
=============
*/
const std::string& Texture::Atlas::GetTextureNameByCoords( Vec2 texCoords )
{
  //Pos< Dword > = 
  Texture::Atlas::ItemsList::iterator iter, iterEnd = this->textures.end();
  Vec4 atlasPos( texCoords.x * float( this->size.width ), texCoords.y * float( this->size.height ), 0.0f, 0.0f );
  for( iter = this->textures.begin(); iter != iterEnd; ++iter )
  {
    Texture::Atlas::Item *item = *iter;
    if(  atlasPos.x >= item->rect.right
      || atlasPos.x < item->rect.left
      || atlasPos.y >= item->rect.bottom
      || atlasPos.y < item->rect.top
      )
      continue;
    return item->textureFileName;
  }
  static const std::string badResult = "<NULL>";
  return badResult;
}//GetTextureNameByCoords




/*
=============
  IsTextureLoaded
=============
*/
Texture::Atlas::Item* Texture::Atlas::IsTextureLoaded( const std::string& textureFileName )
{
  Texture::Atlas::ItemsList::iterator iter, iterEnd = this->textures.end();
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
void Texture::Atlas::__Dump( const std::string& fileName )
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



/*
=============
  GetTextureSize
=============
*/
Vec2 Texture::Atlas::GetTextureSize( const std::string& textureFileName )
{
  Texture::Atlas::Item *texture = this->IsTextureLoaded( textureFileName );
  if( !texture )
  {
    __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::GetTextureSize => texture '%s' not found", textureFileName.c_str() );
    return Vec2( 0.0f, 0.0f );
  }

  //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::GetTextureSize => texture['%s'] size[%d; %d]", textureFileName.c_str(), texture->rect.right - texture->rect.left + 1, texture->rect.bottom - texture->rect.top + 1 );
  return Vec2( texture->rect.right - texture->rect.left + 1.0f, texture->rect.bottom - texture->rect.top + 1.0f );
}//GetTextureSize



/*
=============
  LoadTexture
=============
*/
Texture::Atlas::Item* Texture::Atlas::LoadTexture( const std::string& textureFileName, const bool forceLoad )
{
  ImageLoader image;
  Size size( 0, 0 );
  memory *deferredLoadingData;

  if( forceLoad ) {
    if( !image.LoadFromFile( textureFileName ) ) {
      __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::LoadTexture => file not found '%s'", textureFileName.c_str() );
      return NULL;
    }
    size = image.GetImageSize();
  } else {
    deferredLoadingData = new memory();
    __fileManager->GetFile( textureFileName, *deferredLoadingData );
    size = ImageLoader::GetImageSizeFromBuffer( ( Byte* ) deferredLoadingData->getData(), deferredLoadingData->getLength() );
  }

  if( !this->atlas.HasPlace( size ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::LoadTexture => atlas can't find needed place [ %d; %d ]", size.width, size.height );
    return NULL;
  }

  Texture::Atlas::Item *item = new Texture::Atlas::Item();
  Rect< Dword > fullRect;
  this->atlas.Cut( size + Size( this->borderPerItem * 2, this->borderPerItem * 2 ), &fullRect );
  item->rect = Rect< Dword >( fullRect.left + 1, fullRect.top + 1, fullRect.right - 1, fullRect.bottom - 1 );
  item->textureFileName = textureFileName;

  Vec2 scale( float( item->rect.right - item->rect.left + 1 ) / float( this->size.width ), float( item->rect.bottom - item->rect.top + 1 ) / float( this->size.height ) );
  Vec2 translate( float( item->rect.left ) / float( this->size.width ), float( item->rect.top ) / float( this->size.height ) );

  item->matTransform.Identity();
  Mat4 matTranslate, matScale, matInvTranslate, matInvScale;
  matTranslate.Identity();
  matInvTranslate.Identity();
  matScale.Identity();
  matInvScale.Identity();

  matScale[ 0 ][ 0 ] = scale.x;
  matScale[ 1 ][ 1 ] = scale.y;
  matInvScale[ 0 ][ 0 ] = 1.0f / scale.x;
  matInvScale[ 1 ][ 1 ] = 1.0f / scale.y;

  matTranslate[ 0 ][ 3 ] = translate.x;
  matTranslate[ 1 ][ 3 ] = translate.y;
  matInvTranslate[ 0 ][ 3 ] = -translate.x;
  matInvTranslate[ 1 ][ 3 ] = -translate.y;

  //__log.PrintInfo( Filelevel_DEBUG, "matrix translate: [ %3.3f; %3.3f ]", translate.x, translate.y );
  //__log.PrintInfo( Filelevel_DEBUG, "matrix scale: [ %3.3f; %3.3f ]", scale.x, scale.y );

  item->matTransform = matTranslate * matScale;
  item->matInvTransform = matInvScale * matInvTranslate;

  //размещение картинки в текстуре
  if( forceLoad ) {
    //this->Bind();
    Dword
      *dst = ( Dword* ) this->textureData.getData(),
      *src = ( Dword* ) image.GetImageData();
    for( Dword y = 0; y < image.GetImageSize().height; ++y ) {
      memcpy( dst + ( item->rect.top + y ) * this->size.width + item->rect.left, src + y * image.GetImageSize().width, image.GetImageSize().width * 4 );
    }
    //glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
    //this->Unbind();
    this->FlushToGPU();
  } else {
    // TODO: здесь отложенная загрузка
    // класть в стек deferredLoadingData, указатель на атлас и Texture::Atlas::Item item
    ThreadDataLoadTexture *loader = new ThreadDataLoadTexture();
    loader->data = deferredLoadingData;
    loader->size = size;
    loader->atlas = this;
    loader->item = item;
    extern Thread::Pipeline *__workPipeline;
    __workPipeline->Add( loader, Texture::Atlas::ThreadLoadTexture );
  }

  this->textures.push_back( item );
  return item;
}//LoadTexture


unsigned int __stdcall Texture::Atlas::ThreadLoadTexture( void* data ) {
  Thread::Work *work = ( Thread::Work* ) data;
  ThreadDataLoadTexture *loader = ( ThreadDataLoadTexture* ) work->data;
  //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::ThreadLoadTexture => begin: data[%d] size[%d; %d]", loader->data->getLength(), loader->size.width, loader->size.height );
  //

  ImageLoader image;
  if( !image.LoadFromBuffer( ( Byte* ) loader->data->getData(), loader->data->getLength() ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Texture::Atlas::ThreadLoadTexture => image.LoadFromBuffer failed" );
    work->status = Thread::THREAD_WORK_STATUS_ERROR;
    return 1;
  }
  Dword
    *dst = ( Dword* ) loader->atlas->textureData.getData(),
    *src = ( Dword* ) image.GetImageData();
  for( Dword y = 0; y < image.GetImageSize().height; ++y ) {
    memcpy( dst + ( loader->item->rect.top + y ) * loader->atlas->size.width + loader->item->rect.left, src + y * image.GetImageSize().width, image.GetImageSize().width * 4 );
  }
  loader->atlas->FlushToGPU();

  //done
  delete loader->data;
  delete loader;
  work->status = Thread::THREAD_WORK_STATUS_DONE;
  //__log.PrintInfo( Filelevel_DEBUG, "Texture::Atlas::ThreadLoadTexture => done" );
  return 0;
}//ThreadLoadTexture
