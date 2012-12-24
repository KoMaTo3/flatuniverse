#include "mouse.h"
#include "file.h"
#include "imageloader.h"



Mouse::Mouse()
:windowSize( 1, 1 )
{
  this->cursor.position.Set( 0.0f, 0.0f );
  this->cursor.size.Set( 1.0f, 1.0f );
  this->cursor.sprite = NULL;
}//constructor



Mouse::~Mouse()
{
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
void Mouse::SetCursor( const std::string& imageFileName, Object* object )
{
  this->cursor.sprite = object;
  if( !object )
  {
    ShowCursor( true );
    return;
  }

  ImageLoader image;
  image.LoadFromFile( imageFileName );

  this->cursor.size.Set( float( image.GetImageSize()->width ) , float( image.GetImageSize()->height ) );
  this->cursor.spriteOffset.Set( this->cursor.size.x * 0.5f * this->cursor.pixelsToTexels.x, this->cursor.size.y * 0.5f * this->cursor.pixelsToTexels.y, 0.0f );

  this->cursor.sprite->SetPosition( Vec3( 0.0f, 0.0f, 9.0f ) );
  RenderableQuad *render = this->cursor.sprite->EnableRenderableGUI();
  render->SetSize( Vec2( this->cursor.size.x * this->cursor.pixelsToTexels.x, this->cursor.size.y * this->cursor.pixelsToTexels.y ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  render->SetTexture( imageFileName, Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );

  ShowCursor( false );
}//SetCursor



/*
=============
  MoveCursor
=============
*/
void Mouse::MoveCursor( const Vec2& newPosition )
{
  this->cursor.position = newPosition;
  if( this->cursor.sprite )
    this->cursor.sprite->SetPosition( Vec3( this->cursor.position.x * this->cursor.pixelsToTexels.x, this->cursor.position.y * this->cursor.pixelsToTexels.y, 9.0f ) + Vec3( this->cursor.spriteOffset.x, this->cursor.spriteOffset.y, 0.0f ) );
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
