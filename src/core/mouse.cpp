#include "mouse.h"
#include "file.h"
#include "imageloader.h"
#include "tools.h"



Mouse::Mouse()
:windowSize( 1, 1 )
{
  this->cursor.position.Set( 0.0f, 0.0f );
  this->cursor.size.Set( 1.0f, 1.0f );
  this->cursor.sprite = NULL;
  this->cursor.isHardware = false;
  this->cursor.hCursor = NULL;
}//constructor



Mouse::~Mouse()
{
  DEF_DELETE( this->cursor.sprite );
}//destructor




/*
=============
  Init
=============
*/
void Mouse::Init( const Size& newWindowSize )
{
  this->windowSize = newWindowSize;
  this->cursor.pixelsToTexels = Vec2( 100.0f / float( this->windowSize.width ), 100.0f / float( this->windowSize.height ) );
}//Init



/*
=============
  SetCursor
=============
*/
void Mouse::SetCursor( const std::string& imageFileName, Object* object, bool setIsHardware )
{
  if( !object )
  {
    ShowCursor( true );
    return;
  }
  DEF_DELETE( this->cursor.sprite );
  this->cursor.sprite = new Pointer( object );
  this->cursor.isHardware = setIsHardware;

  ImageLoader image;
  image.LoadFromFile( imageFileName );

  this->cursor.size.Set( float( image.GetImageSize().width ) , float( image.GetImageSize().height ) );
  this->cursor.spriteOffset.Set( this->cursor.size.x * 0.5f * this->cursor.pixelsToTexels.x, this->cursor.size.y * 0.5f * this->cursor.pixelsToTexels.y, 0.0f );

  if( this->cursor.sprite->GetIsValid() )
  {
    this->cursor.sprite->GetObject< Object >()->SetPosition( Vec3( 0.0f, 0.0f, 9.0f ) );
    if( this->cursor.isHardware ) {
        ImageLoader image;
        image.LoadFromFile( imageFileName );

        //перевернуть RGBA в ARGB
        //и сделать пиксели с прозрачностью < minAlpha полностью прозрачными
        Dword *pos = ( Dword* ) image.GetImageData();
        Byte *block, minAlpha = 255;
        Size imageSize = image.GetImageSize();
        for( Dword y = 0; y < imageSize.height; ++y )
        for( Dword x = 0; x < imageSize.width; ++x ) {
          block = ( Byte* ) pos;
          *(pos++) = ( block[ 3 ] < minAlpha ? ARGB( 0, 0, 0, 0 ) : ARGB( 255, block[ 0 ], block[ 1 ], block[ 2 ] ) );
        }

        this->cursor.hCursor = this->_CreateCursorFromMemory( image.GetImageData(), &image.GetImageSize(), RGB( 0, 0, 0), 0, 0 );

        this->cursor.isHardware = true;
        ::ShowCursor( TRUE );
        this->ResetCursor();

    } else {
      RenderableQuad *render = this->cursor.sprite->GetObject< Object >()->EnableRenderableGUI();
      render->SetSize( Vec2( this->cursor.size.x * this->cursor.pixelsToTexels.x, this->cursor.size.y * this->cursor.pixelsToTexels.y ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
      render->SetTexture( imageFileName, Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
    }
  }

  ShowCursor( false );
}//SetCursor




/*
=============
  DestroyCursor
=============
*/
void Mouse::DestroyCursor() {
  if( this->cursor.sprite && this->cursor.sprite->GetIsValid() ) {
    this->cursor.sprite->GetObject< Object >()->DisableRenderable();
    ShowCursor( true );
  }
}//DestroyCursor




/*
=============
  MoveCursor
=============
*/
void Mouse::MoveCursor( const Vec2& newPosition )
{
  this->cursor.position = newPosition;
  if( this->cursor.sprite )
  {
    if( this->cursor.sprite->GetIsValid() )
      this->cursor.sprite->GetObject< Object >()->SetPosition( Vec3( this->cursor.position.x * this->cursor.pixelsToTexels.x, this->cursor.position.y * this->cursor.pixelsToTexels.y, 9.0f ) + Vec3( this->cursor.spriteOffset.x, this->cursor.spriteOffset.y, 0.0f ) );
    else
      DEF_DELETE( this->cursor.sprite );
  }

  if( !this->moveListeners.empty() )
  {
    MoveListenerList::iterator iter, iterEnd = this->moveListeners.end();
    for( iter = this->moveListeners.begin(); iter != iterEnd; ++iter )
      ( *iter )( this->cursor.position );
  }
}//MoveCursor



/*
=============
  GetCursorPosition
=============
*/
const Vec2& Mouse::GetCursorPosition()
{
  return this->cursor.position;
}//GetCursorPosition



/*
=============
  AddMoveListener
=============
*/
void Mouse::AddMoveListener( MoveListener *listenerProc )
{
  if( listenerProc )
    this->moveListeners.push_back( listenerProc );
  else
    __log.PrintInfo( Filelevel_WARNING, "Mouse::AddMoveListener => function is NULL" );
}//AddMoveListener



/*
=============
  RemoveMoveListener
=============
*/
void Mouse::RemoveMoveListener( MoveListener *listenerProc )
{
  MoveListenerList::iterator iter, iterEnd = this->moveListeners.end();
  for( iter = this->moveListeners.begin(); iter != iterEnd; ++iter )
    if( *iter == listenerProc )
    {
      this->moveListeners.erase( iter );
      return;
    }
}//RemoveMoveListener




/*
=============
  _CreateCursorFromMemory
=============
*/
HCURSOR Mouse::_CreateCursorFromMemory( Byte *buffer, const Size *size, COLORREF clrTransparent, DWORD xHotspot, DWORD yHotspot )
{
  HCURSOR hRetCursor = NULL;

  HBITMAP hSourceBitmap = CreateBitmap( size->width, size->height, 4, 8, buffer );

  if( hSourceBitmap )
  {
    HBITMAP hAndMask = NULL;
    HBITMAP hXorMask = NULL;
    this->_GetMaskBitmaps( hSourceBitmap, clrTransparent, hAndMask, hXorMask );
    if( NULL == hAndMask || NULL == hXorMask ) {
      __log.PrintInfo( Filelevel_ERROR, "CreateCursorFromBitmap" );
      return NULL;
    }

    ICONINFO iconinfo = { 0 };
    iconinfo.fIcon    = FALSE;
    iconinfo.xHotspot = xHotspot;
    iconinfo.yHotspot = yHotspot;
    iconinfo.hbmMask  = hAndMask;
    iconinfo.hbmColor = hXorMask;

    hRetCursor = ::CreateIconIndirect( &iconinfo );
    ::DeleteObject( hSourceBitmap );
  }

  return hRetCursor;
}//_CreateCursorFromMemory


/*
=============
  ResetCursor
=============
*/
void Mouse::ResetCursor()
{
  if( this->cursor.isHardware && this->cursor.hCursor ) {
    ::SetCursor( this->cursor.hCursor );
  }
}//ResetCursor



/*
=============
  _GetMaskBitmaps
=============
*/
void Mouse::_GetMaskBitmaps( HBITMAP hSourceBitmap, COLORREF clrTransparent, HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap )
{
  HDC hDC         = ::GetDC( NULL );
  HDC hMainDC     = ::CreateCompatibleDC( hDC );
  HDC hAndMaskDC  = ::CreateCompatibleDC( hDC );
  HDC hXorMaskDC  = ::CreateCompatibleDC( hDC );

  BITMAP bm;
  ::GetObject( hSourceBitmap, sizeof( BITMAP ), &bm );

  hAndMaskBitmap  = ::CreateCompatibleBitmap( hDC, bm.bmWidth, bm.bmHeight );
  hXorMaskBitmap  = ::CreateCompatibleBitmap( hDC, bm.bmWidth, bm.bmHeight );

  HBITMAP hOldMainBitmap    = ( HBITMAP ) ::SelectObject( hMainDC, hSourceBitmap );
  HBITMAP hOldAndMaskBitmap = ( HBITMAP ) ::SelectObject( hAndMaskDC, hAndMaskBitmap );
  HBITMAP hOldXorMaskBitmap = ( HBITMAP ) ::SelectObject( hXorMaskDC, hXorMaskBitmap );

  COLORREF MainBitPixel;
  for( Int x = 0; x < bm.bmWidth; ++x ) {
    for( Int y = 0; y < bm.bmHeight; ++y ) {
      MainBitPixel = ::GetPixel( hMainDC, x, y );
      if( MainBitPixel == clrTransparent ) {
        ::SetPixel( hAndMaskDC, x, y, RGB( 255, 255, 255 ) );
        ::SetPixel( hXorMaskDC, x, y, RGB( 0, 0, 0) );
      } else {
        ::SetPixel( hAndMaskDC, x, y, RGB( 0, 0, 0) );
        ::SetPixel( hXorMaskDC, x, y, MainBitPixel );
      }
    }
  }
  this->cursor.size.Set( ( float ) bm.bmWidth, ( float ) bm.bmHeight );
  
  ::SelectObject( hMainDC, hOldMainBitmap );
  ::SelectObject( hAndMaskDC, hOldAndMaskBitmap );
  ::SelectObject( hXorMaskDC, hOldXorMaskBitmap );

  ::DeleteDC( hXorMaskDC );
  ::DeleteDC( hAndMaskDC );
  ::DeleteDC( hMainDC );

  ::ReleaseDC( NULL, hDC );
}//_GetMaskBitmaps
