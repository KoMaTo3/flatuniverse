#ifndef __ANIMATIONPARAMETER_H__
#define __ANIMATIONPARAMETER_H__


#include <vector>


#include "klib.h"
#include "interpolation.h"


namespace Animation {


template< class T >
class KeyFrame {
public:
  KeyFrame( float setTime, const T& setValue, InterpolationType setInterpolation )
    :time( setTime ), value( setValue ), interpolation( setInterpolation ) {
  }
  float time;
  InterpolationType interpolation;
  T value;
};


class IAnimationParameter
{
public:
  IAnimationParameter();
  virtual ~IAnimationParameter();
  virtual void Update( float animationTime ) = 0;
  virtual void __Dump( const std::string &prefix = "" ) = NULL;
  virtual void MakeFromTemplate( const IAnimationParameter& parameter ) = NULL;
};


/*
* 1 Float
*/
class AnimationParameterFloat1: public IAnimationParameter {
public:
  AnimationParameterFloat1();
  virtual ~AnimationParameterFloat1();
  virtual void Update( float animationTime );
  void Bind( float *setValue );
  AnimationParameterFloat1* AddKeyFrame( float time, float value, InterpolationType interpolation );
  __forceinline const float GetValue() const {
    return ( this->_value ? *this->_value : 0.0f );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );

private:
  float *_value;
  typedef KeyFrame< float > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;
};


/*
* 2 Float
*/
class AnimationParameterFloat2: public IAnimationParameter {
public:
  AnimationParameterFloat2();
  virtual ~AnimationParameterFloat2();
  virtual void Update( float animationTime );
  void Bind( float *setValue0, float *setValue1 );
  void Bind( Vec2& setValue );
  AnimationParameterFloat2* AddKeyFrame( float time, float value0, float value1, InterpolationType interpolation );
  AnimationParameterFloat2* AddKeyFrame( float time, const Vec2& value, InterpolationType interpolation );
  __forceinline const Vec2 GetValue() const {
    return ( this->_value ? Vec2( *this->_value[ 0 ], *this->_value[ 1 ] ) : Vec2Null );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );

private:
  float *_value[ 2 ];
  typedef KeyFrame< Vec2 > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;
};


/*
* 3 Float
*/
class AnimationParameterFloat3: public IAnimationParameter {
public:
  AnimationParameterFloat3();
  virtual ~AnimationParameterFloat3();
  virtual void Update( float animationTime );
  void Bind( float *setValue0, float *setValue1, float *setValue2 );
  void Bind( Vec3& setValue );
  AnimationParameterFloat3* AddKeyFrame( float time, float value0, float value1, float value2, InterpolationType interpolation );
  AnimationParameterFloat3* AddKeyFrame( float time, const Vec3& value, InterpolationType interpolation );
  __forceinline const Vec3 GetValue() const {
    return ( this->_value ? Vec3( *this->_value[ 0 ], *this->_value[ 1 ], *this->_value[ 2 ] ) : Vec3Null );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );

private:
  float *_value[ 3 ];
  typedef KeyFrame< Vec3 > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;
};


/*
* 4 Float
*/
class AnimationParameterFloat4: public IAnimationParameter {
public:
  AnimationParameterFloat4();
  virtual ~AnimationParameterFloat4();
  virtual void Update( float animationTime );
  void Bind( float *setValue0, float *setValue1, float *setValue2, float *setValue3 );
  void Bind( Vec4& setValue );
  AnimationParameterFloat4* AddKeyFrame( float time, float value0, float value1, float value2, float value3, InterpolationType interpolation );
  AnimationParameterFloat4* AddKeyFrame( float time, const Vec4& value, InterpolationType interpolation );
  __forceinline const Vec4 GetValue() const {
    return ( this->_value ? Vec4( *this->_value[ 0 ], *this->_value[ 1 ], *this->_value[ 2 ], *this->_value[ 3 ] ) : Vec4Null );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );

private:
  float *_value[ 4 ];
  typedef KeyFrame< Vec4 > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;
};


/*
* string
*/
class AnimationParameterString: public IAnimationParameter {
public:
  AnimationParameterString();
  virtual ~AnimationParameterString();
  virtual void Update( float animationTime );
  void Bind( std::string *setValue, bool *onChangeFlag );
  AnimationParameterString* AddKeyFrame( float time, const std::string &value );
  __forceinline const std::string GetValue() const {
    return ( this->_value ? *this->_value : "" );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );

private:
  std::string *_value;
  bool *_valueChanged;
  std::hash< std::string > _valueHasher;
  size_t _valueHash;
  typedef KeyFrame< std::string > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;

  void _SetValue( const std::string &newValue );
};


/*
* bool
*/
class AnimationParameterBool: public IAnimationParameter {
public:
  AnimationParameterBool();
  virtual ~AnimationParameterBool();
  virtual void Update( float animationTime );
  void Bind( bool *setValue );
  AnimationParameterBool* AddKeyFrame( float time, bool value );
  __forceinline bool GetValue() const {
    return ( this->_value ? *this->_value : false );
  }
  virtual void __Dump( const std::string &prefix = "" );
  virtual void MakeFromTemplate( const IAnimationParameter& parameter );
  void SetValue( bool newValue );

private:
  bool *_value;
  typedef KeyFrame< bool > KeyFrameType;
  typedef std::vector< KeyFrameType > KeyFramesList;
  KeyFramesList _keyFrames;
};


}

#endif
