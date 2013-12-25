#include "objectwidget.h"
#include "objectwidgetowner.h"
#include "objectwidgetmgr.h"
#include "file.h"
#include "tools.h"
#include "collision.h"
#include "lightrenderer.h"


using namespace ObjectWidget;


WidgetMatrix *Widget::_widgetList = NULL;


void ObjectWidget::Init() {
  Widget::_widgetList = new WidgetMatrix;
  Widget::_widgetList->resize( OBJECT_WIDGET_COUNT__ );
  for( auto &list: *Widget::_widgetList ) {
    list = new WidgetList;
  }
}//Init


void ObjectWidget::Destroy() {
  for( auto &list: *Widget::_widgetList ) {
    delete list;
  }
  delete Widget::_widgetList;
  Widget::_widgetList = NULL;
}//Destroy


Widget::Widget( const ObjectWidgetGUID setGuid, WidgetOwner *setOwner )
:guid( setGuid ), owner( setOwner ) {
  this->_widgetList->at( this->guid )->push_back( this );
}


Widget::~Widget() {
  auto &list = this->_widgetList->at( this->guid );
  WidgetList::iterator
    iter = list->begin(),
    iterEnd = list->end();
  for( ; iter != iterEnd; ++iter ) {
    if( *iter == this ) {
      list->erase( iter );
      break;
    }
  }
}


WidgetOwner::WidgetOwner() {
  this->widget = new ObjectWidget::WidgetMgr( this );
}

WidgetOwner::~WidgetOwner() {
  DEF_DELETE( this->widget );
  this->widget = NULL;
}

void WidgetOwner::ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget ) {
  this->widget->ListenEvent( event, newWidget );
}//ListenEvent
