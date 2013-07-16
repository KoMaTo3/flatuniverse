#include "animationtemplate.h"


AnimationTemplate::AnimationTemplate( const std::string& setName )
:IAnimation( setName )
{
}


AnimationTemplate::~AnimationTemplate() {
}


IAnimationObject* AnimationTemplate::MakeObjectInstance() {
  return NULL;
}//MakeObjectInstance
