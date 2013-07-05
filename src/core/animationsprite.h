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
  //Vec2 _position;
  //Vec4 _color;
  ISprite();
  virtual ~ISprite();
  virtual Vec3& GetPositionPtr() = NULL;
  virtual Vec4& GetColorPtr() = NULL;
  virtual std::string& GetTextureNamePtr() = NULL;
  virtual Vec4& GetTextureCoordsPtr() = NULL;
  virtual bool& GetTextureChangedFlag() = NULL;
  virtual Vec2& GetScalePtr() = NULL;
  virtual float* GetRotationPtr() = NULL;
  virtual bool* GetEnabledPtr() = NULL;
  virtual Vec2& GetSizePtr() = NULL;
  virtual IAnimationObject* MakeInstance() = NULL;

  Animation::AnimationPack *ApplyAnimation( const std::string& templateName, const std::string& animationName = "default" );
  Animation::AnimationPack *ApplySubAnimation( const std::string& animationName );
  inline Animation::AnimationPack *GetAnimation() {
    return this->_animation;
  }

protected:
  Animation::AnimationPack *_animation;
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
