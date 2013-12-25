#ifndef __LIGHTPOINT_H__
#define __LIGHTPOINT_H__


#include "../objectwidget.h"


namespace ObjectWidget {
  

class WidgetLightPoint: public Widget {
public:
  WidgetLightPoint( WidgetOwner *setOwner, LightMap::LightsListPtr *setLightList, const Vec3 *setPosition, const Vec4 &setColor, const Vec2 &setSize, const float setBrightness, const int bufferSize = 1024 );
  virtual ~WidgetLightPoint();
  virtual void OnEvent( const ObjectWidgetEvent &event );
  virtual void SaveToBuffer( MemoryWriter &writer );
  virtual void LoadFromBuffer( MemoryReader &reader );
  void SetSize( const Vec2& setSize );
  void SetColor( const Vec4& setColor );
  void SetPenetration( const float setPenetration);

private:
  WidgetLightPoint();
  WidgetLightPoint( const WidgetLightPoint& );
  WidgetLightPoint& operator=( const WidgetLightPoint& );

  LightMap::LightEntity *_lightEntity;
  LightMap::LightsListPtr *_lightList;
  const Vec3 *_position;
};


};


#endif
