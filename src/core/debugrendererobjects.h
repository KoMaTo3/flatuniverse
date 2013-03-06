#pragma once

#include "tools.h"


class DebugRendererObject {
public:
  DebugRendererObject();
  virtual ~DebugRendererObject();

  virtual void Render() = NULL;

private:

};




class DebugRendererLine: public DebugRendererObject {
public:
  DebugRendererLine( const Vec3& setStart, const Vec3& setEnd, const Vec4& setColor );
  virtual ~DebugRendererLine();

  virtual void Render();

private:
  Vec3  start,
        end;
  Vec4  color;
};




class DebugRendererSprite: public DebugRendererObject {
public:
  DebugRendererSprite( const Vec3& setLeftTop, const Vec3& setRightBottom, const std::string& setTextureName );
  virtual ~DebugRendererSprite();

  virtual void Render();

private:
  Vec3  leftTop,
        rightBottom;
  std::string textureName;
};




class DebugRendererRect: public DebugRendererObject {
public:
  DebugRendererRect( const Vec3& setLeftTop, const Vec3& setRightBottom, const Vec4& setColor );
  virtual ~DebugRendererRect();

  virtual void Render();

private:
  Vec3  leftTop,
        rightBottom;
  Vec4  color;
};
