#ifndef __OBJECTWIDGET_H__
#define __OBJECTWIDGET_H__


#include <vector>
#include <deque>
#include "ktypes.h"


class Collision;
class LightRenderer;


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
  OBJECT_WIDGET_COUNT__,
};


typedef std::deque< Widget* > WidgetList;
typedef std::vector< WidgetList* > WidgetMatrix;


class WidgetOwner {
public:
  WidgetOwner();
  virtual ~WidgetOwner();

protected:
  WidgetMgr *widget;

private:
  WidgetOwner( const WidgetOwner& );
  WidgetOwner& operator=( const WidgetOwner& );
};


class Widget
{
public:
  Widget( const ObjectWidgetGUID setGuid, const WidgetOwner *setOwner );
  virtual ~Widget();

  const ObjectWidgetGUID guid;
  static WidgetMatrix* _widgetList;

private:
  const WidgetOwner *owner;

  Widget();
  Widget( const Widget& );
  Widget& operator=( const Widget& );
};


class WidgetMgr {
public:
  typedef std::vector< Widget* > ObjectWidgetList;

  WidgetMgr( const WidgetOwner *setOwner );
  virtual ~WidgetMgr();
  bool AddWidget( Widget *newWidget );

private:
  Widget* AddWidgetLightBlock();

  WidgetMgr();
  ObjectWidgetList _widgetList;
  const WidgetOwner *owner;
};


class WidgetLightBlock: public Widget {
public:
  WidgetLightBlock( const WidgetOwner *setOwner );
  virtual ~WidgetLightBlock();

private:
  WidgetLightBlock();
  WidgetLightBlock( const WidgetLightBlock& );
  WidgetLightBlock& operator=( const WidgetLightBlock& );
};


class WidgetLightBlockByCollision: public Widget {
public:
  WidgetLightBlockByCollision( const WidgetOwner *setOwner, LightRenderer *setLightRenderer, Collision *setCollision );
  virtual ~WidgetLightBlockByCollision();

private:
  WidgetLightBlockByCollision();
  WidgetLightBlockByCollision( const WidgetLightBlockByCollision& );
  WidgetLightBlockByCollision& operator=( const WidgetLightBlockByCollision& );

  Collision *_collision;
  LightRenderer *_lightRenderer;
};


}


#endif
