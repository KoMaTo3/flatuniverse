#pragma once

#include "kvector.h"
#include "kmatrix.h"
#include "camera.h"

namespace Engine {

class Camera2d: public ICamera {
public:
  Camera2d();
  virtual ~Camera2d();
  Camera2d* SetPosition( const Vec3& setPosition );
  Camera2d* SetScale( const Vec2& setScale );
  Camera2d* SetRotation( const float setRotation );
  inline const Vec3& GetPosition() const { return this->position; }
  inline const Vec2& GetScale() const { return this->scale; }
  inline const float GetRotation() const { return this->rotation; }
  virtual void Update();
  inline float* GetMatrixPointer() { return &this->matrix[ 0 ][ 0 ]; }
  inline const Mat4& GetMatrix() { return this->matrix; }

private:
  Camera2d( Camera2d& );
  Camera2d& operator=( Camera2d& );

  Vec3 position;
  Vec2 scale;
  float rotation;
  bool matrixChanged;
  Mat4 matrix;
};

}
