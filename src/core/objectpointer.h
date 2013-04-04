#pragma once

#include <deque>



/*
===============
  IPointerValidator
===============
*/
class IPointerValidator {
public:
  inline void SetValid( bool setIsValid );
  inline bool GetIsValid() const;

private:
  bool isValid;
};



/*
===============
  PointerOwnerInfo
===============
*/
class PointerOwnerInfo {
public:
  long count;
  typedef std::deque< IPointerValidator* > PointerList;
  PointerList pointers;
  void *self; //указатель на родительский объект

  ~PointerOwnerInfo();
  void Bind( void *setSelf );
};



/*
===============
  IPointerOwner
  Этим классом должен наследоваться контролируемый объект
===============
*/
class IPointerOwner {
public:
  IPointerOwner();
  void PointerBind( void *self );
  void PointerAdd( IPointerValidator *pointer );
  void PointerRemove( IPointerValidator *pointer );

private:
  PointerOwnerInfo _pointerObjectValidator;
};



/*
===============
  Pointer
  Класс-указатель на необходимый объект
===============
*/
class Pointer: public IPointerValidator {
public:
  Pointer( IPointerOwner *setObject );
  virtual ~Pointer();
  inline bool IsValid() const;

  template< class T >
  T* GetObject() const {
    printf( "GetObject => object[x%p] valid[%d]\n", this->object, this->GetIsValid() );
    return ( this->GetIsValid() ? ( T* ) this->object : NULL );
  }

private:
  IPointerOwner *object;
};
