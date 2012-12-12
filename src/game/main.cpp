#include <windows.h>
#include "core/core.h"
#include "core/imageloader.h"
#include "worldgridmgr.h"

Core *core = NULL;
WorldGridManager *world = NULL;

File __log;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
  bool isDebug = true;

  core = new Core();
  core->Init( 320, 200, false, "FlatGL" );

  Object *obj;
  RenderableQuad *quad;
  Collision *col;
  float worldAlpha = ( isDebug ? 0.1f : 1.0f );

  
  core->CreateObject( "gui" );
  core->CreateObject( "lifts" );

  core->mouse.SetCursor( "data/temp/cursor_hw.png", core->CreateObject( "mouse-cursor", core->GetObject( "gui" ) ) );
  //__log.PrintInfo( Filelevel_DEBUG, "test: %s", core->GetObject( "gui" )->GetNameFull().c_str() );
  core->CreateObject( "bottom-block", core->GetObject( "gui" ) )->SetPosition( Vec3( 50.0f, 85.0f, 2.0f ) )->EnableRenderableGUI( 2.0f )->SetSize( Vec2( 100.0f, 30.0f ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 0.7f ) )
    ->SetTexture( "data/temp/ui-bg-0.png" );

  world = new WorldGridManager( core->CreateObject( "gridcore" ) );

  obj = core->CreateObject( "player" );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0.0f );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/mario.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 17.0f, 20.0f, 0.0f ) );
  col->SetPosition( Vec3( 65.0f, 30.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 500.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  //quad->scale.Set( 0.5f, 1.0f );
  core->SetCamera( obj );

  obj = core->CreateObject( "player-halo", obj );
  obj->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  //obj->AttachPoisitionToParent();
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 0.7f ) );
  quad->SetSize( Vec2( 15.0f, 15.0f ) );
  quad->SetTexture( "data/temp/glow.tga", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  //core->SetCamera( obj );

  obj = core->CreateObject( "halo", core->GetObject( "/gui/mouse-cursor" ) );
  obj->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderableGUI( 5.0f );
  quad->SetColor( Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
  quad->SetSize( Vec2( 2.0f, 2.0f ) );
  quad->SetTexture( "data/temp/glow.tga", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );

  obj = core->CreateObject( "wall-top" );
  obj->SetPosition( Vec3( 53.0f, 0.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 50.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 50.0f, 10.0f ) );

  obj = core->CreateObject( "wall-bottom" );
  obj->SetPosition( Vec3( 103.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 150.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 150.0f, 10.0f ) );

  obj = core->CreateObject( "wall-left" );
  obj->SetPosition( Vec3( 15.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 55.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 55.0f ) );

  obj = core->CreateObject( "wall-right" );
  obj->SetPosition( Vec3( 185.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 50.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 50.0f ) );

  obj = core->CreateObject( "enemy" );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 10.0f, 10.0f ) );
  quad->SetTexture( "data/temp/enemy0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 10.0f, 0.0f ) );
  col->SetPosition( Vec3( 45.0f, 30.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 400.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  //quad->scale.Set( 0.5f, 1.0f );

  obj = core->CreateObject( "enemy-2" );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/enemy0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 20.0f, 20.0f, 0.0f ) );
  col->SetPosition( Vec3( 40.0f, 0.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 400.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );

  obj = core->CreateObject( "lift", core->GetObject( "lifts" ) );
  obj->SetPosition( Vec3( -5.0f, 40.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick2.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 30.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 30.0f, 10.0f ) );

  obj = core->CreateObject( "lift-2", core->GetObject( "lifts" ) );
  obj->SetPosition( Vec3( -35.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 30.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 0.0f, 0.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 30.0f, 10.0f ) );

  obj = core->CreateObject( "test-bg-0" );
  obj->SetPosition( Vec3( 90.0f, 85.0f, 1.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, -1.0 );
  quad->SetTexture( "data/temp/brick2.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );

  obj = core->CreateObject( "test-bg-1" );
  obj->SetPosition( Vec3( 100.0f, 85.0f, -1.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 1.0 );
  quad->SetTexture( "data/temp/brick2.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );

  //фоновая картинка

  /*
  obj = core->CreateObject( "bg-0" );
  obj->SetPosition( Vec3( 160.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, -4.0f );
  quad->SetTexture( "data/temp/thu128.jpg" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 320.0f, 320.0f ) );
  */

  /*
  obj = core->CreateObject( "cube" );
  obj->SetPosition( Vec3( 55.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 10.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 150.0f, 0.0f ) );
  col->SetIsStatic( false );
  quad->SetColor( Vec4( 0.0f, 0.0f, 1.0f, 0worldAlpha1f ) );
  quad->SetSize( Vec2( 10.0f, 10.0f ) );
  */

  float t = 0.0f, rot = 0.0f;
  wglSwapIntervalEXT( false );
  char tempChar[ 1024 ];
  DWORD fps = 0, currentFps = 0;
  //__textureAtlas->__Dump( "data/temp/__atlas.tga" );

  sTimer.Update();
  while( core->Update() )
  {
    core->Redraw();
    world->Update();

    t += sTimer.GetDeltaF();
    rot += sTimer.GetDeltaF();
    if( t > 1.0f )
    {
      t -= 1.0f;
      fps = currentFps;
      currentFps = 0;
    }

    if( core->keyboard.IsPressed( VK_RETURN ) )
      core->RemoveObject( "wall-right" );

    if( core->keyboard.IsPressed( VK_ESCAPE ) )
      core->SetState( CORE_STATE_EXIT );

    if( core->keyboard.IsPressed( VK_LEFT ) )
    {
      obj = core->GetObject( "player" );
      obj->SetForce( 4, Vec3( -100.0f, 0.0f, 0.0f ) );
    }

    if( core->keyboard.IsReleased( VK_LEFT ) )
    {
      obj = core->GetObject( "player" );
      obj->RemoveForce( 4 );
    }

    if( core->keyboard.IsPressed( VK_RIGHT ) )
    {
      obj = core->GetObject( "player" );
      obj->SetForce( 2, Vec3( 100.0f, 0.0f, 0.0f ) );
    }

    if( core->keyboard.IsReleased( VK_RIGHT ) )
    {
      obj = core->GetObject( "player" );
      obj->RemoveForce( 2 );
    }

    if( core->keyboard.IsPressed( VK_UP ) )
    {
      obj = core->GetObject( "player" );
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, -250.0f, 0.0f ) );
    }

    if( core->keyboard.IsPressed( VK_DOWN ) )
    {
      obj = core->GetObject( "player" );
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, 100.0f, 0.0f ) );
    }

    if( core->mouse.IsPressed( VK_RBUTTON ) )
    {
      Vec3 pos( core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y, 0.0f );
      pos += core->GetCamera()->GetPosition() - Vec3( core->GetWindowHalfSize().x, core->GetWindowHalfSize().y, 0.0f );
      core->GetObject( "player" )->SetPosition( pos );
      //core->SetState( CORE_STATE_EXIT );
    }

    if( core->mouse.IsPressed( VK_LBUTTON ) )
    {
      Vec3 pos( core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y, 0.0f );
      pos += core->GetCamera()->GetPosition() - Vec3( core->GetWindowHalfSize().x, core->GetWindowHalfSize().y, 0.0f );
      Object *obj = core->CreateObject( "new-block" );
      Collision *col = obj->EnableCollision();
      col->SetPosition( pos )->SetIsStatic( true )->SetSize( Vec3( 10.0f, 10.0f, 0.0f ) );
      RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
      quad->SetColor( Vec4( ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, worldAlpha ) );
      quad->SetSize( Vec2( 10.0f, 10.0f ) );
      quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
    }

    if( core->keyboard.IsPressed( VK_TAB ) )
    {
      static long num = 0;
      static char t[ 1024 ];
      for( int q = 0; q < 500; ++q )
      {
        ++num;
        sprintf_s( t, 1024, "grid0_0/%d", num );
        Object *obj = core->CreateObject( t );
        RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, 0 );
        quad->SetColor( Vec4( ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, 0.1f ) );
        quad->SetRotation( ( rand() % 3600 ) / 10.0f );
        quad->SetPosition( Vec3( ( rand() % 1000 ) / 10.0f, ( rand() % 1000 ) / 10.0f, 0.0f ) );
        quad->SetSize( Vec2( ( rand() % 500 ) / 10.0f, ( rand() % 500 ) / 10.0f ) );
        //obj->EnableCollision();
      }
      //core->RemoveObject( "/gui/button-exit" );
    }

    if( obj = core->GetObject( "/lifts/lift" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 5.0f ) * 300.0f, 0.0f ) );
    if( obj = core->GetObject( "/lifts/lift-2" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 3.0f ) * 100.0f, 0.0f ) );
    if( obj = core->GetObject( "/player/player-halo" ) )
    {
      obj->SetPosition( Vec3( Math::Sin16( rot * 3.0f ) * 20.0f, -Math::Cos16( rot * 3.0f ) * 20.0f , 0.0f ) );
      ( ( RenderableQuad* ) obj->GetRenderable() )->SetColor( Vec4( Math::Sin16( rot * 5.0f ) * 0.3f + 0.7f, -Math::Sin16( rot * 3.0f ) * 0.3f + 0.7f, 1.0f, 0.99f ) );

      if( obj = core->GetObject( "/gui/mouse-cursor/halo" ) )
      {
        Vec3 cursorPos = core->PixelToTexel( Vec2( 9.0f, 0 ) );
        obj->SetPosition( Vec3( Math::Sin16( rot * 15.0f ) - cursorPos.x, Math::Cos16( rot * 15.0f ) - cursorPos.y, 0.0f ) );
        //( ( RenderableQuad* ) obj->GetRenderable() )->SetColor( Vec4( Math::Sin16( rot * 5.0f ) * 0.3f + 0.7f, -Math::Sin16( rot * 3.0f ) * 0.3f + 0.7f, 1.0f, 0.99f ) );
      }
    }
      //obj->SetPosition( Vec3( -20.0f, 50.0f + Math::Sin16( rot * 5.0f ) * 50.0f, 0.0f ) );

    ++currentFps;

    sprintf_s( tempChar, 1024, "FPS[%d], quads[%d], cursor[%3.0f; %3.0f]", fps, __coreRenderableList->size(), core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
    core->SetWindowTitle( tempChar );
  }
  //MessageBox( NULL, "Ok", "Debug", MB_OK );

  core->Destroy();
  DEF_DELETE( world );
  DEF_DELETE( core );

  return 0;
}
