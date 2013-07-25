#include "collisionmgr.h"
#include "tools.h"
#include "file.h"

CollisionList *__collisionList = NULL;



CollisionManager::CollisionManager()
:time( 0.0f )
{
  if( !__collisionList )
    __collisionList = new CollisionList();
}//constructor



CollisionManager::~CollisionManager()
{
  DEF_DELETE( __collisionList );
}//destructor



/*
=============
  Update
=============
*/
bool CollisionManager::Update( float dt )
{
  if( !__collisionList )
    return false;

  CollisionList::iterator iter0, iter1, iterEnd = __collisionList->end();

  this->time += dt;
  bool forceUpdate = ( dt == 0.0f );
  float updateInterval = ( forceUpdate ? 0.0f : COLLISION_UPDATE_FREQUENCY );

  while( this->time > COLLISION_UPDATE_FREQUENCY || forceUpdate )
  {
    //Обновляем позиции, сбрасываем решения коллизий
    for( iter0 = __collisionList->begin(); iter0 != iterEnd; ++iter0 )
      ( *iter0 )->Update( updateInterval );

    //проверяем расстояния и коллизии
    //не проверяем статика+статика или статика+динамика, т.к. при дальнейшем проходе эта динамика будет проверяться со всей статикой

    //оптимизация: динамика проверяется со всеми статиками, и с той частью динамики, что идёт после себя в списке __collisionList
    for( int q = 0; q < 2; ++q )
    {
      bool allowTestWidthDynamic;
      for( iter0 = __collisionList->begin(); iter0 != iterEnd; ++iter0 )
      {
        if( ( *iter0 )->IsStatic() )
          continue;
        allowTestWidthDynamic = false;
        for( iter1 = __collisionList->begin(); iter1 != iterEnd; ++iter1 )
        {
          //iter0 тут всегда динамика
          //iter1 тут что угодно
          if( iter0 == iter1 )  //сам с собой не пересекается
          {
            allowTestWidthDynamic = true; //разрешаем проверять всю динамику, что дальше по списку
            continue;
          }

          if( !( *iter1 )->IsStatic() && !allowTestWidthDynamic ) //пропускаем динамику, которая предшествует iter0
            continue;

          //Проверка дистанции (расстояние между центрами должно быть меньше, чем суммы радиусов)
          if( this->CheckDistance( **iter0, **iter1 ) )
          {
            //__log.PrintInfo( Filelevel_DEBUG, "test" );
            if( ( *iter0 )->TestIntersect( **iter1 ) )
            {
              //__log.PrintInfo( Filelevel_DEBUG, "intersect" );
            }
          }//CheckDistance
        }
      }

      //по полученным решениям делаем вывод, как правильнее смещать объект
      for( iter0 = __collisionList->begin(); iter0 != iterEnd; ++iter0 )
        if( !( *iter0 )->IsStatic() )
          ( *iter0 )->ResolveCollision();
    }

    this->time -= updateInterval;
    if( forceUpdate ) {
      break;
    }
  }//while

  return true;
}//Update



/*
=============
  CheckDistance
  Проверка расстояний между Collision
=============
*/
bool CollisionManager::CheckDistance( const Collision &item0, const Collision &item1 )
{
  Vec3 distanceVector = item1.GetPosition() - item0.GetPosition();
  float distance = distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y;

  //__log.PrintInfo( Filelevel_DEBUG, "vec[%3.3f; %3.3f] => %3.3f ~ %3.3f", distanceVector.x, distanceVector.y, distance, item0.GetRadius2() + item1.GetRadius2() );

  return distance < item0.GetRadius2() + item1.GetRadius2();
}//CheckDistance
