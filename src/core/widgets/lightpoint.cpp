#include "lightpoint.h"
#include "../objectwidgetowner.h"
#include "../objectwidgetmgr.h"


using namespace ObjectWidget;


WidgetLightPoint::WidgetLightPoint( WidgetOwner *setOwner, LightMap::LightsListPtr *setLightList, const Vec3 *setPosition, const Vec4 &setColor, const Vec2 &setSize, const float setBrightness, const int bufferSize )
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


void WidgetLightPoint::SetPenetration( const float setPenetration ) {
  this->_lightEntity->SetPenetration( setPenetration );
}//SetPenetration


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
