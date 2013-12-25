#include "objectwidgetmgr.h"


using namespace ObjectWidget;


WidgetMgr::WidgetMgr( const WidgetOwner *setOwner )
:owner( setOwner ), _eventListenersList( new ObjectWidgetEventListenersList ) {
  this->_widgetList.resize( OBJECT_WIDGET_COUNT__ );
  for( auto &widget: this->_widgetList ) {
    widget = NULL;
  }
  this->_eventListenersList->resize( OBJECT_WIDGET_EVENT_COUNT__ );
  for( int num = 0; num < OBJECT_WIDGET_EVENT_COUNT__; ++num ) {
    this->_eventListenersList->at( num ) = new ObjectWidgetList();
  }
}


WidgetMgr::~WidgetMgr() {
  for( auto &widgetInstance: this->_widgetList ) {
    if( widgetInstance ) {
      delete widgetInstance;
    }
  }
  this->_widgetList.clear();
  for( int num = 0; num < OBJECT_WIDGET_EVENT_COUNT__; ++num ) {
    this->_eventListenersList->at( num )->clear();
    delete this->_eventListenersList->at( num );
  }
  this->_eventListenersList->clear();
  delete this->_eventListenersList;
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


Widget* WidgetMgr::GetWidget( const ObjectWidgetGUID &guid ) {
  return this->_widgetList[ guid ];
}//GetWidget


bool WidgetMgr::DeleteWidget( const ObjectWidgetGUID &guid ) {
  if( this->_widgetList[ guid ] ) {
    //__log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::DeleteWidget => delete widget %p...", this->_widgetList[ guid ] );
    this->RemoveAllWidgetListeners( this->_widgetList[ guid ] );
    delete this->_widgetList[ guid ];
    this->_widgetList[ guid ] = NULL;
    return true;
  }
  return false;
}//DeleteWidget


bool WidgetMgr::WidgetExists( const ObjectWidgetGUID &guid ) {
  return this->_widgetList[ guid ] != NULL;
}//WidgetExists


void WidgetMgr::ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget ) {
  this->_eventListenersList->at( event )->push_back( newWidget );
}//ListenEvent


void WidgetMgr::RemoveAllWidgetListeners( Widget *widget ) {
  //__log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::RemoveAllWidgetListeners => widget[%p]", widget );
  for( auto &widgetList: *this->_eventListenersList ) {
    bool removed;
    do {
      removed = false;
      auto
        iter = widgetList->begin(),
        iterEnd = widgetList->end();
      while( iter != iterEnd ) {
        if( *iter == widget ) {
          widgetList->erase( iter );
          //__log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::RemoveAllWidgetListeners => removed" );
          removed = true;
          break;
        }
        ++iter;
      }
    } while( removed );
  }
  //__log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::RemoveAllWidgetListeners => widget[%p] done", widget );
}//ListenEvent


void WidgetMgr::TouchEvent( const ObjectWidgetEvent &event ) {
  for( auto &widget: *this->_eventListenersList->at( event ) ) {
    widget->OnEvent( event );
  }
}//TouchEvent
