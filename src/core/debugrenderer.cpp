#include "debugrenderer.h"
#include "debugrendererobjects.h"



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
void DebugRenderer::Sprite( const Vec3& leftTop, const Vec3& rightBottom, const std::string& textureName ) {
  DebugRendererSprite *item = new DebugRendererSprite( leftTop, rightBottom, textureName );
  this->pipeline.push_back( item );
}//Sprite
