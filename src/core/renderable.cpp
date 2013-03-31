#include "renderable.h"
#include "../gl/gl.h"


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
  __log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SetPosition => pos[%3.1f; %3.1f]", newPosition.x, newPosition.y );
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
  this->info = new RenderableQuadInfo();
  this->info->textureName = "";
}//constructor



RenderableQuad::RenderableQuad( const RenderableQuad &src )
:position( src.position ), size( src.size ), color( src.color ), scale( src.scale ), rotation( src.rotation ), texCoords( src.texCoords )
{
  this->info = new RenderableQuadInfo();
  this->info->textureName = src.info->textureName;
}//constructor


void RenderableQuad::operator=( const RenderableQuad &src )
{
  this->position = src.position;
  this->size = src.size;
  this->color = src.color;
  this->scale = src.scale;
  this->rotation = src.rotation;
  this->texCoords = src.texCoords;

  this->info->textureName = src.info->textureName;
}//operator=





RenderableQuad::~RenderableQuad()
{
  delete this->info;
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
  this->info->textureName = textureFileName;
  return this;
}//SetTexture



/*
=============
  GetMiddleTextureCoords
=============
*/
Vec2 RenderableQuad::GetMiddleTextureCoords()
{
  return Vec2( ( this->texCoords.x + this->texCoords.z ) * 0.5f, ( this->texCoords.y + this->texCoords.w ) * 0.5f );
}//GetMiddleTextureCoords




/*
=============
  SaveToBuffer
=============
*/
void RenderableQuad::SaveToBuffer( MemoryWriter &writer )
{
  __log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SaveToBuffer => texture['%s']", this->info->textureName.c_str() );
  writer << this->info->textureName;
  __log.PrintInfo( Filelevel_DEBUG, ". pos: sizeof[%d]", sizeof( this->GetPosition() ) );
  writer << this->GetPosition();
  __log.PrintInfo( Filelevel_DEBUG, ". rot" );
  writer << this->GetRotation();
  writer << this->GetColor();
  writer << this->GetSize();
  writer << this->GetScale();
  __log.PrintInfo( Filelevel_DEBUG, ". atlas..." );
  writer << __textureAtlas->GetInvTextureCoords( this->info->textureName, this->GetTexCoords() );
}//SaveToBuffer




/*
=============
  LoadFromBuffer
=============
*/
void RenderableQuad::LoadFromBuffer( MemoryReader &reader )
{
  std::string textureName;
  Vec4 tex;

  reader >> textureName;
  reader >> this->position;
  reader >> this->rotation;
  reader >> this->color;
  reader >> this->size;
  reader >> this->scale;
  reader >> tex;

  if( !textureName.length() ) {
    textureName = RENDERABLE_DEFAULT_TEXTURE;
  }
  this->SetTexture( textureName, Vec2( tex.x, tex.y ), Vec2( tex.z, tex.w ) );
}//LoadFromBuffer



/*
=============
  IsHasPoint
=============
*/
bool RenderableQuad::IsHasPoint( const Vec2& pos )
{
  Vec4 resRect;
  Vec2 halfSize;
  halfSize = this->size * 0.5f;
  halfSize.x *= this->scale.x;
  halfSize.y *= this->scale.y;
  resRect.x = this->position.x - halfSize.x;
  resRect.y = this->position.y - halfSize.y;
  resRect.z = this->position.x + halfSize.x;
  resRect.w = this->position.y + halfSize.y;

  return !(
    resRect.z < pos.x ||
    resRect.x > pos.x ||
    resRect.y > pos.y ||
    resRect.w < pos.y
    );
}//IsHasPoint


/*
=============
  CalculateRect
=============
*/
void RenderableQuad::CalculateRect( Vec2 &leftTop, Vec2 &rightBottom )
{
  Vec2 resSize = Vec2( this->size.x * this->scale.x, this->size.y * this->scale.y ) * 0.5f;
  leftTop.Set( this->position.x - resSize.x, this->position.y - resSize.y );
  rightBottom.Set( this->position.x + resSize.x, this->position.y + resSize.y );
}//CalculateRect
