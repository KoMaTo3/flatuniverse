#include "objectwidget.h"


using namespace ObjectWidget;


WidgetMatrix *Widget::_widgetList = NULL;


void ObjectWidget::Init() {
  Widget::_widgetList = new WidgetMatrix;
  Widget::_widgetList->reserve( OBJECT_WIDGET_COUNT__ );
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
  for( auto &widget: *list ) {
    if( widget == this ) {
      delete widget;
      break;
    }
  }
}


WidgetMgr::WidgetMgr( const WidgetOwner *setOwner )
:owner( setOwner ) {
  //WidgetMgr::_widgetInitializator[ OBJECT_WIDGET_LIGHTBLOCK ] = &this->AddWidgetLightBlock;
}


WidgetMgr::~WidgetMgr() {
}


Widget* WidgetMgr::AddWidget( const ObjectWidgetGUID guid ) {
  if( this->_widgetList[ guid ] ) {
    return this->_widgetList[ guid ];
  }
}


WidgetLightBlock::WidgetLightBlock( const WidgetOwner *setOwner )
:Widget( ObjectWidgetGUID::OBJECT_WIDGET_LIGHTBLOCK, setOwner ) {
}


WidgetLightBlock::~WidgetLightBlock() {
}


WidgetOwner::WidgetOwner() {
}

WidgetOwner::~WidgetOwner() {
}
