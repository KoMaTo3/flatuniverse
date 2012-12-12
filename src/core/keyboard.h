#pragma once

#include "ktypes.h"
#include "memory.h"

class Keyboard
{
private:
  memory  keys,
          keysOld,
          keysPressed,
          keysReleased;

private:
  void  _ReallocBuffers( Dword newKeysCount );

public:
  Keyboard();
  virtual ~Keyboard();

  void  Update();

  void  DoPress   ( Dword keyId );
  void  DoRelease ( Dword keyId );
  bool  IsHold    ( Dword keyId );
  bool  IsFree    ( Dword keyId );
  bool  IsPressed ( Dword keyId );
  bool  IsReleased( Dword keyId );
};