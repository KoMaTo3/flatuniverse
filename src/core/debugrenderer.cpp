#include "debugrenderer.h"
#include "debugrendererobjects.h"
#include "gl/gl.h"
#include "interngl.h"



DebugRenderer::DebugRenderer() {
}//constructor



DebugRenderer::~DebugRenderer() {
  this->Clrscr();
  for( auto &font: this->fonts ) {
    delete font.second;
  }
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
  if( !this->pipeline.empty() ) {
    glLineWidth( 1.0f );
    glActiveTexture( GL_TEXTURE0 + 0 );
  }

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



/*
=============
  Text
=============
*/
void DebugRenderer::Text( const Vec3& pos, const Vec4& color, const std::string &text ) {
  DebugRendererText *item = new DebugRendererText( pos, color, text );
  this->pipeline.push_back( item );
}//Text




/*
=============
  GetTextWidh
=============
*/
float DebugRenderer::GetTextWidh( const std::string &text, const std::string &fontProportionsFileName ) {
  FontWidthsList::iterator iter = this->fonts.find( fontProportionsFileName );
  Font *font;
  if( iter == this->fonts.end() ) {
    font = new Font();
    font->SetFont( DEBUG_RENDERER_DEFAULT_FONT, fontProportionsFileName );
    this->fonts.insert( make_pair( fontProportionsFileName, font ) );
  } else {
    font = iter->second;
  }
  return font->GetTextWidth( text );
}//GetTextWidh
