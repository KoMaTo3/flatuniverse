#ifndef __OBJECTWIDGET_H__
#define __OBJECTWIDGET_H__


#include <hash_map>
#include "ktypes.h"


class ObjectWidget;
enum ObjectWidgetGUID {
  OBJECT_WIDGET_RENDERABLE  = 1,
  OBJECT_WIDGET_COLLISION   = 2,
  OBJECT_WIDGET_TRIGGER     = 3,
  OBJECT_WIDGET_LIGHT       = 4,
  OBJECT_WIDGET_LIGHTBLOCK  = 5,
};
typedef std::hash_map< ObjectWidgetGUID, ObjectWidget* > ObjectWidgetList;


class ObjectWidget
{
public:
  ObjectWidget();
  virtual ~ObjectWidget();
};


class ObjectWidgetLightBlock {
};


#endif
