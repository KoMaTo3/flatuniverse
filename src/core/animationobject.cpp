#include "animationobject.h"
#include "animationpack.h"
#include "file.h"


IObject::IObject()
:_animation( NULL ), _animationTemplate( NULL )
{
}


IObject::~IObject() {
  if( this->_animation ) {
    delete this->_animation;
    this->_animation = NULL;
  }
}


Animation::AnimationPack* IObject::ApplyAnimation( const std::string& templateName, const std::string& animationName ) {
  Animation::AnimationPack *templatePack = Animation::GetPackTemplate( templateName );
  if( !templatePack ) {
    __log.PrintInfo( Filelevel_ERROR, "IObject::ApplyAnimation => animation '%s' not found", templateName.c_str() );
    return NULL;
  }

  if( this->_animationTemplate == templatePack ) {
    return this->ApplySubAnimation( animationName );
  }

  if( this->_animation ) {
    this->_animation->SetEnabled( false );
    delete this->_animation;
  }

  this->_animation = new Animation::AnimationPack();
  ( this->_animation )->MakeFromTemplate< AnimationObject >( *templatePack, AnimationObject::SetParameterOfExternAnimation, this );
  ( this->_animation )->SetCurrentAnimation( animationName );
  this->_animationTemplate = templatePack;

  return this->_animation;
}//ApplyAnimation


Animation::AnimationPack* IObject::ApplySubAnimation( const std::string& animationName ) {
  if( !this->_animation ) {
    __log.PrintInfo( Filelevel_ERROR, "IObject::ApplySubAnimation => animation '%s' not found", animationName.c_str() );
    return NULL;
  }

  ( this->_animation )->SetCurrentAnimation( animationName );
  return this->_animation;
}//ApplySubAnimation


void IObject::StopAnimation() {
  if( !this->_animation ) {
    return;
  }

  this->_animation->SetEnabled( false );
  delete this->_animation;
  this->_animation = NULL;
  this->_animationTemplate = NULL;
}//StopAnimation


AnimationObject::AnimationObject( IAnimationObject* object, const std::string& setName )
:AnimationTemplate( setName ), _object( ( IObject* ) object )
{
  static_cast< AnimationParameterBool* >( this->SetParameter( ENABLED ) )->Bind( static_cast< IObject* >( object )->GetEnabledPtr() );
}


AnimationObject::~AnimationObject() {
  if( this->_object ) {
    delete this->_object;
  }
}


/*
* SetParameter
* Делает один из параметров анимированным, возвращает его аниматор
*
*/
IAnimationParameter* AnimationObject::SetParameter( AnimationSpriteParameterType parameterType ) {
  IAnimationParameter *parameter = NULL;
 
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
    AnimationParameterBool *value = new AnimationParameterBool();
    parameter = value;
    value->Bind( this->_object->GetEnabledPtr() );
    break;
    }
  case RENDERABLE_SIZE: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_object->GetRenderableSize() );
    break;
    }
  case RENDERABLE_TEXTURE_COORDINATES: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetTextureCoords() );
    break;
    }
  case RENDERABLE_TEXTURE_NAME: {
    AnimationParameterString *value = new AnimationParameterString();
    parameter = value;
    value->Bind( &this->_object->GetTextureNamePtr(), &this->_object->GetTextureChangedFlag() );
    break;
    }
  case POSITION: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( &this->_object->GetPositionPtr().x, &this->_object->GetPositionPtr().y );
    break;
    }
  case RENDERABLE_ROTATION: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( this->_object->GetRenderableRotationPtr() );
    break;
    }
  case COLOR: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_object->GetColorPtr() );
    break;
    }
  case COLLISION_SQUARE: {
    __log.PrintInfo( Filelevel_DEBUG, "AnimationObject::SetParameter => COLLISION_SQUARE" );
    AnimationParameterFloat3 *value = new AnimationParameterFloat3();
    parameter = value;
    value->Bind( this->_object->GetCollisionSquareSize() );
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


IAnimationParameter* AnimationObject::SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationObjectThis ) {
  return ( static_cast< AnimationObject* >( AnimationObjectThis ) )->SetParameter( parameterType );
}


IAnimationObject* AnimationObject::MakeObjectInstance( const std::string& setName ) {
  return this->_object->MakeInstance( setName );
}//MakeObjectInstance


void AnimationObject::MakeFromTemplate( const IAnimation &animation ) {
  const AnimationObject *anim = static_cast< const AnimationObject* >( &animation );
  for( auto &parameter: anim->_parameters ) {
    this->SetParameter( parameter.first )->MakeFromTemplate( *parameter.second );
  }
}//MakeFromTemplate
