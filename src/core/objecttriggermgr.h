#pragma once

#include "objecttrigger.h"
#include <vector>

class ObjectTrigger;
typedef std::vector< ObjectTrigger* > ObjectTriggerList;
extern ObjectTriggerList *__triggerList;

class ObjectTriggerManager
{
private:
public:
  ObjectTriggerManager();
  virtual ~ObjectTriggerManager();

  void  Update();
};
