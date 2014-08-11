#include "animationset.h"
#include "animation.h"
#include "file.h"


Animation::AnimationSetAction::AnimationSetAction()
:callback( NULL ), luaFunctionId( 0 ) {
}


Animation::AnimationSetAction::AnimationSetAction( const AnimationSetAction& from )
:action( from.action ), animation( from.animation ), userData( from.userData ), callback( from.callback ), luaFunctionId( from.luaFunctionId ) {
}


Animation::AnimationSetAction& Animation::AnimationSetAction::operator=( const Animation::AnimationSetAction& from ) {
  this->action = from.action;
  this->animation = from.animation;
  this->userData = from.userData;
  this->callback = from.callback;
  this->luaFunctionId = from.luaFunctionId;
  return *this;
}


Animation::AnimationSetAction::AnimationSetAction( const Animation::ANIMATION_SET_ACTION setAction, const std::string &setAnimation )
:action( setAction ), animation( setAnimation ), luaFunctionId( 0 ), callback( NULL ) {
}



Animation::AnimationSet::~AnimationSet() {
  if( !this->_animationList.empty() ) {
    for( auto &animation: this->_animationList ) {
      delete animation;
    }
    this->_animationList.clear();
  }
}


Animation::AnimationSet::AnimationSet( const std::string &setName, float setAnimationLength )
:_name( setName ), _time( 0.0f ), _animationLength( setAnimationLength ), _status( Animation::ANIMATION_SET_STATUS_STOPPED ), _actionAfterAnimationDone(  )
{
}


Animation::IAnimation* Animation::AnimationSet::AddAnimation( Animation::IAnimation* newAnimation ) {
  if( !newAnimation ) {
    __log.PrintInfo( Filelevel_ERROR, "AnimationSet::AddAnimation => is NULL" );
    return NULL;
  }

  this->_animationList.push_back( newAnimation );

  return newAnimation;
}//AddAnimation


void Animation::AnimationSet::Update( float dt ) {
  //__log.PrintInfo( Filelevel_DEBUG, "AnimationSet::Update => status[%d]", this->_status );
  if( this->_status == Animation::ANIMATION_SET_STATUS_PLAYING ) {
    if( this->_animationLength > 0.0f ) {
      this->_time += dt;
      if( this->_time > this->_animationLength ) {
        //__log.PrintInfo( Filelevel_DEBUG, "AnimationSet::Update => animation done, next action %d", this->_actionAfterAnimationDone.action );
        switch( this->_actionAfterAnimationDone.action ) {
          case Animation::ANIMATION_SET_ACTION_DIE: {
            this->SetEnabled( false );
            this->_status = Animation::ANIMATION_SET_STATUS_STOPPED;
          }
          break;
          case Animation::ANIMATION_SET_ACTION_REPEAT: {
            while( this->_time > this->_animationLength ) {
              this->_time -= this->_animationLength;
            }
          }
          break;
          case Animation::ANIMATION_SET_ACTION_SET_ANIMATION: {
            this->_status = Animation::ANIMATION_SET_STATUS_CHANGING;
            this->_time -= this->_animationLength;
          }
          break;
          case Animation::ANIMATION_SET_ACTION_STOP: {
            this->_status = Animation::ANIMATION_SET_STATUS_PAUSED;
            this->_time = 0.0f;
          }
          break;
        }
        if( this->_actionAfterAnimationDone.callback ) {
          __log.PrintInfo( Filelevel_DEBUG, "this->_actionAfterAnimationDone.callback => functionId[%d]", this->_actionAfterAnimationDone.luaFunctionId );
          this->_actionAfterAnimationDone.callback( &this->_actionAfterAnimationDone );
        }
      }
    } else {
      this->_time = 0.0f;
    }
  }//playing
  if( this->_status != Animation::ANIMATION_SET_STATUS_STOPPED ) {
    for( auto &animation: this->_animationList ) {
      animation->Update( this->_time );
    }
  }
}//Update


void Animation::AnimationSet::__Dump( const std::string &prefix ) {
  /*
  LOGD( "%s. AnimationSet[%p] name['%s'] length[%3.1f] time[%3.1f] cycled[%d] animationCount[%d]\n", prefix.c_str(), this, this->_name.c_str(), this->_animationLength, this->_time, this->_cycled, this->_animationList.size() );
  for( auto &animation: this->_animationList ) {
    animation->__Dump( prefix + "  " );
  }
  */
}//__Dump


void Animation::AnimationSet::SetEnabled( bool isEnabled ) {
  for( auto &animation: this->_animationList ) {
    animation->SetEnable( isEnabled );
  }
}//DisableAnimation


void Animation::AnimationSet::SetActionAfterAnimationComplete( const Animation::AnimationSetAction& actionAfterComplete ) {
  this->_actionAfterAnimationDone = actionAfterComplete;
}//SetActionAfterAnimationComplete
