#ifndef __OBJECTWIDGET_H__
#define __OBJECTWIDGET_H__


#include <vector>
#include <deque>
#include "ktypes.h"
#include "kvector.h"
#include "lightmgr.h"
#include "memorywriter.h"
#include "memoryreader.h"


class CollisionElement;

namespace LightMap {
  class LightRenderer;
};


namespace ObjectWidget {


void Init();
void Destroy();


class Widget;
class WidgetMgr;
enum ObjectWidgetGUID {
  OBJECT_WIDGET_RENDERABLE  = 1,
  OBJECT_WIDGET_COLLISION   = 2,
  OBJECT_WIDGET_TRIGGER     = 3,
  OBJECT_WIDGET_LIGHT       = 4,
  OBJECT_WIDGET_LIGHTBLOCK  = 5,
  OBJECT_WIDGET_LIGHTBLOCKBYCOLLISION  = 6,
  OBJECT_WIDGET_LIGHTPOINT  = 7,
  OBJECT_WIDGET_COUNT__,
};
enum ObjectWidgetEvent {
  OBJECT_WIDGET_EVENT_UPDATE = 0,
  OBJECT_WIDGET_EVENT_COUNT__,
};


typedef std::deque< Widget* > WidgetList;
typedef std::vector< WidgetList* > WidgetMatrix;


class WidgetOwner {
public:
  WidgetOwner();
  virtual ~WidgetOwner();
  void ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget );
  inline WidgetMgr* GetWidgetManager() { return this->widget; }

protected:
  WidgetMgr *widget;

private:
  WidgetOwner( const WidgetOwner& );
  WidgetOwner& operator=( const WidgetOwner& );
};


class Widget
{
public:
  Widget( const ObjectWidgetGUID setGuid, WidgetOwner *setOwner );
  virtual ~Widget();
  virtual void OnEvent( const ObjectWidgetEvent &event ) = 0;
  virtual void SaveToBuffer( MemoryWriter &writer ) = 0;
  virtual void LoadFromBuffer( MemoryReader &reader ) = 0;

  const ObjectWidgetGUID guid;
  static WidgetMatrix* _widgetList;

protected:
  WidgetOwner *owner;

private:
  Widget();
  Widget( const Widget& );
  Widget& operator=( const Widget& );
};


class WidgetMgr {
public:
  typedef std::vector< Widget* > ObjectWidgetList;
  typedef std::vector< ObjectWidgetList* > ObjectWidgetEventListenersList;

  WidgetMgr( const WidgetOwner *setOwner );
  virtual ~WidgetMgr();
  bool AddWidget( Widget *newWidget );
  bool DeleteWidget( const ObjectWidgetGUID &guid );
  bool WidgetExists( const ObjectWidgetGUID &guid );
  void ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget );
  void RemoveAllWidgetListeners( Widget *widget );
  void TouchEvent( const ObjectWidgetEvent &event );
  Widget *GetWidget( const ObjectWidgetGUID &guid );

private:
  Widget* AddWidgetLightBlock();

  WidgetMgr();
  ObjectWidgetList _widgetList;
  const WidgetOwner *owner;
  ObjectWidgetEventListenersList *_eventListenersList;
};


class WidgetLightBlock: public Widget {
public:
  WidgetLightBlock( WidgetOwner *setOwner );
  virtual ~WidgetLightBlock();
  virtual void OnEvent( const ObjectWidgetEvent &event );
  virtual void SaveToBuffer( MemoryWriter &writer );
  virtual void LoadFromBuffer( MemoryReader &reader );

private:
  WidgetLightBlock();
  WidgetLightBlock( const WidgetLightBlock& );
  WidgetLightBlock& operator=( const WidgetLightBlock& );
};


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


}


#endif
