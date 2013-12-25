#ifndef __LIGHTBLOCKBYCOLLISION_H__
#define __LIGHTBLOCKBYCOLLISION_H__


#include "../objectwidget.h"


namespace ObjectWidget {


class WidgetLightBlockByCollision: public Widget {
public:
  WidgetLightBlockByCollision( WidgetOwner *setOwner, LightMap::LightRenderer *setLightRenderer, CollisionElement *setCollision );
  virtual ~WidgetLightBlockByCollision();
  virtual void OnEvent( const ObjectWidgetEvent &event );
  virtual void SaveToBuffer( MemoryWriter &writer );
  virtual void LoadFromBuffer( MemoryReader &reader );

private:
  WidgetLightBlockByCollision();
  WidgetLightBlockByCollision( const WidgetLightBlockByCollision& );
  WidgetLightBlockByCollision& operator=( const WidgetLightBlockByCollision& );

  CollisionElement *_collisionElement;
  LightMap::LightRenderer *_lightRenderer;
};


};


#endif
