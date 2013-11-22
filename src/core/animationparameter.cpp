#include "animationparameter.h"
#include "file.h"


IAnimationParameter::~IAnimationParameter() {
}


IAnimationParameter::IAnimationParameter() {
}


/*
* Float 1
*/
AnimationParameterFloat1::AnimationParameterFloat1()
:_value( NULL )
{
}


AnimationParameterFloat1::~AnimationParameterFloat1() {
}


void AnimationParameterFloat1::Update( float animationTime ) {
  if( !this->_value ) {
    __log.PrintInfo( Filelevel_WARNING, "AnimationParameterFloat1::Update => value is NULL" );
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    *this->_value = 0.0f;
    __log.PrintInfo( Filelevel_WARNING, "AnimationParameterFloat1::Update => no key frames" );
    return;
  }

  if( size == 1 ) {
    *this->_value = this->_keyFrames.begin()->value;
    return;
  }

  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin(),
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterNext, iterPrev;
  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        iterNext = ++iter;
      } else {
        iterNext = iter;
        iterPrev = --iter;
      }
      float t = ( animationTime - iterPrev->time ) / ( iterNext->time - iterPrev->time );
      *this->_value = Interpolation::Apply( iterPrev->value, iterNext->value, t, iterNext->interpolation );
      return;
    }
    ++iter;
  }//while
  *this->_value = iterLast->value;
}//Update


void AnimationParameterFloat1::Bind( float *setValue ) {
  this->_value = setValue;
}//Bind


AnimationParameterFloat1* AnimationParameterFloat1::AddKeyFrame( float time, float value, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, interpolation ) );
  return this;
}//AddKeyFrame


void AnimationParameterFloat1::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value[%3.3f]\n", prefix.c_str(), this, this->_keyFrames.size(), *this->_value );
}//__Dump


void AnimationParameterFloat1::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterFloat1 *param = static_cast< const AnimationParameterFloat1* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value, key.interpolation );
  }
}//MakeFromTemplate



/*
* Float 2
*/
AnimationParameterFloat2::AnimationParameterFloat2() {
  this->_value[ 0 ] = NULL;
  this->_value[ 1 ] = NULL;
}


AnimationParameterFloat2::~AnimationParameterFloat2() {
}


void AnimationParameterFloat2::Update( float animationTime ) {
  if( !this->_value ) {
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    *this->_value[ 0 ] = 0.0f;
    *this->_value[ 1 ] = 0.0f;
    return;
  }

  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();

  if( size == 1 ) {
    *this->_value[ 0 ] = iterLast->value.x;
    *this->_value[ 1 ] = iterLast->value.y;
    return;
  }

  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin(),
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterNext, iterPrev;

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        iterNext = ++iter;
      } else {
        iterNext = iter;
        iterPrev = --iter;
      }
      float t = ( animationTime - iterPrev->time ) / ( iterNext->time - iterPrev->time );
      *this->_value[ 0 ] = Interpolation::Apply( iterPrev->value.x, iterNext->value.x, t, iterNext->interpolation );
      *this->_value[ 1 ] = Interpolation::Apply( iterPrev->value.y, iterNext->value.y, t, iterNext->interpolation );
      return;
    }
    ++iter;
  }//while
  *this->_value[ 0 ] = iterLast->value.x;
  *this->_value[ 1 ] = iterLast->value.y;
}//Update


void AnimationParameterFloat2::Bind( float *setValue0, float *setValue1 ) {
  this->_value[ 0 ] = setValue0;
  this->_value[ 1 ] = setValue1;
}//Bind


void AnimationParameterFloat2::Bind( Vec2& setValue ) {
  this->_value[ 0 ] = &setValue.x;
  this->_value[ 1 ] = &setValue.y;
}//Bind


AnimationParameterFloat2* AnimationParameterFloat2::AddKeyFrame( float time, float value0, float value1, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, Vec2( value0, value1 ), interpolation ) );
  return this;
}//AddKeyFrame


AnimationParameterFloat2* AnimationParameterFloat2::AddKeyFrame( float time, const Vec2& value, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, interpolation ) );
  return this;
}//AddKeyFrame


void AnimationParameterFloat2::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value[%3.3f; %3.3f]\n", prefix.c_str(), this, this->_keyFrames.size(), *this->_value[ 0 ], *this->_value[ 1 ] );
}//__Dump


void AnimationParameterFloat2::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterFloat2 *param = static_cast< const AnimationParameterFloat2* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value, key.interpolation );
  }
}//MakeFromTemplate


/*
* Float 3
*/
AnimationParameterFloat3::AnimationParameterFloat3() {
  this->_value[ 0 ] = NULL;
  this->_value[ 1 ] = NULL;
  this->_value[ 2 ] = NULL;
}


AnimationParameterFloat3::~AnimationParameterFloat3() {
}


void AnimationParameterFloat3::Update( float animationTime ) {
  if( !this->_value ) {
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    *this->_value[ 0 ] = 0.0f;
    *this->_value[ 1 ] = 0.0f;
    *this->_value[ 2 ] = 0.0f;
    return;
  }

  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();
  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin();

  if( size == 1 ) {
    *this->_value[ 0 ] = iter->value.x;
    *this->_value[ 1 ] = iter->value.y;
    *this->_value[ 2 ] = iter->value.z;
    return;
  }

  KeyFramesList::const_iterator
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterNext, iterPrev;

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        iterNext = ++iter;
      } else {
        iterNext = iter;
        iterPrev = --iter;
      }
      float t = ( animationTime - iterPrev->time ) / ( iterNext->time - iterPrev->time );
      *this->_value[ 0 ] = Interpolation::Apply( iterPrev->value.x, iterNext->value.x, t, iterNext->interpolation );
      *this->_value[ 1 ] = Interpolation::Apply( iterPrev->value.y, iterNext->value.y, t, iterNext->interpolation );
      *this->_value[ 2 ] = Interpolation::Apply( iterPrev->value.z, iterNext->value.z, t, iterNext->interpolation );
      return;
    }
    ++iter;
  }//while
  *this->_value[ 0 ] = iterLast->value.x;
  *this->_value[ 1 ] = iterLast->value.y;
  *this->_value[ 2 ] = iterLast->value.z;
}//Update


void AnimationParameterFloat3::Bind( float *setValue0, float *setValue1, float *setValue2 ) {
  this->_value[ 0 ] = setValue0;
  this->_value[ 1 ] = setValue1;
  this->_value[ 2 ] = setValue2;
}//Bind


void AnimationParameterFloat3::Bind( Vec3& setValue ) {
  this->_value[ 0 ] = &setValue.x;
  this->_value[ 1 ] = &setValue.y;
  this->_value[ 2 ] = &setValue.z;
}//Bind


AnimationParameterFloat3* AnimationParameterFloat3::AddKeyFrame( float time, float value0, float value1, float value2, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, Vec3( value0, value1, value2 ), interpolation ) );
  return this;
}//AddKeyFrame


AnimationParameterFloat3* AnimationParameterFloat3::AddKeyFrame( float time, const Vec3& value, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, interpolation ) );
  return this;
}//AddKeyFrame


void AnimationParameterFloat3::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value[%3.3f; %3.3f; %3.3f]\n", prefix.c_str(), this, this->_keyFrames.size(), *this->_value[ 0 ], *this->_value[ 1 ], *this->_value[ 2 ] );
}//__Dump


void AnimationParameterFloat3::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterFloat3 *param = static_cast< const AnimationParameterFloat3* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value, key.interpolation );
  }
}//MakeFromTemplate


/*
* Float 4
*/
AnimationParameterFloat4::AnimationParameterFloat4() {
  this->_value[ 0 ] = NULL;
  this->_value[ 1 ] = NULL;
  this->_value[ 2 ] = NULL;
  this->_value[ 3 ] = NULL;
}


AnimationParameterFloat4::~AnimationParameterFloat4() {
}


void AnimationParameterFloat4::Update( float animationTime ) {
  if( !this->_value ) {
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    *this->_value[ 0 ] = 0.0f;
    *this->_value[ 1 ] = 0.0f;
    *this->_value[ 2 ] = 0.0f;
    *this->_value[ 3 ] = 0.0f;
    return;
  }

  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();
  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin();

  if( size == 1 ) {
    *this->_value[ 0 ] = iter->value.x;
    *this->_value[ 1 ] = iter->value.y;
    *this->_value[ 2 ] = iter->value.z;
    *this->_value[ 3 ] = iter->value.w;
    return;
  }

  KeyFramesList::const_iterator
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterNext, iterPrev;

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        iterNext = ++iter;
      } else {
        iterNext = iter;
        iterPrev = --iter;
      }
      float t = ( animationTime - iterPrev->time ) / ( iterNext->time - iterPrev->time );
      *this->_value[ 0 ] = Interpolation::Apply( iterPrev->value.x, iterNext->value.x, t, iterNext->interpolation );
      *this->_value[ 1 ] = Interpolation::Apply( iterPrev->value.y, iterNext->value.y, t, iterNext->interpolation );
      *this->_value[ 2 ] = Interpolation::Apply( iterPrev->value.z, iterNext->value.z, t, iterNext->interpolation );
      *this->_value[ 3 ] = Interpolation::Apply( iterPrev->value.w, iterNext->value.w, t, iterNext->interpolation );
      return;
    }
    ++iter;
  }//while
  *this->_value[ 0 ] = iterLast->value.x;
  *this->_value[ 1 ] = iterLast->value.y;
  *this->_value[ 2 ] = iterLast->value.z;
  *this->_value[ 3 ] = iterLast->value.w;
}//Update


void AnimationParameterFloat4::Bind( float *setValue0, float *setValue1, float *setValue2, float *setValue3 ) {
  this->_value[ 0 ] = setValue0;
  this->_value[ 1 ] = setValue1;
  this->_value[ 2 ] = setValue2;
  this->_value[ 3 ] = setValue3;
}//Bind


void AnimationParameterFloat4::Bind( Vec4& setValue ) {
  this->_value[ 0 ] = &setValue.x;
  this->_value[ 1 ] = &setValue.y;
  this->_value[ 2 ] = &setValue.z;
  this->_value[ 3 ] = &setValue.w;
}//Bind


AnimationParameterFloat4* AnimationParameterFloat4::AddKeyFrame( float time, float value0, float value1, float value2, float value3, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, Vec4( value0, value1, value2, value3 ), interpolation ) );
  return this;
}//AddKeyFrame


AnimationParameterFloat4* AnimationParameterFloat4::AddKeyFrame( float time, const Vec4& value, InterpolationType interpolation ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, interpolation ) );
  return this;
}//AddKeyFrame


void AnimationParameterFloat4::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value[%3.3f; %3.3f; %3.3f; %3.3f]\n", prefix.c_str(), this, this->_keyFrames.size(), *this->_value[ 0 ], *this->_value[ 1 ], *this->_value[ 2 ], *this->_value[ 3 ] );
}//__Dump


void AnimationParameterFloat4::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterFloat4 *param = static_cast< const AnimationParameterFloat4* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value, key.interpolation );
  }
}//MakeFromTemplate


/*
* String
*/
AnimationParameterString::AnimationParameterString()
:_value( NULL ), _valueChanged( NULL ), _valueHash( 0 )
{
}


AnimationParameterString::~AnimationParameterString() {
}


void AnimationParameterString::Update( float animationTime ) {
  if( !this->_value ) {
    __log.PrintInfo( Filelevel_WARNING, "AnimationParameterString::Update => value is NULL" );
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    this->_SetValue( "" );
    __log.PrintInfo( Filelevel_WARNING, "AnimationParameterString::Update => no key frames" );
    return;
  }

  if( size == 1 ) {
    this->_SetValue( this->_keyFrames.begin()->value );
    return;
  }

  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin(),
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterPrev;
  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        //iterNext = ++iter;
      } else {
        //iterNext = iter;
        iterPrev = --iter;
      }
      this->_SetValue( iterPrev->value );
      return;
    }
    ++iter;
  }//while
  this->_SetValue( iterLast->value );
}//Update


void AnimationParameterString::Bind( std::string *setValue, bool *onChangeFlag ) {
  this->_value = setValue;
  this->_valueChanged = onChangeFlag;
  this->_valueHash = this->_valueHasher( *this->_value );
}//Bind


AnimationParameterString* AnimationParameterString::AddKeyFrame( float time, const std::string &value ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, FLAT ) );
  return this;
}//AddKeyFrame


void AnimationParameterString::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value['%s']\n", prefix.c_str(), this, this->_keyFrames.size(), this->_value->c_str() );
}//__Dump


void AnimationParameterString::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterString *param = static_cast< const AnimationParameterString* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value );
  }
}//MakeFromTemplate


void AnimationParameterString::_SetValue( const std::string &newValue ) {
  *this->_value = newValue;
  size_t newHash = this->_valueHasher( *this->_value );
  *this->_valueChanged = ( this->_valueHash != newHash );
  this->_valueHash = newHash;
}//_SetValue


/*
* Boolean
*/
AnimationParameterBool::AnimationParameterBool()
:_value( NULL )
{
}


AnimationParameterBool::~AnimationParameterBool() {
}


void AnimationParameterBool::Update( float animationTime ) {
  if( !this->_value ) {
    __log.PrintInfo( Filelevel_WARNING, "AnimationParameterBool::Update => value is NULL" );
    return;
  }

  int size = this->_keyFrames.size();

  if( !size ) {
    //__log.PrintInfo( Filelevel_WARNING, "AnimationParameterBool::Update => no keyframes" );
    return;
  }

  if( size == 1 ) {
    *this->_value = this->_keyFrames.begin()->value;
    return;
  }

  KeyFramesList::const_iterator
    iter = this->_keyFrames.begin(),
    iterEnd = this->_keyFrames.end(),
    iterBegin = this->_keyFrames.begin(),
    iterPrev;
  KeyFramesList::const_reverse_iterator
    iterLast = this->_keyFrames.rbegin();

  if( animationTime < iterBegin->time ) {
    animationTime = iterBegin->time;
  } else if( animationTime > iterLast->time ) {
    animationTime = iterLast->time;
  }

  while( iter != iterEnd ) {
    if( animationTime < iter->time ) {
      if( iter == iterBegin ) {
        iterPrev = iter;
        //iterNext = ++iter;
      } else {
        //iterNext = iter;
        iterPrev = --iter;
      }
      *this->_value = iterPrev->value;
      return;
    }
    ++iter;
  }//while
  *this->_value = iterLast->value;
}//Update


void AnimationParameterBool::Bind( bool *setValue ) {
  this->_value = setValue;
}//Bind


AnimationParameterBool* AnimationParameterBool::AddKeyFrame( float time, bool value ) {
  this->_keyFrames.push_back( KeyFrameType( time, value, FLAT ) );
  return this;
}//AddKeyFrame


void AnimationParameterBool::__Dump( const std::string &prefix ) {
  //LOGD( "%s. parameter[%p] keyFramesCount[%d] value[%d]\n", prefix.c_str(), this, this->_keyFrames.size(), *this->_value );
}//__Dump


void AnimationParameterBool::MakeFromTemplate( const IAnimationParameter& parameter ) {
  const AnimationParameterBool *param = static_cast< const AnimationParameterBool* >( &parameter );
  for( auto &key: param->_keyFrames ) {
    this->AddKeyFrame( key.time, key.value );
  }
}//MakeFromTemplate


void AnimationParameterBool::SetValue( bool newValue ) {
  *this->_value = newValue;
}//SetValue
