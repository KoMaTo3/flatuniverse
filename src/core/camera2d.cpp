#include "camera2d.h"
#include "file.h"

using namespace Engine;

Camera2d::Camera2d()
:position( Vec3Null ), scale( Vec2One ), rotation( 0.0f ), matrixChanged( true ) {
}

Camera2d::~Camera2d() {
}


Camera2d* Camera2d::SetPosition( const Vec3& setPosition ) {
  if( this->position.x != setPosition.x || this->position.y != setPosition.y || this->position.z != setPosition.z ) {
    this->matrixChanged = true;
  }
  this->position = setPosition;
  return this;
}//SetPosition

Camera2d* Camera2d::SetScale( const Vec2& setScale ) {
  if( this->scale.x != setScale.x || this->scale.y != setScale.y ) {
    this->matrixChanged = true;
  }
  this->scale = setScale;
  return this;
}//SetScale

Camera2d* Camera2d::SetRotation( const float setRotation ) {
  if( this->rotation != setRotation ) {
    this->matrixChanged = true;
  }
  this->rotation = setRotation;
  return this;
}//SetRotation

void Camera2d::Update() {
  if( this->matrixChanged ) {
    Mat4 matScale, matTranslate, matRotation;
    matScale.Identity();
    matTranslate.Identity();
    matRotation.Identity();

    matTranslate[ 0 ][ 3 ] = this->position.x;
    matTranslate[ 1 ][ 3 ] = this->position.y;

    float cosa = Math::Cos16( this->rotation );
    float sina = Math::Sin16( this->rotation );
    matRotation[ 0 ][ 0 ] = cosa;
    matRotation[ 1 ][ 0 ] = -sina;
    matRotation[ 0 ][ 1 ] = sina;
    matRotation[ 1 ][ 1 ] = cosa;

    matScale[ 0 ][ 0 ] = this->scale.x;
    matScale[ 1 ][ 1 ] = this->scale.y;

    this->matrix = matScale * matRotation * matTranslate;

    this->matrixChanged = false;
  }
}//Update
