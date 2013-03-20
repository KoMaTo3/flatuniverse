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
  DebugRendererSprite( const Vec3& setLeftTop, const Vec3& setRightBottom, const std::string& setTextureName, const Vec4& setColor );
  virtual ~DebugRendererSprite();

  virtual void Render();

private:
  Vec3  leftTop,
        rightBottom;
  std::string textureName;
  Vec4  color;
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




class DebugRendererScissorEnable: public DebugRendererObject {
public:
  DebugRendererScissorEnable( const Vec2& setLeftTop, const Vec2& setRightBottom );
  virtual ~DebugRendererScissorEnable() {}

  virtual void Render();

private:
  Vec2  leftTop,
        rightBottom;
};




class DebugRendererScissorDisable: public DebugRendererObject {
public:
  DebugRendererScissorDisable() {}
  virtual ~DebugRendererScissorDisable() {}

  virtual void Render();
};




class DebugRendererText: public DebugRendererObject {
public:
  DebugRendererText( const Vec3& setPos, const Vec4& setColor, const std::string &setText );
  virtual ~DebugRendererText();

  virtual void Render();

private:
  Vec3  pos;
  Vec4  color;
  std::string text;
};
