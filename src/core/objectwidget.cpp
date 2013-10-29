#include "objectwidget.h"
#include "file.h"
#include "tools.h"


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


Widget::Widget( const ObjectWidgetGUID setGuid, const WidgetOwner *setOwner )
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


WidgetMgr::WidgetMgr( const WidgetOwner *setOwner )
:owner( setOwner ) {
  this->_widgetList.resize( OBJECT_WIDGET_COUNT__ );
  for( auto &widget: this->_widgetList ) {
    widget = NULL;
  }
}


WidgetMgr::~WidgetMgr() {
  for( auto &widgetInstance: this->_widgetList ) {
    if( widgetInstance ) {
      delete widgetInstance;
    }
  }
}


bool WidgetMgr::AddWidget( Widget *newWidget ) {
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
}


WidgetLightBlock::~WidgetLightBlock() {
}


WidgetOwner::WidgetOwner() {
  this->widget = new ObjectWidget::WidgetMgr( this );
}

WidgetOwner::~WidgetOwner() {
  DEF_DELETE( this->widget );
}
