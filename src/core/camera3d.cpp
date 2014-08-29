#include "camera3d.h"
#include "tools.h"

using namespace Engine;

Camera3d::Camera3d()
:planeNear( 0.01f ), planeFar( 1000.0f ), FOV( Math::HALF_PI ), aspect( 1.0f ), position( Vec3Null ), rotation( 0.0f, 0.0f, 0.0f, 1.0f ), matrixChanged( true ) {
}

Camera3d::~Camera3d() {
}


Camera3d* Camera3d::SetPosition( const Vec3& setPosition ) {
  if( this->position.x != setPosition.x || this->position.y != setPosition.y || this->position.z != setPosition.z ) {
    this->matrixChanged = true;
  }
  this->position = setPosition;

  return this;
}//SetPosition


Camera3d* Camera3d::SetFOV( const float radians ) {
  if( radians != this->FOV ) {
    this->matrixChanged = true;
  }
  this->FOV = radians;

  return this;
}//SetFOV


Camera3d* Camera3d::SetRotation( const float yaw, const float pitch, const float roll ) {
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;

	Math::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	Math::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	Math::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

  Quat oldQuat( this->rotation );

	this->rotation.x = cxsy * sz - sxcy * cz;
  this->rotation.y = -cxsy * cz - sxcy * sz;
  this->rotation.z = sxsy * cz - cxcy * sz;
  this->rotation.w = cxcy * cz + sxsy * sz;

  if( this->rotation.x != oldQuat.x ||
    this->rotation.y != oldQuat.y ||
    this->rotation.z != oldQuat.z ||
    this->rotation.w != oldQuat.w ) {
      this->matrixChanged = true;
  }

  return this;
}//SetRotation


void Camera3d::Update() {
  if( this->matrixChanged ) {
    Mat4 matTranslation;
    matTranslation.Identity();
    matTranslation[3][0] = this->position.x;
    matTranslation[3][1] = this->position.y;
    matTranslation[3][2] = this->position.z;

    const float h = 1.0f / Math::Tan16( this->FOV * 0.5f );
    float negDepth = this->planeNear - this->planeFar;
 
    this->matrixProjection.Identity();
    this->matrixWorld.Identity();

    this->matrixProjection[ 0 ][ 0 ] = h / this->aspect;
    this->matrixProjection[ 1 ][ 1 ] = h;
    this->matrixProjection[ 2 ][ 2 ] = ( this->planeFar + this->planeNear ) / negDepth;
    this->matrixProjection[ 2 ][ 3 ] = -1.0f;
    this->matrixProjection[ 3 ][ 2 ] = 2.0f * ( this->planeNear * this->planeFar ) / negDepth;
    this->matrixProjection[ 3 ][ 3 ] = 0.0f;
    this->matrixProjection.TransposeSelf();

    Mat4 matRotFromQuat( this->rotation.ToMat4() );

    //this->matrixWorld = matTranslation * matRotFromQuat;
    this->matrixWorld = matRotFromQuat * matTranslation;
    this->matrixWorld.TransposeSelf();

    //this->matrixProjection = matTranslation * matRotFromQuat * this->matrixProjection;
    this->matrixChanged = false;
  }
}//Update
