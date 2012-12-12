#include "renderable.h"
#include "../gl/gl.h"


extern CoreRenderableList *__coreRenderableList;


Renderable::Renderable()
//:isChanged( false )
{
}//constructor



Renderable::~Renderable()
{
}//destructor




/*
=============
  SetPosition
=============
*/
RenderableQuad* RenderableQuad::SetPosition( const Vec3& newPosition )
{
  this->position = newPosition;
  //this->isChanged = true;
  return this;
}//SetPosition




/*
=============
  SetSize
=============
*/
RenderableQuad* RenderableQuad::SetSize( const Vec2& newSize )
{
  this->size = newSize;
  //this->isChanged = true;
  return this;
}//SetSize




/*
=============
  SetColor
=============
*/
RenderableQuad* RenderableQuad::SetColor( const Vec4& newColor )
{
  this->color = newColor;
  //this->isChanged = true;
  return this;
}//SetColor




/*
=============
  SetScale
=============
*/
RenderableQuad*RenderableQuad::SetScale( const Vec2& newScale )
{
  this->scale = newScale;
  //this->isChanged = true;
  return this;
}//SetScale




/*
=============
  SetRotation
=============
*/
RenderableQuad* RenderableQuad::SetRotation( const float newAngle )
{
  this->rotation = newAngle;
  //this->isChanged = true;
  return this;
}//SetRotation



//
//  RenderableQuad
//
RenderableQuad::RenderableQuad()
:position( 0.0f, 0.0f, 0.0f ), size( 1.0f, 1.0f ), color( 1.0f, 1.0f, 1.0f, 1.0f ), scale( 1.0f, 1.0f ), rotation( 0.0f ), texCoords( 0.0f, 0.0f, 0.0f, 0.0f )
{
}//constructor



RenderableQuad::~RenderableQuad()
{
}//destructor



bool RenderableQuad::Render()
{
  glColor4fv( ( const float* ) &this->color );

  Mat4 matPos, matRot, matScale, matRes;
  matPos.Identity();
  matRot.Identity();
  matScale.Identity();

  matPos[ 0 ][ 3 ] = this->position.x;
  matPos[ 1 ][ 3 ] = this->position.y;

  float sa = Math::Sin16( this->rotation );
  float ca = Math::Cos16( this->rotation );
  matRot[ 0 ][ 0 ] = ca;
  matRot[ 0 ][ 1 ] = -sa;
  matRot[ 1 ][ 0 ] = sa;
  matRot[ 1 ][ 1 ] = ca;

  matScale[ 0 ][ 0 ] = this->scale.x;
  matScale[ 1 ][ 1 ] = this->scale.y;

  matRes = matPos * matRot * matScale;

  Vec4 resPos;
  Vec2 halfSize = this->size * 0.5f;

  resPos = Vec4( -halfSize.x, -halfSize.y, this->position.z, 1.0f ) * matRes;
  glVertex2f( resPos.x, resPos.y );
  resPos = Vec4( -halfSize.x, halfSize.y, this->position.z, 1.0f ) * matRes;
  glVertex2f( resPos.x, resPos.y );
  resPos = Vec4( halfSize.x, halfSize.y, this->position.z, 1.0f ) * matRes;
  glVertex2f( resPos.x, resPos.y );
  resPos = Vec4( halfSize.x, -halfSize.y, this->position.z, 1.0f ) * matRes;
  glVertex2f( resPos.x, resPos.y );

  return true;
}



/*
=============
  SetTexture
=============
*/
RenderableQuad* RenderableQuad::SetTexture( const std::string& textureFileName, const Vec2& texCoordsLeftTop, const Vec2& texCoordsRightBottom )
{
  this->texCoords = __textureAtlas->GetTextureCoords( textureFileName, Vec4( texCoordsLeftTop.x, texCoordsLeftTop.y, texCoordsRightBottom.x, texCoordsRightBottom.y ) );
  return this;
}//SetTexture




/*
=============
  GetPosition
=============
*/
const Vec3& RenderableQuad::GetPosition()
{
  return this->position;
}//GetPosition
