#ifndef __ANIMATIONSPRITE_H__
#define __ANIMATIONSPRITE_H__

/*
* Единичная анимация спрайта. Должен быть набор таких анимаций на каждое действие.
*/

#include "animationtemplate.h"

namespace Animation {
  class AnimationPack;
};


//Интерфейс для работы со спрайтом
class ISprite: public IAnimationObject {
public:
  ISprite();
  virtual ~ISprite();
  virtual bool* GetEnabledPtr() = NULL;
  //virtual Vec3& GetPositionPtr() = NULL;
  //virtual IAnimationObject* MakeInstance() = NULL;

  Animation::AnimationPack *ApplyAnimation( const std::string& templateName, const std::string& animationName = "default" );
  Animation::AnimationPack *ApplySubAnimation( const std::string& animationName );
  inline Animation::AnimationPack *GetAnimation() {
    return this->_animation;
  }

protected:
  Animation::AnimationPack *_animation;

private:
  ISprite( const ISprite& );
  ISprite& operator=( const ISprite& );
};


class AnimationSprite: public AnimationTemplate
{
public:
  AnimationSprite( IAnimationObject *sprite );
  virtual ~AnimationSprite();
  virtual IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType );
  static IAnimationParameter* SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationSpriteThis );
  virtual IAnimationObject* MakeObjectInstance();
  virtual void MakeFromTemplate( const IAnimation &animation );

private:
  ISprite *_sprite;

  AnimationSprite();
  AnimationSprite( const AnimationSprite& );
  AnimationSprite& operator=( const AnimationSprite& );
};


#endif
