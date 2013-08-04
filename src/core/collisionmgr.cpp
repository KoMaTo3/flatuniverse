#include "collisionmgr.h"
#include "tools.h"
#include "file.h"

CollisionList *__collisionList = NULL;
CollisionPairList __collisionPairList;


CollisionManager::CollisionManager()
:time( 0.0f )
{
  if( !__collisionList )
    __collisionList = new CollisionList();
  __collisionPairList.reserve( 256 );
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
    //��������� �������, ���������� ������� ��������
    __log.PrintInfo( Filelevel_DEBUG, "__collisionPairList => count[%d]", __collisionPairList.size() );
    for( auto &pair: __collisionPairList ) {
      int exists = 0;
      auto iterEnd = __collisionList->end();
      for( auto iter = __collisionList->begin(); iter != iterEnd && exists < 2; ++iter ) {
        if( *iter == pair.platform || *iter == pair.target ) {
          ++exists;
        }
      }
      if( exists == 2 ) {
        pair.target->SetPositionBy( pair.platform->GetOffsetFromLastPosition() );
        __log.PrintInfo( Filelevel_DEBUG, "__collisionPairList => move by[%3.3f; %3.3f]", pair.platform->GetOffsetFromLastPosition().x, pair.platform->GetOffsetFromLastPosition().y );
      }
    }
    __collisionPairList.clear();

    for( iter0 = __collisionList->begin(); iter0 != iterEnd; ++iter0 )
      ( *iter0 )->Update( updateInterval );

    //��������� ���������� � ��������
    //�� ��������� �������+������� ��� �������+��������, �.�. ��� ���������� ������� ��� �������� ����� ����������� �� ���� ��������

    //�����������: �������� ����������� �� ����� ���������, � � ��� ������ ��������, ��� ��� ����� ���� � ������ __collisionList
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
          //iter0 ��� ������ ��������
          //iter1 ��� ��� ������
          if( iter0 == iter1 )  //��� � ����� �� ������������
          {
            allowTestWidthDynamic = true; //��������� ��������� ��� ��������, ��� ������ �� ������
            continue;
          }

          if( !( *iter1 )->IsStatic() && !allowTestWidthDynamic ) //���������� ��������, ������� ������������ iter0
            continue;

          //�������� ��������� (���������� ����� �������� ������ ���� ������, ��� ����� ��������)
          if( this->CheckDistance( **iter0, **iter1 ) )
          {
            //__log.PrintInfo( Filelevel_DEBUG, "test" );
            if( ( *iter0 )->TestIntersect( **iter1 ) )
            {
              __collisionPairList.push_back( CollisionPair( *iter1, *iter0 ) );
              if( !( *iter1 )->IsStatic() ) {
                __collisionPairList.push_back( CollisionPair( *iter0, *iter1 ) );
              }
              //__log.PrintInfo( Filelevel_DEBUG, "intersect" );
            }
          }//CheckDistance
        }
      }

      //�� ���������� �������� ������ �����, ��� ���������� ������� ������
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
  �������� ���������� ����� Collision
=============
*/
bool CollisionManager::CheckDistance( const Collision &item0, const Collision &item1 )
{
  Vec3 distanceVector = item1.GetPosition() - item0.GetPosition();
  float distance = distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y;

  //__log.PrintInfo( Filelevel_DEBUG, "vec[%3.3f; %3.3f] => %3.3f ~ %3.3f", distanceVector.x, distanceVector.y, distance, item0.GetRadius2() + item1.GetRadius2() );

  return distance < item0.GetRadius2() + item1.GetRadius2();
}//CheckDistance
