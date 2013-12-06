#include "animation.h"


Animation::IAnimation::IAnimation( const std::string& setName )
:_name( setName )
{
}


Animation::IAnimation::~IAnimation() {
  /*
  while( this->_parameters.size() ) {
    delete this->_parameters.begin()->second;
    this->_parameters.erase( this->_parameters.begin() );
  }
  */
}


void Animation::IAnimation::Update( float animationTime ) {

  ParametersList::const_iterator
    iter = this->_parameters.begin(),
    iterEnd = this->_parameters.end();

  while( iter != iterEnd ) {
    iter->second->Update( animationTime );
    ++iter;
  }
}//Update


void Animation::IAnimation::__Dump( const std::string &prefix ) {
  for( auto &parameter: this->_parameters ) {
    parameter.second->__Dump( prefix + "  " );
  }
}//__Dump


void Animation::IAnimation::SetEnable( bool isEnabled ) {
  static_cast< AnimationParameterBool* >( &( *this->_parameters.find( ENABLED )->second ) )->SetValue( isEnabled );
}//Disable
