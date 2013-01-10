#pragma once

#include <stdio.h>
#include "file.h"
#include "objectpointerinterface.h"


class ObjectPointer
{
private:
  bool            valid;  //флаг того, что объект, на который ссылкается указатель, валиден
  IObjectPointer  *object;
public:
  ObjectPointer ();
  ObjectPointer ( IObjectPointer *setObject );
  ObjectPointer ( const ObjectPointer& copyFrom );
  ~ObjectPointer();

  inline void     SetValid  ( bool setValid ){ this->valid = setValid; if( !setValid ) this->object = NULL; }
  inline bool     GetValid  (){ return this->valid; }
  inline
  IObjectPointer* GetObject () const { return ( this->valid ? this->object : NULL ); }
  inline
  IObjectPointer* GetObjectSrc() const { return this->object; }
  template < class T >
  T*              GetObject () const { return ( this->valid ? ( T* ) this->object : NULL ); }
  template < class T >
  T*              GetObjectSrc() const { return ( T* ) this->object; }
  IObjectPointer* operator* (){ return this->object; }
  void            Init      ( IObjectPointer *setObject );
  void            Reset     ();
};
