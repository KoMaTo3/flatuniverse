#include "animationobject.h"
#include "animationpack.h"
#include "file.h"


Animation::IObject::IObject()
:_animation( NULL ), _animationTemplate( NULL )
{
}


Animation::IObject::~IObject() {
  if( this->_animation ) {
    delete this->_animation;
    this->_animation = NULL;
  }
}


Animation::AnimationPack* Animation::IObject::ApplyAnimation( const Animation::AnimationSetAction& actionAfterComplete, const std::string& templateName, const std::string& animationName, float startTime ) {
  Animation::AnimationPack *templatePack = Animation::GetPackTemplate( templateName );
  if( !templatePack ) {
    __log.PrintInfo( Filelevel_ERROR, "IObject::ApplyAnimation => animation '%s' not found", templateName.c_str() );
    return NULL;
  }

  if( this->_animationTemplate == templatePack ) {
    return this->ApplySubAnimation( actionAfterComplete, animationName );
  }

  if( this->_animation ) {
    this->_animation->SetEnabled( false );
    delete this->_animation;
  }

  this->_animation = new Animation::AnimationPack();
  ( this->_animation )->MakeFromTemplate< AnimationObject >( *templatePack, AnimationObject::SetParameterOfExternAnimation, this );
  ( this->_animation )->SetCurrentAnimation( actionAfterComplete, animationName, startTime );
  this->_animationTemplate = templatePack;

  return this->_animation;
}//ApplyAnimation


Animation::AnimationPack* Animation::IObject::ApplySubAnimation( const Animation::AnimationSetAction& actionAfterComplete, const std::string& animationName ) {
  if( !this->_animation ) {
    __log.PrintInfo( Filelevel_ERROR, "IObject::ApplySubAnimation => animation '%s' not found", animationName.c_str() );
    return NULL;
  }

  ( this->_animation )->SetCurrentAnimation( actionAfterComplete, animationName );
  return this->_animation;
}//ApplySubAnimation


void Animation::IObject::StopAnimation() {
  if( !this->_animation ) {
    return;
  }

  this->_animation->SetEnabled( false );
  delete this->_animation;
  this->_animation = NULL;
  this->_animationTemplate = NULL;
}//StopAnimation


Animation::AnimationObject::AnimationObject( IAnimationObject* object, const std::string& setName )
:AnimationTemplate( setName ), _object( ( IObject* ) object )
{
  static_cast< AnimationParameterBool* >( this->SetParameter( ENABLED ) )->Bind( static_cast< IObject* >( object )->GetEnabledPtr() );
}


Animation::AnimationObject::~AnimationObject() {
  //if( this->_object ) {
  //  delete this->_object;
  //}
}


/*
* SetParameter
* Делает один из параметров анимированным, возвращает его аниматор
*
*/
Animation::IAnimationParameter* Animation::AnimationObject::SetParameter( Animation::AnimationSpriteParameterType parameterType ) {
  Animation::IAnimationParameter *parameter = NULL;
 
  ParametersList::iterator iter = this->_parameters.find( parameterType );
  if( iter != this->_parameters.end() ) {
    return &( *iter->second );
  }

  switch( parameterType ) {
    /*
  case POSITION: {
    AnimationParameterFloat3 *value = new AnimationParameterFloat3();
    parameter = value;
    value->Bind( this->_object->GetPositionPtr() );
    break;
    }
  case POSITION_X: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_object->GetPositionPtr().x );
    break;
    }
  case POSITION_Y: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_object->GetPositionPtr().y );
    break;
    }
  case COLOR: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetColorPtr() );
    break;
    }
  case TEXTURE_COORDINATES: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetTextureCoordsPtr() );
    break;
    }
  case ROTATION: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( this->_object->GetRotationPtr() );
    break;
    }
  case SCALE: {
    __log.PrintInfo( Filelevel_DEBUG, "AnimationObject::SetParameter => binded SCALE[%p]", &this->_object->GetScalePtr() );
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_object->GetScalePtr() );
    break;
    }
    */
  case ENABLED: {
    AnimationParameterBool *value = new Animation::AnimationParameterBool();
    parameter = value;
    value->Bind( this->_object->GetEnabledPtr() );
    break;
    }
  case RENDERABLE_SIZE: {
    AnimationParameterFloat2 *value = new Animation::AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_object->GetRenderableSize() );
    break;
    }
  case RENDERABLE_TEXTURE_COORDINATES: {
    AnimationParameterFloat4 *value = new Animation::AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetTextureCoords() );
    break;
    }
  case RENDERABLE_TEXTURE_NAME: {
    AnimationParameterString *value = new Animation::AnimationParameterString();
    parameter = value;
    value->Bind( &this->_object->GetTextureNamePtr(), &this->_object->GetTextureChangedFlag() );
    break;
    }
  case POSITION: {
    AnimationParameterFloat2 *value = new Animation::AnimationParameterFloat2();
    parameter = value;
    value->Bind( &this->_object->GetPositionPtr().x, &this->_object->GetPositionPtr().y );
    break;
    }
  case RENDERABLE_ROTATION: {
    AnimationParameterFloat1 *value = new Animation::AnimationParameterFloat1();
    parameter = value;
    value->Bind( this->_object->GetRenderableRotationPtr() );
    break;
    }
  case COLOR: {
    AnimationParameterFloat4 *value = new Animation::AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetColorPtr() );
    break;
    }
  case COLLISION_SQUARE: {
    //__log.PrintInfo( Filelevel_DEBUG, "AnimationObject::SetParameter => COLLISION_SQUARE" );
    AnimationParameterFloat3 *value = new Animation::AnimationParameterFloat3();
    parameter = value;
    value->Bind( this->_object->GetCollisionSquareSize() );
    break;
    }
  case COLLISION_OFFSET: {
    //__log.PrintInfo( Filelevel_DEBUG, "AnimationObject::SetParameter => COLLISION_OFFSET" );
    AnimationParameterFloat3 *value = new Animation::AnimationParameterFloat3();
    parameter = value;
    value->Bind( this->_object->GetCollisionOffsetPtr() );
    break;
    }
  default:
    __log.PrintInfo( Filelevel_ERROR, "AnimationObject::SetParameter => can't bind parameter x%X", parameterType );
  }

  if( parameter ) {
    this->_parameters.insert( std::make_pair( parameterType, ParameterPtr( parameter ) ) );
  }

  return parameter;
}//SetParameter


Animation::IAnimationParameter* Animation::AnimationObject::SetParameterOfExternAnimation( Animation::AnimationSpriteParameterType parameterType, void* AnimationObjectThis ) {
  return ( static_cast< AnimationObject* >( AnimationObjectThis ) )->SetParameter( parameterType );
}


Animation::IAnimationObject* Animation::AnimationObject::MakeObjectInstance( const std::string& setName ) {
  return this->_object->MakeInstance( setName );
}//MakeObjectInstance


void Animation::AnimationObject::MakeFromTemplate( const Animation::IAnimation &animation ) {
  const Animation::AnimationObject *anim = static_cast< const Animation::AnimationObject* >( &animation );
  for( auto &parameter: anim->_parameters ) {
    this->SetParameter( parameter.first )->MakeFromTemplate( *parameter.second );
  }
}//MakeFromTemplate
