#pragma once

#include "kvector.h"
#include "kmatrix.h"
#include "kquat.h"
#include "camera.h"

namespace Engine {

class Camera3d: public ICamera {
public:
  Camera3d();
  virtual ~Camera3d();
  Camera3d* SetPosition( const Vec3& setPosition );
  Camera3d* SetRotation( const float yaw, const float pitch, const float roll );
  inline const Vec3& GetPosition() const { return this->position; }
  Camera3d* SetFOV( const float radians );
  virtual void Update();
  inline float* GetProjectionMatrixPointer() { return &this->matrixProjection[ 0 ][ 0 ]; }
  inline float* GetWorldMatrixPointer() { return &this->matrixWorld[ 0 ][ 0 ]; }

private:
  Camera3d( Camera3d& );
  Camera3d& operator=( Camera3d& );

  float
    planeNear,
    planeFar;
  float FOV;
  float aspect;
  Vec3 position;
  Quat rotation;
  bool matrixChanged;
  Mat4
    matrixProjection,
    matrixWorld;
};

}
