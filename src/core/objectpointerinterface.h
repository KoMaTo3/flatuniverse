#pragma once

//#include "objectpointer.h"

class ObjectPointer;

class IObjectPointer
{
private:
public:
  IObjectPointer(){}
  virtual ~IObjectPointer(){}
  virtual void  PointerAdd    ( ObjectPointer *pointer ) = NULL;
  virtual void  PointerRemove ( ObjectPointer *pointer ) = NULL;
};
