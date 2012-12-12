#pragma once

#include "keyboard.h"
#include "klib.h"
#include "object.h"

class Mouse: public Keyboard
{
private:
  struct
  {
    Vec2    position;
    Vec2    size;
    Vec3    spriteOffset;
    Object  *sprite;
    Vec2    pixelsToTexels;
  } cursor;

  Size  windowSize;

public:
  Mouse();
  virtual ~Mouse();

  void  Init                    ( const Size& newWindowSize );
  void  SetCursor               ( const std::string& imageFileName, Object* object );
  void  MoveCursor              ( const Vec2& newPosition );
  const Vec2& GetCursorPosition ();
};
