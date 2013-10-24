#ifndef __OBJECTWIDGET_H__
#define __OBJECTWIDGET_H__


#include <vector>
#include <deque>
#include "ktypes.h"


namespace ObjectWidget {


void Init();
void Destroy();


class Widget;
enum ObjectWidgetGUID {
  OBJECT_WIDGET_RENDERABLE  = 1,
  OBJECT_WIDGET_COLLISION   = 2,
  OBJECT_WIDGET_TRIGGER     = 3,
  OBJECT_WIDGET_LIGHT       = 4,
  OBJECT_WIDGET_LIGHTBLOCK  = 5,
  OBJECT_WIDGET_COUNT__,
};


typedef std::deque< Widget* > WidgetList;
typedef std::vector< WidgetList* > WidgetMatrix;


class WidgetOwner {
public:
  WidgetOwner();
  virtual ~WidgetOwner();

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
  typedef Widget* (WidgetMgr::WidgetInitializator)();
  //typedef std::vector< WidgetInitializator* > WidgetInitializatorList;

  WidgetMgr( const WidgetOwner *setOwner );
  virtual ~WidgetMgr();
  Widget* AddWidget( const ObjectWidgetGUID guid );

private:
  //WidgetInitializatorList _widgetInitializator;

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


}


#endif
