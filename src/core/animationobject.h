#ifndef __ANIMATIONOBJECT_H__
#define __ANIMATIONOBJECT_H__

/*
* Единичная анимация Object
*/

#include "animationtemplate.h"

namespace Animation {
  class AnimationPack;
  class AnimationSetAction;

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
  virtual Vec3& GetPositionPtr() = NULL;
  virtual float* GetRenderableRotationPtr() = NULL;
  virtual Vec4& GetColorPtr() = NULL;
  virtual Vec3& GetCollisionSquareSize() = NULL;
  virtual Vec3& GetCollisionOffsetPtr() = NULL;
  //virtual IAnimationObject* MakeInstance() = NULL;

  Animation::AnimationPack *ApplyAnimation( const AnimationSetAction& actionAfterComplete, const std::string& templateName, const std::string& animationName = "default", float startTime = 0.0f );
  Animation::AnimationPack *ApplySubAnimation( const AnimationSetAction& actionAfterComplete, const std::string& animationName );
  void StopAnimation();
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
  AnimationObject( IAnimationObject *object, const std::string& setName );
  virtual ~AnimationObject();
  virtual IAnimationParameter* SetParameter( AnimationSpriteParameterType parameterType );
  static IAnimationParameter* SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationObjectThis );
  virtual IAnimationObject* MakeObjectInstance( const std::string& setName );
  virtual void MakeFromTemplate( const IAnimation &animation );

private:
  IObject *_object;

  AnimationObject();
  AnimationObject( const AnimationObject& );
  AnimationObject& operator=( const AnimationObject& );
};

};


#endif
