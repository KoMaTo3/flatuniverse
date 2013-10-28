#include "animationset.h"
#include "animation.h"
#include "file.h"


AnimationSet::AnimationSet( const std::string &setName, float setAnimationLength )
:_name( setName ), _time( 0.0f ), _animationLength( setAnimationLength ), _cycled( true )
{
  __log.PrintInfo( Filelevel_DEBUG, "AnimationSet +1 x%p", this );
}


AnimationSet::~AnimationSet() {
  __log.PrintInfo( Filelevel_DEBUG, "AnimationSet -1 x%p", this );
  if( !this->_animationList.empty() ) {
    for( auto &animation: this->_animationList ) {
      delete animation;
    }
    this->_animationList.clear();
  }
}


IAnimation* AnimationSet::AddAnimation( IAnimation* newAnimation ) {
  if( !newAnimation ) {
    __log.PrintInfo( Filelevel_ERROR, "AnimationSet::AddAnimation => is NULL" );
    return NULL;
  }

  this->_animationList.push_back( newAnimation );

  return newAnimation;
}//AddAnimation


void AnimationSet::Update( float dt ) {
  if( this->_animationLength > 0.0f ) {
    this->_time += dt;
    if( this->_cycled ) {
      while( this->_time > this->_animationLength ) {
        this->_time -= this->_animationLength;
      }
    }
  } else {
    this->_time = 0.0f;
  }

  for( auto &animation: this->_animationList ) {
    animation->Update( this->_time );
  }
}//Update


void AnimationSet::__Dump( const std::string &prefix ) {
  /*
  LOGD( "%s. AnimationSet[%p] name['%s'] length[%3.1f] time[%3.1f] cycled[%d] animationCount[%d]\n", prefix.c_str(), this, this->_name.c_str(), this->_animationLength, this->_time, this->_cycled, this->_animationList.size() );
  for( auto &animation: this->_animationList ) {
    animation->__Dump( prefix + "  " );
  }
  */
}//__Dump


void AnimationSet::SetEnabled( bool isEnabled ) {
  for( auto &animation: this->_animationList ) {
    animation->SetEnable( isEnabled );
  }
}//DisableAnimation
