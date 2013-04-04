#include "objectpointer.h"



inline void IPointerValidator::SetValid( bool setIsValid ) {
  this->isValid = setIsValid;
}//SetValid

inline bool IPointerValidator::GetIsValid() const {
  return this->isValid;
}//GetIsValid





void PointerOwnerInfo::Bind( void *setSelf ) {
  this->self = setSelf;
}

PointerOwnerInfo::~PointerOwnerInfo() {
  if( this->self && this->count ) {
    for( PointerList::const_iterator iter = this->pointers.begin(), iterEnd = this->pointers.end(); iter != iterEnd; ++iter ) {
      if( *iter ) {
        ( *iter )->SetValid( false );
      }
    }
  }
}





IPointerOwner::IPointerOwner() {
  this->_pointerObjectValidator.count = 0;
}

void IPointerOwner::PointerBind( void *self ) {
  this->_pointerObjectValidator.Bind( self );
}

void IPointerOwner::PointerAdd( IPointerValidator *pointer ) {
  ++this->_pointerObjectValidator.count;
  this->_pointerObjectValidator.pointers.push_back( pointer );
}

void IPointerOwner::PointerRemove( IPointerValidator *pointer ) {
  --this->_pointerObjectValidator.count;
  if( this->_pointerObjectValidator.count < 0 ) {
    //WARNING!!!
  }

  for( PointerOwnerInfo::PointerList::const_iterator iter = this->_pointerObjectValidator.pointers.begin(), iterEnd = this->_pointerObjectValidator.pointers.end(); iter != iterEnd; ++iter ) {
    if( *iter == pointer ) {
      this->_pointerObjectValidator.pointers.erase( iter );
      break;
    }
  }
}





Pointer::Pointer( IPointerOwner *setObject ): object( setObject ) {
  if( setObject ) {
    setObject->PointerAdd( this );
    this->SetValid( true );
  } else {
    this->SetValid( false );
  }
}


Pointer::~Pointer() {
  if( this->GetIsValid() && this->object ) {
    this->object->PointerRemove( this );
    this->SetValid( false );
  }
}


inline bool Pointer::IsValid() const {
  return this->GetIsValid();
}
