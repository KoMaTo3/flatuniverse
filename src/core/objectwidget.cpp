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
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::DeleteWidget ..." );
  if( this->_widgetList[ guid ] ) {
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::DeleteWidget => delete widget %p...", this->_widgetList[ guid ] );
    delete this->_widgetList[ guid ];
    this->_widgetList[ guid ] = NULL;
    return true;
  }
  __log.PrintInfo( Filelevel_DEBUG, "WidgetMgr::DeleteWidget => widget not found" );
  return false;
}//DeleteWidget


bool WidgetMgr::WidgetExists( const ObjectWidgetGUID &guid ) {
  return this->_widgetList[ guid ] != NULL;
}//WidgetExists


void WidgetMgr::ListenEvent( const ObjectWidgetEvent &event, Widget *newWidget ) {
  this->_eventListenersList->at( event )->push_back( newWidget );
}//ListenEvent


void WidgetMgr::RemoveAllWidgetListeners( Widget *widget ) {
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
          removed = true;
          break;
        }
        ++iter;
      }
    } while( removed );
  }
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
  this->widget = NULL;
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


void WidgetLightBlock::SaveToBuffer( MemoryWriter &writer ) {
}//SaveToBuffer


void WidgetLightBlock::LoadFromBuffer( MemoryReader &reader ) {
}//SaveToBuffer



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
  this->_lightRenderer->GetLightManager()->ClearCacheByBlock( this->_collisionElement );
}



void WidgetLightBlockByCollision::OnEvent( const ObjectWidgetEvent &event ) {
  switch( event ) {
  case OBJECT_WIDGET_EVENT_UPDATE:
    this->_collisionElement->Update();
  break;
  }//switch
}


void WidgetLightBlockByCollision::SaveToBuffer( MemoryWriter &writer ) {
}//SaveToBuffer


void WidgetLightBlockByCollision::LoadFromBuffer( MemoryReader &reader ) {
}//SaveToBuffer



WidgetLightPoint::WidgetLightPoint( WidgetOwner *setOwner, LightsListPtr *setLightList, const Vec3 *setPosition, const Vec4 &setColor, const Vec2 &setSize, const float setBrightness, const int bufferSize )
:Widget( ObjectWidgetGUID::OBJECT_WIDGET_LIGHTPOINT, setOwner ), _lightList( setLightList ), _position( setPosition ) {
  this->_lightEntity = new LightMap::LightEntity( LightMap::LT_POINT, Vec2( setPosition->x, setPosition->y ), setColor, setSize, setBrightness, bufferSize );
  ( *setLightList )->push_back( this->_lightEntity );
  this->owner->ListenEvent( OBJECT_WIDGET_EVENT_UPDATE, this );
}


WidgetLightPoint::~WidgetLightPoint() {
  auto
    iter = ( *this->_lightList )->begin(),
    iterEnd = ( *this->_lightList )->end();
  for(; iter != iterEnd; ++iter ) {
    if( *iter == this->_lightEntity ) {
      ( *this->_lightList )->erase( iter );
      break;
    }
  }
  delete this->_lightEntity;
  this->owner->GetWidgetManager()->RemoveAllWidgetListeners( this );
}


void WidgetLightPoint::OnEvent( const ObjectWidgetEvent &event ) {
  switch( event ) {
  case OBJECT_WIDGET_EVENT_UPDATE:
    this->_lightEntity->SetPosition( Vec2( this->_position->x, -this->_position->y ) );
  break;
  }//switch
}//OnEvent


void WidgetLightPoint::SetSize( const Vec2& setSize ) {
  this->_lightEntity->SetSize( setSize );
}//SetSize

void WidgetLightPoint::SetColor( const Vec4& setColor ) {
  this->_lightEntity->SetColor( setColor );
}//SetColor


void WidgetLightPoint::SaveToBuffer( MemoryWriter &writer ) {
  writer << this->_lightEntity->GetSize();
  writer << this->_lightEntity->GetColor();
}//SaveToBuffer


void WidgetLightPoint::LoadFromBuffer( MemoryReader &reader ) {
  Vec2 size;
  Vec4 color;
  reader >> size;
  reader >> color;
  this->_lightEntity->SetSize( size );
  this->_lightEntity->SetColor( color );
}//SaveToBuffer
