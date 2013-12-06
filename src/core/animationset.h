#ifndef __ANIMATIONSET_H__
#define __ANIMATIONSET_H__


#include <vector>
#include <memory>
#include <string>


namespace Animation {


class IAnimation;
class IAnimationObject;


enum ANIMATION_SET_ACTION {
  ANIMATION_SET_ACTION_STOP,
  ANIMATION_SET_ACTION_DIE,
  ANIMATION_SET_ACTION_REPEAR,
  ANIMATION_SET_ACTION_SET_ANIMATION,
};


struct AnimationSetAction {
  ANIMATION_SET_ACTION action;
  std::string animation;
};


class AnimationSet
{
public:
  AnimationSet( const std::string &setName, float setAnimationLength = 0.0f );
  virtual ~AnimationSet();
  IAnimation* AddAnimation( IAnimation* newAnimation );
  void Update( float dt );
  __forceinline const std::string& GetName() const {
    return this->_name;
  }
  __forceinline void SetAnimationLength( float length ) {
    this->_animationLength = ( length > 0.0f ? length : 0.0f );
  }
  __forceinline void SetAnimationCycled( bool cycled ) {
    this->_cycled = cycled;
  }
  __forceinline void ResetAnimation( float setTime = 0.0f ) {
    this->_time = setTime;
  }
  void SetEnabled( bool isEnabled );
  bool GetEnabled() const;
  void __Dump( const std::string &prefix = "" );
  template< class TAnimation >
  void MakeFromTemplate( const AnimationSet& set, IAnimationObject *object );
  inline float GetTime() const {
    return this->_time;
  }

private:
  typedef std::vector< IAnimation* > AnimationList;
  AnimationList _animationList;
  std::string _name;
  float
    _time,
    _animationLength;
  bool _cycled;
  AnimationSetAction _actionAfterAnimationDone;

  AnimationSet();
  AnimationSet( const AnimationSet& );
  AnimationSet& operator=( const AnimationSet& );
};


template< class TAnimation >
void AnimationSet::MakeFromTemplate( const AnimationSet& set, IAnimationObject *object ) {
  this->_name   = set._name;
  this->_time   = set._time;
  this->_animationLength = set._animationLength;
  this->_cycled = set._cycled;
  for( auto &animation: set._animationList ) {
    TAnimation *anim = new TAnimation( object->MakeInstance( animation->GetName() ), animation->GetName() );
    this->AddAnimation( anim )->MakeFromTemplate( *animation );
  }
}//MakeFromTemplate

};

#endif
