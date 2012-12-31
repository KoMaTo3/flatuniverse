#include "objecttriggermgr.h"
#include "collisionmgr.h"


ObjectTriggerList *__triggerList = NULL;



ObjectTriggerManager::ObjectTriggerManager()
{
}//constructor


ObjectTriggerManager::~ObjectTriggerManager()
{
}//destructor




/*
=============
  Update
=============
*/
void ObjectTriggerManager::Update()
{
  //пересчитываем координаты триггера
  //проверяем все collision-объекты
  if( __collisionList && __triggerList )
  {
    ObjectTriggerList::iterator triggerIter, triggerIterEnd = __triggerList->end();
    CollisionList::iterator objectIter, objectIterEnd = __collisionList->end();
    for( triggerIter = __triggerList->begin(); triggerIter != triggerIterEnd; ++triggerIter )
    {
      ( *triggerIter )->Update();

      for( objectIter = __collisionList->begin(); objectIter != objectIterEnd; ++objectIter )
        if( !( *objectIter )->IsStatic() )
          ( *triggerIter )->CheckCollision( *objectIter );
    }
  }
}//Update

