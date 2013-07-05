#ifndef __ANIMATIONTEMPLATE_H__
#define __ANIMATIONTEMPLATE_H__


/*
* Шаблон Animation. Не привязан к реальным объектам.
*/

#include "animation.h"


class AnimationTemplate: public IAnimation
{
public:
  AnimationTemplate();
  virtual ~AnimationTemplate();
  template <class T>
  IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType );
  virtual IAnimationObject* MakeObjectInstance();
  virtual void MakeFromTemplate( const IAnimation &animation ) {
  }

private:
  AnimationTemplate( const AnimationTemplate& );
  AnimationTemplate& operator=( const AnimationTemplate& );
  virtual IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType ) { return nullptr; }
};


/*
* SetParameter
* Делает один из параметров анимированным, возвращает его аниматор
*
*/
template <class T>
IAnimationParameter* AnimationTemplate::SetParameter( AnimationSpriteParameterType parameterType ) {
  IAnimationParameter *parameter = NULL;
 
  ParametersList::iterator iter = this->_parameters.find( parameterType );
  if( iter != this->_parameters.end() ) {
    return &( *iter->second );
  }
  parameter = new T();

  this->_parameters.insert( std::make_pair( parameterType, ParameterPtr( parameter ) ) );

  return parameter;
}//SetParameter


#endif
