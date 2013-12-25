#ifndef __OBJECTWIDGETOWNER_H__
#define __OBJECTWIDGETOWNER_H__


#include "objectwidget.h"


namespace ObjectWidget {


class Widget;
class WidgetMgr;


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


};


#endif
