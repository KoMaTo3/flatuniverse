#ifndef __ANIMATIONOBJECT_H__
#define __ANIMATIONOBJECT_H__

/*
* Единичная анимация Object
*/

#include "animationtemplate.h"

namespace Animation {
  class AnimationPack;
};


//Интерфейс для работы с объектом
class IObject: public IAnimationObject {
public:
  IObject();
  virtual ~IObject();
  virtual bool* GetEnabledPtr() = NULL;
  virtual Vec2& GetRenderableSize() = NULL;
  virtual Vec4& GetTextureCoords() = NULL;
  virtual std::string& GetTextureNamePtr() = NULL;
  virtual bool& GetTextureChangedFlag() = NULL;
  //virtual Vec3& GetPositionPtr() = NULL;
  //virtual IAnimationObject* MakeInstance() = NULL;

  Animation::AnimationPack *ApplyAnimation( const std::string& templateName, const std::string& animationName = "default" );
  Animation::AnimationPack *ApplySubAnimation( const std::string& animationName );
  inline Animation::AnimationPack *GetAnimation() {
    return this->_animation;
  }

protected:
  Animation::AnimationPack
    *_animation,
    *_animationTemplate;

private:
  IObject( const IObject& );
  IObject& operator=( const IObject& );
};


class AnimationObject: public AnimationTemplate
{
public:
  AnimationObject( IAnimationObject *object );
  virtual ~AnimationObject();
  virtual IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType );
  static IAnimationParameter* SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationObjectThis );
  virtual IAnimationObject* MakeObjectInstance();
  virtual void MakeFromTemplate( const IAnimation &animation );

private:
  IObject *_object;

  AnimationObject();
  AnimationObject( const AnimationObject& );
  AnimationObject& operator=( const AnimationObject& );
};


#endif
