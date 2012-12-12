#include "worldgrid.h"
#include "core/tools.h"
#include "core/file.h"


WorldGridList *__worldGridList = NULL;



/*
  [in] rootObject: объект, в котором будет хранитьс€ грид
*/
WorldGrid::WorldGrid( const Pos< short >& newPosition, Object *rootObject )
:rootGridObject( rootObject ), thisGridObject( NULL )
{
  if( !rootObject )
    __log.PrintInfo( Filelevel_ERROR, "WorldGrid constructor => rootObject is NULL" );
  else
  {
    char strRow[ 1024 ], strColumn[ 1024 ];
    sprintf_s( strRow, sizeof( strRow ), "gridr%d", newPosition.y );
    sprintf_s( strColumn, sizeof( strColumn ), "gridc%d", newPosition.x );

    Object *colObject = this->rootGridObject->GetChild( strColumn );
    if( !colObject ) //этот грид первый в колонке
    {
      colObject = new Object( strColumn, this->rootGridObject );
      __log.PrintInfo( Filelevel_DEBUG, "WorldGrid +1: created new column '%s'", colObject->GetNameFull().c_str() );
    }

    Object *rowObject = rowObject->GetChild( strRow );
    if( rowObject )
      __log.PrintInfo( Filelevel_ERROR, "WorldGrid constructor: this grid [%d; %d] already exists", newPosition.x, newPosition.y );
    else
    {
      this->thisGridObject = new Object( strRow, colObject );
      __log.PrintInfo( Filelevel_DEBUG, "WorldGrid: new grid [%d; %d]", newPosition.x, newPosition.y );
      this->position.pos.x = newPosition.x;
      this->position.pos.y = newPosition.y;
    }

  }
}//constructor



WorldGrid::~WorldGrid()
{
  DEF_DELETE( this->thisGridObject ); //удал€ютс€ все объекты грида
  __log.PrintInfo( Filelevel_DEBUG, "WorldGrid deleted: pos[%d; %d]", this->position.pos.x, this->position.pos.y );
}//destructor
