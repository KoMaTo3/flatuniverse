#include "kvector.h"
#include "kangles.h"


Vec2 Vec2Null( 0.0f, 0.0f );
Vec2 Vec2One ( 1.0f, 1.0f );
Vec3 Vec3Null( 0.0f, 0.0f, 0.0f );
Vec3 Vec3One ( 1.0f, 1.0f, 1.0f );
Vec4 Vec4Null( 0.0f, 0.0f, 0.0f, 0.0f );
Vec4 Vec4One ( 1.0f, 1.0f, 1.0f, 1.0f );

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
Void Vec2::Lerp( const Vec2 &v1, const Vec2 &v2, const Float l ) {
  if ( l <= 0.0f ) {
    (*this) = v1;
  } else if ( l >= 1.0f ) {
    (*this) = v2;
  } else {
    (*this) = v1 + l * ( v2 - v1 );
  }
}


//===============================================================
//
//  Vec3
//
//===============================================================

/*
=============
Vec3::ToYaw
=============
*/
Float Vec3::ToYaw( Void ) const {
  Float yaw;
  
  if ( ( y == 0.0f ) && ( x == 0.0f ) ) {
    yaw = 0.0f;
  } else {
    yaw = RAD2DEG( atan2( y, x ) );
    if ( yaw < 0.0f ) {
      yaw += 360.0f;
    }
  }

  return yaw;
}

/*
=============
Vec3::ToPitch
=============
*/
Float Vec3::ToPitch( Void ) const {
  Float  forward;
  Float  pitch;
  
  if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
    if ( z > 0.0f ) {
      pitch = 90.0f;
    } else {
      pitch = 270.0f;
    }
  } else {
    forward = ( Float )Math::Sqrt( x * x + y * y );
    pitch = RAD2DEG( atan2( z, forward ) );
    if ( pitch < 0.0f ) {
      pitch += 360.0f;
    }
  }

  return pitch;
}

/*
=============
Vec3::ToAngles
=============
*/
Angles Vec3::ToAngles( Void ) const {
  Float forward;
  Float yaw;
  Float pitch;
  
  if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
    yaw = 0.0f;
    if ( z > 0.0f ) {
      pitch = 90.0f;
    } else {
      pitch = 270.0f;
    }
  } else {
    yaw = RAD2DEG( atan2( y, x ) );
    if ( yaw < 0.0f ) {
      yaw += 360.0f;
    }

    forward = ( Float )Math::Sqrt( x * x + y * y );
    pitch = RAD2DEG( atan2( z, forward ) );
    if ( pitch < 0.0f ) {
      pitch += 360.0f;
    }
  }

  return Angles( -pitch, yaw, 0.0f );
}

/*
=============
Vec3::ToPolar
=============
*/
Polar3 Vec3::ToPolar( Void ) const {
  Float forward;
  Float yaw;
  Float pitch;
  
  if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
    yaw = 0.0f;
    if ( z > 0.0f ) {
      pitch = 90.0f;
    } else {
      pitch = 270.0f;
    }
  } else {
    yaw = RAD2DEG( atan2( y, x ) );
    if ( yaw < 0.0f ) {
      yaw += 360.0f;
    }

    forward = ( Float )Math::Sqrt( x * x + y * y );
    pitch = RAD2DEG( atan2( z, forward ) );
    if ( pitch < 0.0f ) {
      pitch += 360.0f;
    }
  }
  return Polar3( Math::Sqrt( x * x + y * y + z * z ), yaw, -pitch );
}

/*
=============
Vec3::ToMat3
=============
*/
Mat3 Vec3::ToMat3( Void ) const {
  Mat3  mat;
  Float  d;

  mat[0] = *this;
  d = x * x + y * y;
  if ( !d ) {
    mat[1][0] = 1.0f;
    mat[1][1] = 0.0f;
    mat[1][2] = 0.0f;
  } else {
    d = Math::InvSqrt( d );
    mat[1][0] = -y * d;
    mat[1][1] = x * d;
    mat[1][2] = 0.0f;
  }
  mat[2] = Cross( mat[1] );

  return mat;
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
Void Vec3::Lerp( const Vec3 &v1, const Vec3 &v2, const Float l ) {
  if ( l <= 0.0f ) {
    (*this) = v1;
  } else if ( l >= 1.0f ) {
    (*this) = v2;
  } else {
    (*this) = v1 + l * ( v2 - v1 );
  }
}

/*
=============
SLerp

Spherical linear interpolation from v1 to v2.
Vectors are expected to be normalized.
=============
*/
#define LERP_DELTA 1e-6

Void Vec3::SLerp( const Vec3 &v1, const Vec3 &v2, const Float t ) {
  Float omega, cosom, sinom, scale0, scale1;

  if ( t <= 0.0f ) {
    (*this) = v1;
    return;
  } else if ( t >= 1.0f ) {
    (*this) = v2;
    return;
  }

  cosom = v1 * v2;
  if ( ( 1.0f - cosom ) > LERP_DELTA ) {
    omega = acos( cosom );
    sinom = sin( omega );
    scale0 = sin( ( 1.0f - t ) * omega ) / sinom;
    scale1 = sin( t * omega ) / sinom;
  } else {
    scale0 = 1.0f - t;
    scale1 = t;
  }

  (*this) = ( v1 * scale0 + v2 * scale1 );
}

/*
=============
ProjectSelfOntoSphere

Projects the z component onto a sphere.
=============
*/
Void Vec3::ProjectSelfOntoSphere( const Float radius ) {
  Float rsqr = radius * radius;
  Float len = Length();
  if ( len  < rsqr * 0.5f ) {
    z = sqrt( rsqr - len );
  } else {
    z = rsqr / ( 2.0f * sqrt( len ) );
  }
}


Line2::Line2() {
}

Line2::Line2( Vec2 p, Vec2 q ) {
	a = p.y - q.y;
	b = q.x - p.x;
	c = - a * p.x - b * p.y;
	Normalize();
}

void Line2::Normalize() {
  float z = Math::Sqrt( a * a + b * b );
  if( Math::Fabs( z ) > Math::FLT_EPSILON_NUM ) {
    float invZ = 1.0f / z;
		a *= invZ;
    b *= invZ;
    c *= invZ;
  }
}

float Line2::Dist( Vec2 p ) const {
	return a * p.x + b * p.y + c;
}
