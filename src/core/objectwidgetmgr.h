#ifndef __OBJECTWIDGETMGR_H__
#define __OBJECTWIDGETMGR_H__


#include <vector>
#include "objectwidget.h"


namespace ObjectWidget {


class WidgetOwner;


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


};

#endif
