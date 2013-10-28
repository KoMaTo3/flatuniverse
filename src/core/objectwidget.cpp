#include "objectwidget.h"
#include "file.h"
#include "tools.h"


using namespace ObjectWidget;


WidgetMatrix *Widget::_widgetList = NULL;


void ObjectWidget::Init() {
  __log.PrintInfo( Filelevel_DEBUG, "ObjectWidget::Init..." );
  Widget::_widgetList = new WidgetMatrix;
  Widget::_widgetList->resize( OBJECT_WIDGET_COUNT__ );
  for( auto &list: *Widget::_widgetList ) {
    list = new WidgetList;
  }
  __log.PrintInfo( Filelevel_DEBUG, "ObjectWidget::Init ok" );
}//Init


void ObjectWidget::Destroy() {
  for( auto &list: *Widget::_widgetList ) {
    delete list;
  }
  delete Widget::_widgetList;
  Widget::_widgetList = NULL;
}//Destroy


Widget::Widget( const ObjectWidgetGUID setGuid, const WidgetOwner *setOwner )
:guid( setGuid ), owner( setOwner ) {
  this->_widgetList->at( this->guid )->push_back( this );
  __log.PrintInfo( Filelevel_DEBUG, "Widget +1: type[%d] this[x%p]", setGuid, this );
}


Widget::~Widget() {
  auto &list = this->_widgetList->at( this->guid );
  for( auto &widget: *list ) {
    if( widget == this ) {
      delete widget;
      break;
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "Widget -1: x%X", this->guid );
}


WidgetMgr::WidgetMgr( const WidgetOwner *setOwner )
:owner( setOwner ) {
  this->_widgetList.resize( OBJECT_WIDGET_COUNT__ );
  for( auto &widget: this->_widgetList ) {
    widget = NULL;
  }
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr +1" );
}


WidgetMgr::~WidgetMgr() {
  for( auto &widgetInstance: this->_widgetList ) {
    if( widgetInstance ) {
      delete widgetInstance;
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr -1" );
}


bool WidgetMgr::AddWidget( Widget *newWidget ) {
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::AddWidget x%p", newWidget );
  if( !newWidget ) {
    __log.PrintInfo( Filelevel_ERROR, "WidgetMgr::AddWidget => is NULL" );
    return NULL;
  }
  if( this->_widgetList[ newWidget->guid ] ) {
    __log.PrintInfo( Filelevel_WARNING, "WidgetMgr::AddWidget => widget x%X already exists", newWidget->guid );
    return false;
  }
  this->_widgetList[ newWidget->guid ] = newWidget;
  return true;
}


WidgetLightBlock::WidgetLightBlock( const WidgetOwner *setOwner )
:Widget( ObjectWidgetGUID::OBJECT_WIDGET_LIGHTBLOCK, setOwner ) {
  __log.PrintInfo( Filelevel_DEBUG, "WidgetLightBlock +1 x%p", this );
}


WidgetLightBlock::~WidgetLightBlock() {
  __log.PrintInfo( Filelevel_DEBUG, "WidgetLightBlock -1 x%p", this );
}


WidgetOwner::WidgetOwner() {
  __log.PrintInfo( Filelevel_DEBUG, "WidgetOwner +1 x%p", this );
  this->widget = new ObjectWidget::WidgetMgr( this );
}

WidgetOwner::~WidgetOwner() {
  DEF_DELETE( this->widget );
  __log.PrintInfo( Filelevel_DEBUG, "WidgetOwner -1 x%p", this );
}
