/*==============================
  Класс SquareAtlas
  =================
  Вспомогательный класс для атласов текстур (склеивание нескольких текстур в одну).
  Хранит в себе свободные области, и адекватно делит их при заполнении атласа.

NOTES:
  Свободные площади должны храниться в map<Square,SizeLTRB> => отсюда сортировка по возрастанию площади
  1. Если нужная область по ширине или по высоте совпадает с одной из свободных, то свободная просто урезается
  2. Если нужная область по обоими параметрами меньше одной из свободных, то из свободной вырезается нужный кусок, и сама делится на 2 области
     При этом необходимо проверять все свободные области на факт пересечения и урезать их по правилам 1 и 2
  3. Проверить, поглощаются ли какие свободные области другими
==============================*/
#pragma once


#include "klib.h"
#include "file.h"
#include <map>

class SquareAtlas
{
public:
  struct _MapKey
  {
    Dword square, //площадь
          left,   //левый
          top;    //  угол
    _MapKey(){}
    _MapKey( Dword newSquare, Dword newLeft, Dword newTop )
    :square( newSquare ), left( newLeft ), top( newTop )
    { }
    _MapKey( const Rect<Dword> &rect )
    :square( ( rect.right - rect.left ) * ( rect.bottom - rect.top ) ), left( rect.left ), top( rect.top )
    { }
    Void __Dump() const;
  };
  struct _MapKey_Less
  {
    bool operator()( const _MapKey &l, const _MapKey &r ) const
    {
      return  ( l.square < r.square ) ||
              ( l.square == r.square && l.top < r.top ) ||
              ( l.square == r.square && l.top == r.top && l.left < r.left );
    }
  };

  typedef std::map< _MapKey, Rect< Dword >, _MapKey_Less > SquareAtlasMap;

private:
  SquareAtlasMap items;  //свободные блоки

public:
  SquareAtlas();
  virtual ~SquareAtlas();

  void        Init        ( const Size& size );
  bool        Cut         ( const Size& size, Rect< Dword > *result );
  bool        HasPlace    ( const Size& size );

  bool        _CanBePlaced( const Size& _who, const Rect< Dword > *_where );
  bool        _IsIntersect( const Rect< Dword >& square1, const Rect< Dword >& square2 );
  bool        _IsHave     ( const Rect< Dword >& minSquare, const Rect< Dword >& maxSquare );

  void        __Dump      ();
};
