#include "squareatlas.h"
#include "file.h"
#include <vector>



void SquareAtlas::_MapKey::__Dump() const
{
  __log.PrintInfo( Filelevel_DEBUG, "SquareAtlas::_MapKey::__Dump => square[%d] left[%d] top[%d]", this->square, this->left, this->top );
}


SquareAtlas::SquareAtlas()
{
}//constructor


SquareAtlas::~SquareAtlas()
{
}//destructor


/*
----------
  Init
----------
*/
void SquareAtlas::Init( const Size& size )
{
  this->items.clear();
  this->items.insert( std::make_pair< _MapKey, Rect< Dword > >( _MapKey( size.width * size.height, 0, 0 ), Rect< Dword >( 0, 0, size.width - 1, size.height - 1 ) ) );

  __log.PrintInfo( Filelevel_DEBUG, "SquareAtlas::Init => %d x %d", size.width, size.height );
}//Init


/*
----------
  HasPlace
  true если в атласе есть место под блок
----------
*/
bool SquareAtlas::HasPlace( const Size& size )
{
  SquareAtlasMap::iterator iterEnd = this->items.end();
  for( SquareAtlasMap::iterator iter = this->items.begin(); iter != iterEnd; ++iter )
    if( this->_CanBePlaced( size, &iter->second ) )
      return true;

  return false;
}//HasPlace


/*
----------
  Cut
  Поиск свободной области, вырезание её и возврат координат
----------
*/
bool SquareAtlas::Cut( const Size& size, Rect< Dword > *result )
{
  //__log.PrintInfo( Filelevel_DEBUG, "SquareAtlas::Cut => %d x %d", size.width, size.height );

  //поиск достаточной области
  SquareAtlasMap::iterator iter, iter2, iterEnd;
  bool  can = false;
  Rect< Dword > placeTo;

  //this->__Dump();

  iterEnd = this->items.end();
  for( iter = this->items.begin(); iter != iterEnd; ++iter )
    if( this->_CanBePlaced( size, &iter->second ) )
    {
      can = true;
      placeTo = Rect< Dword >( iter->second.left, iter->second.top, iter->second.left + size.width - 1, iter->second.top + size.height - 1 );
      //__log.PrintInfo( Filelevel_DEBUG, "place to (%d, %d)-(%d, %d)", placeTo.left, placeTo.top, placeTo.right, placeTo.bottom );
      break;
    }

  if( !can )
  {
    __log.PrintInfo( Filelevel_WARNING, "SquareAtlas::Cut => false" );
    return false;
  }
  else {
    //__log.PrintInfo( Filelevel_DEBUG, "SquareAtlas::Cut => finded place" );
  }

  //место есть => режем все пересекающие области
  //предположительно (как бы это ещё проверить?) - новый блок не может быть в центре какого-либо другого блока,
  //  только перекрывать с бока/угла (да и с угла - вряд ли)
  Rect< Dword > rect;
  iterEnd = this->items.end();
  for( iter = this->items.begin(); iter != iterEnd; ++iter )
  {
    if( this->_IsIntersect( placeTo, iter->second ) ) //нужная площадь пересекает свободную => обрезаем
    {
      //__log.PrintInfo( Filelevel_DEBUG, "is intersect rect(%d, %d)-(%d, %d)", iter->second.left, iter->second.top, iter->second.right, iter->second.bottom );

      //разрезаем iter->second
      Rect< Dword > iterRect = iter->second;
      this->items.erase( iter );

      /*
      if( placeTo.top > iterRect.top )  //нужная площадь ниже этой => добавляется площадь сверху
      {
        rect = Rect< Dword >( iterRect.left, iterRect.top, iterRect.right, placeTo.top - 1 );
        //this->items[ _MapKey( &rect ) ] = rect;
        this->items.insert( std::make_pair< _MapKey, Rect< Dword > >( _MapKey( rect ), rect ) );
        __log.PrintInfo( Filelevel_DEBUG, "added top block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );
      }

      if( placeTo.bottom < iterRect.bottom )  //нужная площадь выше этой => добавляется площадь снизу
      {
        rect = Rect< Dword >( iterRect.left, placeTo.bottom + 1, iterRect.right, iterRect.bottom );
        //this->items[ _MapKey( &rect ) ] = rect;
        this->items.insert( std::make_pair< _MapKey, Rect< Dword > >( _MapKey( rect ), rect ) );
        //this->items.insert( SquareAtlas::SquareAtlas::reference( _MapKey( &rect ) ) );
        //this->items[ _MapKey( &rect ) ] = rect;
        __log.PrintInfo( Filelevel_DEBUG, "added bottom block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );
      }

      if( placeTo.left > iterRect.left )  //нужная площадь правее этой => добавляется площадь слева
      {
        rect = Rect< Dword >( iterRect.left, iterRect.top, placeTo.left - 1, iterRect.bottom );
        //this->items[ _MapKey( &rect ) ] = rect;
        this->items.insert( std::make_pair< _MapKey, Rect< Dword > >( _MapKey( rect ), rect ) );
        __log.PrintInfo( Filelevel_DEBUG, "added left block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );
      }

      if( placeTo.right < iterRect.right )  //нужная площадь левее этой => добавляется площадь справа
      {
        rect = Rect< Dword >( placeTo.right + 1, iterRect.top, iterRect.right, iterRect.bottom );
        //this->items[ _MapKey( &rect ) ] = rect;
        this->items.insert( std::make_pair< _MapKey, Rect< Dword > >( _MapKey( rect ), rect ) );
        __log.PrintInfo( Filelevel_DEBUG, "added right block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );
      }
      */

      //Нужная площадь всегда в левом-верхнем углу
      rect = Rect< Dword >( placeTo.left, placeTo.bottom + 1, iterRect.right, iterRect.bottom );
	    this->items.insert( SquareAtlasMap::value_type( _MapKey( rect ), rect ) );
      //__log.PrintInfo( Filelevel_DEBUG, "added right block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );

      rect = Rect< Dword >( placeTo.right + 1, placeTo.top, iterRect.right, placeTo.bottom );
	    this->items.insert( SquareAtlasMap::value_type( _MapKey( rect ), rect ) );
      //__log.PrintInfo( Filelevel_DEBUG, "added right block: (%d, %d)-(%d, %d)", rect.left, rect.top, rect.right, rect.bottom );

      //__log.PrintInfo( Filelevel_DEBUG, "deleted block: (%d, %d)-(%d, %d)", iterRect.left, iterRect.top, iterRect.right, iterRect.bottom );
      break;
    }//обрезка
  }//for items

  //this->__Dump();

  //проверка на поглощение одних свободных блоков другими
  bool deleted = true;

  //__log.PrintInfo( Filelevel_DEBUG, ". checking for fully-placed rects" );
  while( deleted )
  {
    deleted = false;
    iterEnd = this->items.end();
    for( iter = this->items.begin(); iter != iterEnd && !deleted; ++iter )
    for( iter2 = iter; iter2 != iterEnd; ++iter2 )
    {
      if( iter == iter2 )
        continue;
      if( this->_IsHave( iter->second, iter2->second ) )  //iter полностью в iter2
      {
        /*
        __log.PrintInfo( Filelevel_DEBUG, "rect(%d, %d)-(%d, %d) fully placed in rect(%d, %d)-(%d, %d)",
          iter->second.left, iter->second.top, iter->second.right, iter->second.bottom,
          iter2->second.left, iter2->second.top, iter2->second.right, iter2->second.bottom
          );*/
        deleted = true;
        this->items.erase( iter );
        iter = this->items.begin();
        break;
      }
      else
      if( this->_IsHave( iter2->second, iter->second ) )  //iter2 полностью в iter
      {
        /*
        __log.PrintInfo( Filelevel_DEBUG, "rect(%d, %d)-(%d, %d) fully placed in rect(%d, %d)-(%d, %d)",
          iter2->second.left, iter2->second.top, iter2->second.right, iter2->second.bottom,
          iter->second.left, iter->second.top, iter->second.right, iter->second.bottom
          );*/
        deleted = true;
        this->items.erase( iter2 );
        iter = this->items.begin();
        break;
      }
    }//for
  }//while deleted
  //__log.PrintInfo( Filelevel_DEBUG, ". checking done" );


  if( result )
    *result = placeTo;

  //this->__Dump();

  //__log.PrintInfo( Filelevel_DEBUG, "SquareAtlas::Cut => Done" );
  return true;
}//Cut


/*
----------
  _CanBePlaced
  Проверка, можно ли квадрат _who уместить в области _where
----------
*/
bool SquareAtlas::_CanBePlaced( const Size& _who, const Rect<Dword> *_where )
{
  if( _who.width > _where->right - _where->left + 1 )
    return false;

  if( _who.height > _where->bottom - _where->top + 1 )
    return false;

  return true;
}//_CanBePlaced


/*
----------
  _IsIntersect
  Проверка на пересечение областей
----------
*/
bool SquareAtlas::_IsIntersect( const Rect< Dword >& square1, const Rect< Dword >& square2 )
{
  if( square1.right < square2.left )
    return false;

  if( square1.left > square2.right )
    return false;

  if( square1.top > square2.bottom )
    return false;

  if( square1.bottom < square2.top )
    return false;

  return true;
}//_IsIntersect


/*
----------
  __Dump
----------
*/
void SquareAtlas::__Dump()
{
  __log.PrintInfo( Filelevel_DEBUG, "free rects in atlas: %d", this->items.size() );
  for( SquareAtlasMap::iterator iter = this->items.begin(); iter != this->items.end(); ++iter )
    __log.PrintInfo( Filelevel_DEBUG, ". square[%d] (%d, %d)-(%d, %d)", ( ( iter->second.right - iter->second.left ) * ( iter->second.bottom - iter->second.top ) ), iter->second.left, iter->second.top, iter->second.right, iter->second.bottom );
}//__Dump


/*
----------
  _IsHave
  Проверка на полное поглощение блока minSquare блоком maxSquare
----------
*/
bool SquareAtlas::_IsHave( const Rect< Dword >& minSquare, const Rect< Dword >& maxSquare )
{
  return
    (
    minSquare.left   >= maxSquare.left  &&
    minSquare.right  <= maxSquare.right &&
    minSquare.top    >= maxSquare.top   &&
    minSquare.bottom <= maxSquare.bottom
    /*
      minSquare(48, 32, 63, 63)
      maxSquare(48, 16, 63, 63)
      48 >= 48 +
      63 <= 63 +
      32 >= 16 +
      63 <= 63 +
    */
    )
  ;
}//_IsHave
