#include "animationtemplate.h"


Animation::AnimationTemplate::AnimationTemplate( const std::string& setName )
:Animation::IAnimation( setName )
{
}


Animation::AnimationTemplate::~AnimationTemplate() {
}


Animation::IAnimationObject* Animation::AnimationTemplate::MakeObjectInstance() {
  return NULL;
}//MakeObjectInstance
