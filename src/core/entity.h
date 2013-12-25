#ifndef __ENTITY_H__
#define __ENTITY_H__


#include "kvector.h"


class Entity {
public:
  Entity();
  Entity( const Vec3& setPosition );
  virtual ~Entity();

protected:
  Vec3  position;

private:
  Entity( const Entity& );
  Entity& operator=( const Entity& );
};


#endif
