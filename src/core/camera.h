#pragma once

namespace Engine {

class ICamera {
public:
  ICamera();
  virtual ~ICamera();

  virtual void Update() = 0;
};

};
