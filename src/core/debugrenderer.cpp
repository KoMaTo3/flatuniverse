#include "debugrenderer.h"
#include "debugrendererobjects.h"
#include "gl/gl.h"



DebugRenderer::DebugRenderer() {
}//constructor



DebugRenderer::~DebugRenderer() {
}//destructor




/*
=============
  Clrscr
=============
*/
void DebugRenderer::Clrscr() {
  ObjectsList::const_iterator iter, iterEnd = this->pipeline.end();
  for( iter = this->pipeline.begin(); iter != iterEnd; ++iter ) {
    delete *iter;
  }
  this->pipeline.clear();
}//Clrscr




/*
=============
  Render
=============
*/
void DebugRenderer::Render() {
  glLineWidth( 1.0f );
  glLoadIdentity();

  ObjectsList::const_iterator iter, iterEnd = this->pipeline.end();
  for( iter = this->pipeline.begin(); iter != iterEnd; ++iter ) {
    ( *iter )->Render();
  }
}//Render



/*
=============
  Line
=============
*/
void DebugRenderer::Line( const Vec3& start, const Vec3& end, const Vec4& color ) {
  DebugRendererLine *item = new DebugRendererLine( start, end, color );
  this->pipeline.push_back( item );
}//Line



/*
=============
  Sprite
=============
*/
void DebugRenderer::Sprite( const Vec3& leftTop, const Vec3& rightBottom, const std::string& textureName, const Vec4& color ) {
  DebugRendererSprite *item = new DebugRendererSprite( leftTop, rightBottom, textureName, color );
  this->pipeline.push_back( item );
}//Sprite



/*
=============
  Rect
=============
*/
void DebugRenderer::Rect( const Vec3& leftTop, const Vec3& rightBottom, const Vec4& color ) {
  DebugRendererRect *item = new DebugRendererRect( leftTop, rightBottom, color );
  this->pipeline.push_back( item );
}//Rect



/*
=============
  SciccorEnable
=============
*/
void DebugRenderer::SciccorEnable( const Vec2& leftTop, const Vec2& rightBottom ) {
  DebugRendererScissorEnable *item = new DebugRendererScissorEnable( leftTop, rightBottom );
  this->pipeline.push_back( item );
}//SciccorEnable



/*
=============
  SciccorDisable
=============
*/
void DebugRenderer::SciccorDisable() {
  DebugRendererScissorDisable *item = new DebugRendererScissorDisable();
  this->pipeline.push_back( item );
}//SciccorDisable
