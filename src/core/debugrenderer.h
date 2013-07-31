#pragma once

#include "klib.h"
#include "font.h"
#include <unordered_map>


#define DEBUG_RENDERER_DEFAULT_FONT ( "data/temp/font_default.tga" )
#define DEBUG_RENDERER_DEFAULT_FONT_PROPORTIONS ( "data/temp/font_default.flw" )


class DebugRendererObject;



class DebugRenderer {
public:
  typedef std::vector< DebugRendererObject* > ObjectsList;

  DebugRenderer();
  virtual ~DebugRenderer();

  void  Render();
  void  Clrscr();
  void  Line  ( const Vec3& start, const Vec3& end, const Vec4& color );
  void  Sprite( const Vec3& leftTop, const Vec3& rightBottom, const std::string& textureName, const Vec4& color );
  void  Rect  ( const Vec3& leftTop, const Vec3& rightBottom, const Vec4& color );
  void  SciccorEnable ( const Vec2& leftTop, const Vec2& rightBottom );
  void  SciccorDisable();
  void  Text  ( const Vec3& pos, const Vec4& color, const std::string &text );
  float GetTextWidh( const std::string &text, const std::string &fontProportionsFileName );

private:
  ObjectsList pipeline;
  typedef std::unordered_map< std::string, Font* > FontWidthsList;
  FontWidthsList fonts;

};
