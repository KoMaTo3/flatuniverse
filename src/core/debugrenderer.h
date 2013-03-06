#pragma once

#include "klib.h"
#include <deque>


class DebugRendererObject;



class DebugRenderer {
public:
  typedef std::deque< DebugRendererObject* > ObjectsList;

  DebugRenderer();
  virtual ~DebugRenderer();

  void  Render();
  void  Clrscr();
  void  Line  ( const Vec3& start, const Vec3& end, const Vec4& color );
  void  Sprite( const Vec3& leftTop, const Vec3& rightBottom, const std::string& textureName, const Vec4& color );
  void  Rect  ( const Vec3& leftTop, const Vec3& rightBottom, const Vec4& color );
  void  SciccorEnable ( const Vec2& leftTop, const Vec2& rightBottom );
  void  SciccorDisable();

private:
  ObjectsList pipeline;

};
