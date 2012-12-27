#pragma once


template< class T >
class ObjectPointer
{
private:
  bool  valid;  //флаг того, что объект, на который ссылкается указатель, валиден
  T     *object;
public:
  ObjectPointer ( T *setObject );

  inline void SetValid  ( bool setValid ){ this->valid = setValid; }
  inline bool GetValid  (){ return this->valid; }
  inline T*   GetObject (){ return ( this->valid ? this->object : NULL ); }
  T*          operator*() { return this->object; }
};



template< class T >
ObjectPointer< T >::ObjectPointer( T *setObject )
:object( setObject )
{
  this->valid = ( setObject != NULL );
}
