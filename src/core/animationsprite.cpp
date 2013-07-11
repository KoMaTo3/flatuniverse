#include "animationsprite.h"
#include "animationpack.h"
#include "file.h"


ISprite::~ISprite() {
  if( this->_animation ) {
    delete this->_animation;
    this->_animation = NULL;
  }
}


Animation::AnimationPack* ISprite::ApplyAnimation( const std::string& templateName, const std::string& animationName ) {
  __log.PrintInfo( Filelevel_DEBUG, "ISprite::ApplyAnimation" );
  Animation::AnimationPack *templatePack = Animation::GetPackTemplate( templateName );
  if( !templatePack ) {
    __log.PrintInfo( Filelevel_ERROR, "ISprite::ApplyAnimation => animation '%s' not found", templateName.c_str() );
    return NULL;
  }

  __log.PrintInfo( Filelevel_DEBUG, ". animation[%p]", this->_animation );
  if( this->_animation ) {
    delete this->_animation;
  }

  __log.PrintInfo( Filelevel_DEBUG, ". new AnimationPack" );
  this->_animation = new Animation::AnimationPack();
  __log.PrintInfo( Filelevel_DEBUG, ". MakeFromTemplate" );
  ( this->_animation )->MakeFromTemplate< AnimationSprite >( *templatePack, AnimationSprite::SetParameterOfExternAnimation, this );
  __log.PrintInfo( Filelevel_DEBUG, ". SetCurrentAnimation '%s'", animationName.c_str() );
  ( this->_animation )->SetCurrentAnimation( animationName );
  __log.PrintInfo( Filelevel_DEBUG, ". done" );

  return this->_animation;
}//ApplyAnimation


Animation::AnimationPack* ISprite::ApplySubAnimation( const std::string& animationName ) {
  if( !this->_animation ) {
    __log.PrintInfo( Filelevel_ERROR, "ISprite::ApplySubAnimation => animation '%s' not found", animationName.c_str() );
    return NULL;
  }

  ( this->_animation )->SetCurrentAnimation( animationName );
  return this->_animation;
}//ApplySubAnimation


AnimationSprite::AnimationSprite( IAnimationObject* sprite )
:_sprite( ( ISprite* ) sprite )
{
  static_cast< AnimationParameterBool* >( this->SetParameter( ENABLED ) )->Bind( static_cast< ISprite* >( sprite )->GetEnabledPtr() );
}


AnimationSprite::~AnimationSprite() {
  if( this->_sprite ) {
    delete this->_sprite;
  }
}


/*
* SetParameter
* Делает один из параметров анимированным, возвращает его аниматор
*
*/
IAnimationParameter* AnimationSprite::SetParameter( AnimationSpriteParameterType parameterType ) {
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
    value->Bind( this->_sprite->GetPositionPtr() );
    break;
    }
  case POSITION_X: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_sprite->GetPositionPtr().x );
    break;
    }
  case POSITION_Y: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_sprite->GetPositionPtr().y );
    break;
    }
  case COLOR: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_sprite->GetColorPtr() );
    break;
    }
  case TEXTURE_NAME: {
    AnimationParameterString *value = new AnimationParameterString();
    parameter = value;
    value->Bind( &this->_sprite->GetTextureNamePtr(), &this->_sprite->GetTextureChangedFlag() );
    break;
    }
  case TEXTURE_COORDINATES: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_sprite->GetTextureCoordsPtr() );
    break;
    }
  case ROTATION: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( this->_sprite->GetRotationPtr() );
    break;
    }
  case SCALE: {
    __log.PrintInfo( Filelevel_DEBUG, "AnimationSprite::SetParameter => binded SCALE[%p]", &this->_sprite->GetScalePtr() );
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_sprite->GetScalePtr() );
    break;
    }
    */
  case ENABLED: {
    AnimationParameterBool *value = new AnimationParameterBool();
    parameter = value;
    value->Bind( this->_sprite->GetEnabledPtr() );
    break;
    }
                /*
  case OBJECT_SIZE: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_sprite->GetSizePtr() );
    break;
    }
    */
  default:
    __log.PrintInfo( Filelevel_ERROR, "AnimationSprite::SetParameter => can't bind parameter x%X", parameterType );
  }

  if( parameter ) {
    this->_parameters.insert( std::make_pair( parameterType, ParameterPtr( parameter ) ) );
  }

  return parameter;
}//SetParameter


IAnimationParameter* AnimationSprite::SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationSpriteThis ) {
  return ( static_cast< AnimationSprite* >( AnimationSpriteThis ) )->SetParameter( parameterType );
}


IAnimationObject* AnimationSprite::MakeObjectInstance() {
  return this->_sprite->MakeInstance();
}//MakeObjectInstance


void AnimationSprite::MakeFromTemplate( const IAnimation &animation ) {
  const AnimationSprite *anim = static_cast< const AnimationSprite* >( &animation );
  for( auto &parameter: anim->_parameters ) {
    this->SetParameter( parameter.first )->MakeFromTemplate( *parameter.second );
  }
}//MakeFromTemplate
