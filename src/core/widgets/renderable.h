#ifndef __OBJECTWIDGETRENDERABLE_H__
#define __OBJECTWIDGETRENDERABLE_H__


#include "../objectwidget.h"
#include "../renderable.h"


class Entity;


namespace ObjectWidget {
  

class WidgetRenderable: public Widget {
public:
  WidgetRenderable( WidgetOwner *setOwner, const Entity *setEntity, const Vec3 &position );
  virtual ~WidgetRenderable();
  virtual void OnEvent( const ObjectWidgetEvent &event );
  virtual void SaveToBuffer( MemoryWriter &writer );
  virtual void LoadFromBuffer( MemoryReader &reader );
  void SetSize( const Vec2& setSize );
  void SetColor( const Vec4& setColor );
  void SetPenetration( const float setPenetration);

private:
  WidgetRenderable();
  WidgetRenderable( const WidgetRenderable& );
  WidgetRenderable& operator=( const WidgetRenderable& );

  LightMap::LightEntity *_lightEntity;
  LightMap::LightsListPtr *_lightList;
  const Vec3 *_position;
};


};


#endif
