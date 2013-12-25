#include "lightblockbycollision.h"
#include "../objectwidgetowner.h"
#include "../lightrenderer.h"
#include "../collisionelement.h"


using namespace ObjectWidget;


WidgetLightBlockByCollision::WidgetLightBlockByCollision( WidgetOwner *setOwner, LightMap::LightRenderer *setLightRenderer, CollisionElement *setCollision )
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
