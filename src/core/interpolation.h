#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__


#include <string>
#include <hash_map>


enum InterpolationType {
  UNKNOWN,
  FLAT,
  LINEAR
};


namespace Interpolation {

float Apply( const float& startValue, const float& endValue, const float& time, const InterpolationType& interpolation );
InterpolationType StringToInterpolation( const std::string &str );

};


#endif
