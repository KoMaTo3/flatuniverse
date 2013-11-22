#include "debugrendererobjects.h"
#include "gl/gl.h"
#include "gl/gl3.h"
#include "textureatlas.h"
#include "filemanager.h"


DebugRendererObject::DebugRendererObject() {
}//constructor


DebugRendererObject::~DebugRendererObject() {
}//destructor




DebugRendererLine::DebugRendererLine( const Vec3& setStart, const Vec3& setEnd, const Vec4& setColor )
:start( setStart ), end( setEnd ), color( setColor )
{
}//DebugRendererLine

DebugRendererLine::~DebugRendererLine() {
}//destructor

void DebugRendererLine::Render() {
  __textureAtlas->Unbind();
  glBegin( GL_LINES );
  glColor4f( this->color.x, this->color.y, this->color.z, this->color.w );
  glVertex3f( this->start.x, this->start.y, this->start.z );
  glVertex3f( this->end.x, this->end.y, this->end.z );
  glEnd();
}//Render


DebugRendererSprite::DebugRendererSprite( const Vec3& setLeftTop, const Vec3& setRightBottom, const std::string& setTextureName, const Vec4& setColor )
:DebugRendererObject(), leftTop( setLeftTop ), rightBottom( setRightBottom ), textureName( setTextureName ), color( setColor )
{
}//constructor

DebugRendererSprite::~DebugRendererSprite() {
}//destructor

void DebugRendererSprite::Render() {
  __textureAtlas->Bind();
  glColor4f( this->color.x, this->color.y, this->color.z, this->color.w );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  //glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  Vec4 texCoords = __textureAtlas->GetTextureCoords( this->textureName, Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
  glBegin( GL_QUADS );

  glTexCoord2f( texCoords.x, texCoords.y );
  glVertex3f( this->leftTop.x, this->leftTop.y, this->leftTop.z );

  glTexCoord2f( texCoords.x, texCoords.w );
  glVertex3f( this->leftTop.x, this->rightBottom.y, this->rightBottom.z );

  glTexCoord2f( texCoords.z, texCoords.w );
  glVertex3f( this->rightBottom.x, this->rightBottom.y, this->rightBottom.z );

  glTexCoord2f( texCoords.z, texCoords.y );
  glVertex3f( this->rightBottom.x, this->leftTop.y, this->rightBottom.z );

  glEnd();
  //glDisable( GL_BLEND );
}//Render



DebugRendererRect::DebugRendererRect( const Vec3& setLeftTop, const Vec3& setRightBottom, const Vec4& setColor )
:leftTop( setLeftTop ), rightBottom( setRightBottom ), color( setColor )
{
}//constructor

DebugRendererRect::~DebugRendererRect() {
}

void DebugRendererRect::Render() {
  __textureAtlas->Unbind();
  glColor4f( this->color.x, this->color.y, this->color.z, this->color.w );
  glBegin( GL_LINE_LOOP );
  glVertex3f( this->leftTop.x, this->leftTop.y, this->leftTop.z );
  glVertex3f( this->leftTop.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->leftTop.y, this->rightBottom.z );
  glEnd();
}//Render



DebugRendererScissorEnable::DebugRendererScissorEnable( const Vec2& setLeftTop, const Vec2& setRightBottom )
:leftTop( setLeftTop ), rightBottom( setRightBottom )
{
}//constructor

void DebugRendererScissorEnable::Render() {
  glEnable( GL_SCISSOR_TEST );
  glScissor( ( int ) this->leftTop.x, ( int ) this->leftTop.y, ( int ) this->rightBottom.x - ( int ) this->leftTop.x + 1, ( int ) this->rightBottom.y - ( int ) this->leftTop.y + 1 );
}//Render



void DebugRendererScissorDisable::Render() {
  glDisable( GL_SCISSOR_TEST );
}//Render



DebugRendererText::DebugRendererText( const Vec3& setPos, const Vec4& setColor, const std::string &setText )
:pos( setPos ), color( setColor ), text( setText )
{
}//constructor

DebugRendererText::~DebugRendererText() {
}

void DebugRendererText::Render() {
  __textureAtlas->Bind();
  glColor4f( this->color.x, this->color.y, this->color.z, this->color.w );

  static bool isFontInitialized = false;
  static bool isProportional = false;
  static Vec2 textureSize;
  static Vec4 fontTexCoords;
  struct FontProportions {
    float offset,
          width;
  };
  static FontProportions *proportions = NULL;
  if( !isFontInitialized ) {
    isFontInitialized = true;
    std::string defaultFont = "data/temp/font_default.tga";
    if( !__textureAtlas->IsTextureLoaded( defaultFont ) ) {
      __textureAtlas->LoadTexture( defaultFont );
    }
    textureSize = __textureAtlas->GetTextureSize( defaultFont );
    fontTexCoords = __textureAtlas->GetTextureCoords( defaultFont, Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );

    //test
    std::string defaultFontWidths = "data/temp/font_default.flw";
    memory data;
    if( __fileManager->GetFile( defaultFontWidths, data ) )
    {
      isProportional = true;
      proportions = new FontProportions[ 256 ];
      Byte *dataByte = ( Byte* ) data.getData();
      for( Dword num = 0; num < 256; ++num )
      {
        proportions[ num ].offset = dataByte[ num << 1 ];
        proportions[ num ].width  = dataByte[ ( num << 1 ) + 1 ];
      }//for num
    }
    else {
      isProportional = false;
    }
  }

  //Vec4 texCoords = __textureAtlas->GetTextureCoords( "data/temp/font_default.tga", Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
  int length = this->text.size();
  Vec2 scale( Vec2One );
  Vec2 t0, t1;
  float dtx = ( fontTexCoords.z - fontTexCoords.x ) / 16.0f, dty = ( fontTexCoords.w - fontTexCoords.y ) / 16.0f;
  if( isProportional ) {
    dtx /= 16.0f;
    dty /= 16.0f;
  }
  float dx = 16.0f, dy = 16.0f;
  Vec2 currentPos( Vec2( this->pos.x, this->pos.y ) );

  glBegin( GL_QUADS );
  for( int q = 0; q < length; ++q ) {
    Byte letterId = this->text[ q ];
    Pos< Byte > letterPos( letterId % 16, letterId >> 4 );  //позиция буквы в алфавите
    if( isProportional ) {
      dx = float( proportions[ letterId ].width );
      t0.x = fontTexCoords.x + float( letterPos.x * 16.0f + float( proportions[ letterId ].offset ) ) * dtx;
      t0.y = fontTexCoords.y + float( letterPos.y * 16.0f ) * dty;
      t1 = t0 + Vec2( dtx * dx, dty * 16.0f );
    } else {
      t0.x = fontTexCoords.x + float( letterPos.x ) * dtx;
      t0.y = fontTexCoords.y + float( letterPos.y ) * dty;
      t1 = t0 + Vec2( dtx, dty );
    }

    glTexCoord2f( t0.x, t0.y );
    glVertex3f( currentPos.x, this->pos.y, this->pos.z );

    glTexCoord2f( t1.x, t0.y );
    glVertex3f( currentPos.x + dx, this->pos.y, this->pos.z );

    glTexCoord2f( t1.x, t1.y );
    glVertex3f( currentPos.x + dx, this->pos.y + dy, this->pos.z );

    glTexCoord2f( t0.x, t1.y );
    glVertex3f( currentPos.x, this->pos.y + dy, this->pos.z );

    currentPos.x += dx;
  }//for q

  /*
  glVertex3f( this->leftTop.x, this->leftTop.y, this->leftTop.z );
  glVertex3f( this->leftTop.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->leftTop.y, this->rightBottom.z );
  */
  glEnd();
}//Render
