#ifndef __ANIMATIONPACK_H__
#define __ANIMATIONPACK_H__


#include "animationset.h"


#include <hash_map>
#include <string>
#include <memory>

class ISprite;
enum AnimationSpriteParameterType;
class IAnimationParameter;

namespace Animation {


class AnimationPack;


void Update( float dt );
void Destroy();
AnimationPack* GetPackTemplate( const std::string& packName );
//template< class T >
//bool ApplyAnimation(  );

class AnimationPack
{
public:
  typedef IAnimationParameter* SetParameterFunc( AnimationSpriteParameterType, void* );

  AnimationPack( const std::string &animationPackName = "", bool isTemplate = false );
  virtual ~AnimationPack();
  AnimationSet* CreateAnimationSet( const std::string& animationName, float animationLength = 0.0f );
  void __Dump( const std::string &prefix = "" );
  bool SetCurrentAnimation( const std::string &animationName, float startTime = 0.0f );
  void Reset();
  void Update( float dt );
  void SetEnabled( bool setEnabled );
  template< class T >
  void MakeFromTemplate( const Animation::AnimationPack& templatePack, SetParameterFunc* setParameterFunc, IAnimationObject *object );

private:
  typedef std::shared_ptr< AnimationSet > AnimationSetPtr;
  typedef std::hash_map< std::string, AnimationSetPtr > AnimationSetList;

  AnimationSetList _animationSetList;
  AnimationSet *_currentAnimation;
  std::string _name;
  bool _isEnabled;
  bool _isTemplate;

  //AnimationPack();
  AnimationPack( const AnimationPack& );
  AnimationPack& operator=( const AnimationPack& );
};


template< class TAnimation >
void AnimationPack::MakeFromTemplate( const Animation::AnimationPack& templatePack, SetParameterFunc* setParameterFunc, IAnimationObject *object ) {
  for( auto &set: templatePack._animationSetList ) {
    this->CreateAnimationSet( set.first )->MakeFromTemplate< TAnimation >( *set.second, object );
  }

  this->SetEnabled( templatePack._isEnabled );
}//MakeFromTemplate


};

#endif
