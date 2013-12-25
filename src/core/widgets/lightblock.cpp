#include "lightblock.h"


using namespace ObjectWidget;


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
