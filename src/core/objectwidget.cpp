#include "objectwidget.h"
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


bool WidgetMgr::DeleteWidget( const ObjectWidgetGUID &guid ) {
  if( this->_widgetList[ guid ] ) {
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


void WidgetMgr::TouchEvent( const ObjectWidgetEvent &event ) {
  for( auto &widget: *this->_eventListenersList->at( event ) ) {
    widget->OnEvent( event );
  }
}//TouchEvent


WidgetOwner::WidgetOwner() {
  this->widget = new ObjectWidget::WidgetMgr( this );
}

WidgetOwner::~WidgetOwner() {
  DEF_DELETE( this->widget );
}

void WidgetOwner::ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget ) {
  this->widget->ListenEvent( event, newWidget );
}//ListenEvent


WidgetLightBlock::WidgetLightBlock( WidgetOwner *setOwner )
:Widget( ObjectWidgetGUID::OBJECT_WIDGET_LIGHTBLOCK, setOwner ) {
}


WidgetLightBlock::~WidgetLightBlock() {
}


void WidgetLightBlock::OnEvent( const ObjectWidgetEvent &event ) {
}



WidgetLightBlockByCollision::WidgetLightBlockByCollision( WidgetOwner *setOwner, LightRenderer *setLightRenderer, CollisionElement *setCollision )
:Widget( ObjectWidgetGUID::OBJECT_WIDGET_LIGHTBLOCKBYCOLLISION, setOwner ), _collisionElement( setCollision ), _lightRenderer( setLightRenderer ) {
  if( !setCollision ) {
    __log.PrintInfo( Filelevel_ERROR, "WidgetLightBlockByCollision => collision is NULL" );
  }
  this->_lightRenderer->GetLightManager()->lightBlocks.push_back( setCollision );
  this->owner->ListenEvent( OBJECT_WIDGET_EVENT_UPDATE, this );
}


WidgetLightBlockByCollision::~WidgetLightBlockByCollision() {
  if( this->_collisionElement ) {
    LightMap::LightBlocksList &blocksList = this->_lightRenderer->GetLightManager()->lightBlocks;
    auto
      iter = blocksList.begin(),
      iterEnd = blocksList.end();
    while( iter != iterEnd ) {
      if( *iter == this->_collisionElement ) {
        blocksList.erase( iter );
        break;
      }
      ++iter;
    }
  }
}



void WidgetLightBlockByCollision::OnEvent( const ObjectWidgetEvent &event ) {
  switch( event ) {
  case OBJECT_WIDGET_EVENT_UPDATE:
    this->_collisionElement->Update();
  break;
  }//switch
}
