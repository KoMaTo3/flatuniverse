#ifndef __LIGHTBLOCK_H__
#define __LIGHTBLOCK_H__


#include "../objectwidget.h"


namespace ObjectWidget {

class WidgetLightBlock: public Widget {
public:
  WidgetLightBlock( WidgetOwner *setOwner );
  virtual ~WidgetLightBlock();
  virtual void OnEvent( const ObjectWidgetEvent &event );
  virtual void SaveToBuffer( MemoryWriter &writer );
  virtual void LoadFromBuffer( MemoryReader &reader );

private:
  WidgetLightBlock();
  WidgetLightBlock( const WidgetLightBlock& );
  WidgetLightBlock& operator=( const WidgetLightBlock& );
};


};


#endif
