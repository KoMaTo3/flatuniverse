/*==============================
  ����� SquareAtlas
  =================
  ��������������� ����� ��� ������� ������� (���������� ���������� ������� � ����).
  ������ � ���� ��������� �������, � ��������� ����� �� ��� ���������� ������.

NOTES:
  ��������� ������� ������ ��������� � map<Square,SizeLTRB> => ������ ���������� �� ����������� �������
  1. ���� ������ ������� �� ������ ��� �� ������ ��������� � ����� �� ���������, �� ��������� ������ ���������
  2. ���� ������ ������� �� ������ ����������� ������ ����� �� ���������, �� �� ��������� ���������� ������ �����, � ���� ������� �� 2 �������
     ��� ���� ���������� ��������� ��� ��������� ������� �� ���� ����������� � ������� �� �� �������� 1 � 2
  3. ���������, ����������� �� ����� ��������� ������� �������
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
    Dword square, //�������
          left,   //�����
          top;    //  ����
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
  SquareAtlasMap items;  //��������� �����

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
