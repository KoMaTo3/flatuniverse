#pragma once

#include "keyboard.h"
#include "klib.h"
#include "object.h"

class Mouse: public Keyboard
{
public:
  typedef void MoveListener( const Vec2 &pos );
  typedef std::vector< MoveListener* > MoveListenerList;

private:
  struct
  {
    Vec2    position;
    Vec2    size;
    Vec3    spriteOffset;
    Pointer *sprite;
    Vec2    pixelsToTexels;
    bool    isHardware;
    HCURSOR hCursor;
  } cursor;
  MoveListenerList  moveListeners;

  Size  windowSize;

public:
  Mouse();
  virtual ~Mouse();

  void  Init                    ( const Size& newWindowSize );
  void  SetCursor               ( const std::string& imageFileName, Object* object, bool setIsHardware = false );
  void  MoveCursor              ( const Vec2& newPosition );
  const Vec2& GetCursorPosition ();
  void  DestroyCursor           ();

  void  AddMoveListener         ( MoveListener *listenerProc );
  void  RemoveMoveListener      ( MoveListener *listenerProc );

private:
  HCURSOR _CreateCursorFromMemory ( Byte *buffer, const Size *size, COLORREF clrTransparent, DWORD xHotspot, DWORD yHotspot );
  void    ResetCursor           ();
  void    _GetMaskBitmaps       ( HBITMAP hSourceBitmap, COLORREF clrTransparent, HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap );
};
