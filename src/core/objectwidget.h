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
class WidgetOwner;
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


};


#endif
