/*
*  ласс Animation
* Ѕазовый класс дл€ всех анимаций, например, анимаци€ спрайта, анимаци€ кости скелета, анимаци€ меню.
*
*/

#ifndef __ANIMATION_H__
#define __ANIMATION_H__


#include <hash_map>
#include <memory>
#include <string>


#include "animationparameter.h"


enum AnimationSpriteParameterType {
  ENABLED,
  /*
  POSITION,
  POSITION_X,
  POSITION_Y,
  COLOR,
  COLOR_R,
  COLOR_G,
  COLOR_B,
  TEXTURE_NAME,
  TEXTURE_COORDINATES,
  ROTATION,
  SCALE,
  OBJECT_SIZE,
  */
  RENDERABLE_SIZE,
  RENDERABLE_TEXTURE_NAME,
  RENDERABLE_TEXTURE_COORDINATES
};


class IAnimationObject {
public:
  virtual IAnimationObject* MakeInstance() = NULL;
};


class IAnimation
{
public:
  IAnimation();
  virtual ~IAnimation();
  virtual IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType ) = 0;
  virtual IAnimationObject* MakeObjectInstance() = NULL;
  void Update( float animationTime );
  void SetEnable( bool isEnabled );
  void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimation &animation ) = NULL;

protected:
  typedef std::shared_ptr< IAnimationParameter > ParameterPtr;
  typedef std::hash_map< AnimationSpriteParameterType, ParameterPtr > ParametersList;
  ParametersList _parameters;
};


#endif
