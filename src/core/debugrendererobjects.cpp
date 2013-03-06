#include "debugrendererobjects.h"
#include "gl/gl.h"
#include "gl/gl3.h"
#include "textureatlas.h"


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
:leftTop( setLeftTop ), rightBottom( setRightBottom ), textureName( setTextureName ), color( setColor )
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
