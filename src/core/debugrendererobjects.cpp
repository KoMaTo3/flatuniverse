#include "debugrendererobjects.h"
#include "gl/gl.h"
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


DebugRendererSprite::DebugRendererSprite( const Vec3& setLeftTop, const Vec3& setRightBottom, const std::string& setTextureName )
:leftTop( setLeftTop ), rightBottom( setRightBottom ), textureName( setTextureName )
{
}//constructor

DebugRendererSprite::~DebugRendererSprite() {
}//destructor

void DebugRendererSprite::Render() {
  __textureAtlas->Bind();
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
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
}//Render



DebugRendererRect::DebugRendererRect( const Vec3& setLeftTop, const Vec3& setRightBottom, const Vec4& setColor )
:leftTop( setLeftTop ), rightBottom( setRightBottom ), color( setColor )
{
}//constructor

DebugRendererRect::~DebugRendererRect() {
}

void DebugRendererRect::Render() {
  glColor4f( this->color.x, this->color.y, this->color.z, this->color.w );
  glBegin( GL_LINE_LOOP );
  glVertex3f( this->leftTop.x, this->leftTop.y, this->leftTop.z );
  glVertex3f( this->leftTop.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->rightBottom.y, this->rightBottom.z );
  glVertex3f( this->rightBottom.x, this->leftTop.y, this->rightBottom.z );
  glEnd();
}//Render
