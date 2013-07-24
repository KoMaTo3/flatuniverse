#include "renderable.h"
#include "animationpack.h"
#include "../gl/gl.h"


extern CoreRenderableListIndicies *__coreRenderableListIndicies;


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
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SetPosition => pos[%3.1f; %3.1f]", newPosition.x, newPosition.y );
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
  this->size = this->info->sizeNew = this->info->sizePrev = newSize;
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
  this->color = this->info->colorNew = this->info->colorPrev = newColor;
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
  this->rotation = this->info->rotationNew = this->info->rotationPrev = newAngle;
  //this->isChanged = true;
  return this;
}//SetRotation



//
//  RenderableQuad
//
RenderableQuad::RenderableQuad()
:position( 0.0f, 0.0f, 0.0f ), size( 1.0f, 1.0f ), color( 1.0f, 1.0f, 1.0f, 1.0f ), scale( 1.0f, 1.0f ), rotation( 0.0f ), texCoords( 0.0f, 0.0f, 0.0f, 0.0f )
{
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad test: this[%p] info[%p]", this, this->info );
  this->info = new RenderableQuadInfo();
  this->info->textureName = "";
  this->info->indexInRenderableList = -1;
  this->info->textureChangedFlag = false;
  this->info->textureCoordsNew = this->info->textureCoordsPrev = this->texCoords;
  this->info->sizeNew = this->info->sizePrev = this->size;
  this->info->rotationNew = this->info->rotationPrev = this->rotation;
  this->info->colorNew = this->info->colorPrev = this->color;
}//constructor



RenderableQuad::RenderableQuad( RenderableQuad &src )
:position( src.position ), size( src.size ), color( src.color ), scale( src.scale ), rotation( src.rotation ), texCoords( src.texCoords )
{
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::copy-constructor => this[%p] src[%p]", this, &src );
  this->info = src.info;
  src.info = nullptr;
}//constructor



void RenderableQuad::operator=( RenderableQuad &src )
{
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::operator= => this[%p] src[%p]", this, &src );
  this->position = src.position;
  this->size = src.size;
  this->color = src.color;
  this->scale = src.scale;
  this->rotation = src.rotation;
  this->texCoords = src.texCoords;
  this->info = src.info;
  src.info = nullptr;
}//operator=





RenderableQuad::~RenderableQuad()
{
  //TODO: because RenderableQuad inherited from ISprite, info deleted at ~ISprite
  /*
  if( this->info->indexInRenderableList != RENDERABLE_INDEX_UNDEFINED ) {
    this->info->DestroyExternalRenderableInListFunc( NULL, NULL, NULL, this->info->indexInRenderableList );
  } else {
  */
    //DEF_DELETE( this->info->animation );
    //delete this->info;
  //}
  DEF_DELETE( this->info );
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
  const Vec4 v4( texCoordsLeftTop.x, texCoordsLeftTop.y, texCoordsRightBottom.x, texCoordsRightBottom.y );
  this->texCoords = __textureAtlas->GetTextureCoords( textureFileName, v4 );
  this->info->textureCoordsNew = this->info->textureCoordsPrev = v4;
  this->info->textureName = textureFileName;
  return this;
}//SetTexture



/*
=============
  SetTextureCoords
=============
*/
RenderableQuad* RenderableQuad::SetTextureCoords( const Vec2& texCoordsLeftTop, const Vec2& texCoordsRightBottom ) {
  if( this->info->textureName.size() ) {
    const Vec4 v4( texCoordsLeftTop.x, texCoordsLeftTop.y, texCoordsRightBottom.x, texCoordsRightBottom.y );
    this->texCoords = __textureAtlas->GetTextureCoords( this->info->textureName, v4 );
    this->info->textureCoordsNew = this->info->textureCoordsPrev = v4;
  }
  return this;
}//SetTextureCoords



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
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SaveToBuffer => texture['%s']", this->info->textureName.c_str() );
  writer << this->info->textureName;
  //__log.PrintInfo( Filelevel_DEBUG, ". pos: sizeof[%d]", sizeof( this->GetPosition() ) );
  writer << this->GetPosition();
  //__log.PrintInfo( Filelevel_DEBUG, ". rot" );
  writer << this->GetRotation();
  writer << this->GetColor();
  writer << this->GetSize();
  writer << this->GetScale();
  //__log.PrintInfo( Filelevel_DEBUG, ". atlas..." );
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


/*
=============
  CheckChanges
=============
*/
void RenderableQuad::CheckChanges() {
  //size
  if( this->info->sizeNew != this->info->sizePrev ) {
    this->SetSize( this->info->sizeNew );
  }

  //texture
  if( this->info->textureChangedFlag ) {
    //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::CheckChanges => texture['%s']", this->info->textureName.c_str() );
    this->SetTexture( this->info->textureName, Vec2( this->info->textureCoordsNew.x, this->info->textureCoordsNew.y ), Vec2( this->info->textureCoordsNew.z, this->info->textureCoordsNew.w ) );
    this->info->textureChangedFlag = false;
  } else if( this->info->textureCoordsNew != this->info->textureCoordsPrev ) {
    this->SetTextureCoords( Vec2( this->info->textureCoordsNew.x, this->info->textureCoordsNew.y ), Vec2( this->info->textureCoordsNew.z, this->info->textureCoordsNew.w ) );
  }

  //rotation
  if( this->info->rotationNew != this->info->rotationPrev ) {
    this->SetRotation( this->info->rotationNew );
  }

  //color
  if( this->info->colorNew != this->info->colorPrev ) {
    this->SetColor( this->info->colorNew );
  }
}//CheckChanges



/*
=============
  SetEnabled
=============
*/
void RenderableQuad::SetEnabled( bool isEnabled ) {
  //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SetEnabled => this[x%p] value[%d]", this, isEnabled );
  if( isEnabled ) {
    if( this->info->indexInRenderableList != RENDERABLE_INDEX_UNDEFINED ) {
      //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SetEnabled => added index %d", this->info->indexInRenderableList );
      __coreRenderableListIndicies->push_back( this->info->indexInRenderableList );
    }
  } else {
    if( this->info->indexInRenderableList != RENDERABLE_INDEX_UNDEFINED ) {
      auto iterEnd = __coreRenderableListIndicies->end();
      for( auto iter = __coreRenderableListIndicies->begin(); iter != iterEnd; ++iter ) {
        if( *iter == this->info->indexInRenderableList ) {
          //__log.PrintInfo( Filelevel_DEBUG, "RenderableQuad::SetEnabled => removed index %d", this->info->indexInRenderableList );
          __coreRenderableListIndicies->erase( iter );
          break;
        }
      }
    }
  }
}//SetEnabled
