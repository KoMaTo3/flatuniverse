#include <windows.h>
#include "core/core.h"
#include "core/imageloader.h"
#include "worldgridmgr.h"
#include "game.h"

WorldGridManager *world = NULL;
Game *game = NULL;

extern CoreRenderableListIndicies  *__coreRenderableListIndicies;
extern CoreRenderableListIndicies  *__coreGUIIndicies;


File __log;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
  bool isDebug = true;

  game = new Game();
  game->core->Init( 320, 200, false, "FlatGL" );

  Object *obj;
  RenderableQuad *quad;
  Collision *col;
  float worldAlpha = ( isDebug ? 0.1f : 1.0f );

  
  game->core->CreateObject( "gui" );
  game->core->CreateObject( "lifts" );

  game->core->mouse.SetCursor( "data/temp/cursor_hw.png", game->core->CreateObject( "mouse-cursor", game->core->GetObject( "gui" ) ) );
  //__log.PrintInfo( Filelevel_DEBUG, "test: %s", core->GetObject( "gui" )->GetNameFull().c_str() );
  game->core->CreateObject( "bottom-block", game->core->GetObject( "gui" ) )->SetPosition( Vec3( 50.0f, 85.0f, 2.0f ) )->EnableRenderableGUI()->SetSize( Vec2( 100.0f, 30.0f ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 0.7f ) )
    ->SetTexture( "data/temp/ui-bg-0.png" );
  game->core->CreateObject( "tooltips" );
  game->core->CreateObject( "mouse-grid", game->core->GetObject( "tooltips" ) );

  obj = game->core->CreateObject( "test-bg-2" );
  obj->SetPosition( Vec3( 50.0f, 50.0f, -9.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/AUCHINDOUN_VORTEXCLOUD01.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 100.0f, 100.0f ) );


  world = new WorldGridManager( game->core->CreateObject( "gridcore" ) );

  obj = game->core->CreateObject( "player" );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 17.0f, 20.0f, 0.0f ) );
  col->SetPosition( Vec3( 65.0f, 30.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 500.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/mario.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  //quad->scale.Set( 0.5f, 1.0f );
  game->core->SetCamera( obj );

  obj = game->core->CreateObject( "player-halo", obj );
  obj->SetPosition( Vec3( 0.0f, 0.0f, 0.1f ) );
  //obj->AttachPoisitionToParent();
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 0.7f ) );
  quad->SetSize( Vec2( 15.0f, 15.0f ) );
  quad->SetTexture( "data/temp/T_VFX_FLOWER.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  //game->core->SetCamera( obj );

  obj = game->core->CreateObject( "halo", game->core->GetObject( "/gui/mouse-cursor" ) );
  obj->SetPosition( Vec3( 0.0f, 0.0f, 5.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderableGUI();
  quad->SetColor( Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
  quad->SetSize( Vec2( 2.0f, 2.0f ) );
  quad->SetTexture( "data/temp/GENERICGLOW64.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );

  obj = game->core->CreateObject( "wall-top" );
  obj->SetPosition( Vec3( 53.0f, 0.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 50.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 50.0f, 10.0f ) );

  obj = game->core->CreateObject( "wall-bottom" );
  obj->SetPosition( Vec3( 103.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 150.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 150.0f, 10.0f ) );

  obj = game->core->CreateObject( "wall-left" );
  obj->SetPosition( Vec3( 15.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 55.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 55.0f ) );

  obj = game->core->CreateObject( "wall-right" );
  obj->SetPosition( Vec3( 185.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 50.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 50.0f ) );

  obj = game->core->CreateObject( "enemy" );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 10.0f, 0.0f ) );
  col->SetPosition( Vec3( 45.0f, 30.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 400.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 10.0f, 10.0f ) );
  quad->SetTexture( "data/temp/enemy0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  //quad->scale.Set( 0.5f, 1.0f );

  obj = game->core->CreateObject( "enemy-2" );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 20.0f, 20.0f, 0.0f ) );
  col->SetPosition( Vec3( 40.0f, 0.0f, 0.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 400.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/enemy0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );

  obj = game->core->CreateObject( "lift", game->core->GetObject( "lifts" ) );
  obj->SetPosition( Vec3( -5.0f, 40.0f, 0.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 30.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick2.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 30.0f, 10.0f ) );

  obj = game->core->CreateObject( "lift-2", game->core->GetObject( "lifts" ) );
  obj->SetPosition( Vec3( -35.0f, 50.0f, 0.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 30.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  quad->SetColor( Vec4( 0.0f, 0.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 30.0f, 10.0f ) );

  obj = game->core->CreateObject( "test-bg-0" );
  obj->SetPosition( Vec3( 90.0f, 85.0f, -1.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick2.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );

  obj = game->core->CreateObject( "test-bg-1" );
  obj->SetPosition( Vec3( 100.0f, 85.0f, 1.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick2.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );

  game->world->AttachObjectToGrid( WorldGrid::WorldGridPosition( 0, 0 ), game->core->GetObject( "wall-left" ) );

  //������� ��������

  /*
  obj = game->core->CreateObject( "bg-0" );
  obj->SetPosition( Vec3( 160.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, -4.0f );
  quad->SetTexture( "data/temp/thu128.jpg" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 320.0f, 320.0f ) );
  */

  /*
  obj = game->core->CreateObject( "cube" );
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
  while( game->core->Update() )
  {
    game->core->Redraw();
    world->Update();

    t += sTimer.GetDeltaF();
    rot += sTimer.GetDeltaF();
    if( t > 1.0f )
    {
      t -= 1.0f;
      fps = currentFps;
      currentFps = 0;
    }

    if( game->core->keyboard.IsPressed( VK_RETURN ) )
      game->core->RemoveObject( "wall-right" );

    if( game->core->keyboard.IsPressed( VK_ESCAPE ) )
      game->core->SetState( CORE_STATE_EXIT );

    if( game->core->keyboard.IsPressed( VK_LEFT ) || game->core->keyboard.IsPressed( VK_LETTER_A ) )
    {
      obj = game->core->GetObject( "player" );
      obj->SetForce( 4, Vec3( -150.0f, 0.0f, 0.0f ) );
    }

    if( game->core->keyboard.IsReleased( VK_LEFT ) || game->core->keyboard.IsReleased( VK_LETTER_A ) )
    {
      obj = game->core->GetObject( "player" );
      obj->RemoveForce( 4 );
    }

    if( game->core->keyboard.IsPressed( VK_RIGHT ) || game->core->keyboard.IsPressed( VK_LETTER_D ) )
    {
      obj = game->core->GetObject( "player" );
      obj->SetForce( 2, Vec3( 150.0f, 0.0f, 0.0f ) );
    }

    if( game->core->keyboard.IsReleased( VK_RIGHT ) || game->core->keyboard.IsReleased( VK_LETTER_D ) )
    {
      obj = game->core->GetObject( "player" );
      obj->RemoveForce( 2 );
    }

    if( game->core->keyboard.IsPressed( VK_UP ) || game->core->keyboard.IsPressed( VK_LETTER_W ) )
    {
      obj = game->core->GetObject( "player" );
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, -250.0f, 0.0f ) );
    }

    if( game->core->keyboard.IsPressed( VK_DOWN ) )
    {
      obj = game->core->GetObject( "player" );
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, 100.0f, 0.0f ) );
    }

    if( game->core->mouse.IsPressed( VK_RBUTTON ) )
    {
      Vec3 pos( game->core->mouse.GetCursorPosition().x, game->core->mouse.GetCursorPosition().y, 0.0f );
      pos += game->core->GetCamera()->GetPosition() - Vec3( game->core->GetWindowHalfSize().x, game->core->GetWindowHalfSize().y, 0.0f );
      game->core->GetObject( "player" )->SetPosition( pos );
      //game->core->SetState( CORE_STATE_EXIT );
    }

    if( game->core->mouse.IsPressed( VK_LBUTTON ) )
    {
      //Vec3 pos( game->core->mouse.GetCursorPosition().x, game->core->mouse.GetCursorPosition().y, 0.0f );
      //pos += game->core->GetCamera()->GetPosition() - Vec3( game->core->GetWindowHalfSize().x, game->core->GetWindowHalfSize().y, 0.0f );

      Vec3 pos = game->GetGridPosUnderCursor() * float( WORLD_GRID_BLOCK_SIZE );

      Object *obj = game->core->CreateObject( "new-block" );
      Collision *col = obj->EnableCollision();
      col->SetPosition( pos )->SetIsStatic( true )->SetSize( Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0.0f ) );
      RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
      quad->SetColor( Vec4( ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, worldAlpha ) );
      quad->SetSize( Vec2( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE ) );
      quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
    }

    if( game->core->keyboard.IsPressed( VK_TAB ) )
    {
      static long num = 0;
      static char t[ 1024 ];
      for( int q = 0; q < 500; ++q )
      {
        ++num;
        sprintf_s( t, 1024, "grid0_0/%d", num );
        Object *obj = game->core->CreateObject( t )->SetPosition( Vec3( 0, 0, 0 ) );
        RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
        quad->SetColor( Vec4( ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, 0.1f ) );
        quad->SetRotation( ( rand() % 3600 ) / 10.0f );
        quad->SetPosition( Vec3( ( rand() % 1000 ) / 10.0f, ( rand() % 1000 ) / 10.0f, 0.0f ) );
        quad->SetSize( Vec2( ( rand() % 500 ) / 10.0f, ( rand() % 500 ) / 10.0f ) );
        quad->SetTexture( "data/temp/null.png" );
        //obj->EnableCollision();
      }
      //game->core->RemoveObject( "/gui/button-exit" );
    }

    if( game->core->keyboard.IsPressed( VK_F1 ) )
    {
      game->world->UnloadGrid( WorldGrid::WorldGridPosition( 0, 0 ) );
    }

    //mouse tooltips
    if( game->core->keyboard.IsPressed( VK_SHIFT ) )
    {
      game->gridTooltipUnderMouse.show = !game->gridTooltipUnderMouse.show;
      if( game->gridTooltipUnderMouse.show )
      {
        Vec3 pos = game->GetGridPosUnderCamera( 2.0f );
        pos.z = 9.0f;

        RenderableQuad *quad;

        char str[ 1024 ];
        for( long x = 0; x < 20; ++x )
        {
          sprintf_s( str, sizeof( str ), "grid-%d", x );
          game->gridTooltipUnderMouse.obj[ x ] = game->core->CreateObject( str, game->core->GetObject( "tooltips/mouse-grid" ) );
          quad = ( RenderableQuad* ) game->gridTooltipUnderMouse.obj[ x ]
          ->SetPosition( Vec3( pos ) * float( WORLD_GRID_BLOCK_SIZE ) * 2.0f - Vec3( WORLD_GRID_BLOCK_SIZE, 0, 0 ) * 2.0f * ( float( x ) - 9.0f ) )
            ->EnableRenderable( RENDERABLE_TYPE_QUAD );
          quad->SetColor( Vec4( 0, 1.0f, 0, 0.2f ) )
            ->SetSize( Vec2( WORLD_GRID_BLOCK_SIZE, 1000.0f ) )
            ->SetTexture( "data/temp/blank.bmp" );

          sprintf_s( str, sizeof( str ), "grid-%d", x + 20 );
          game->gridTooltipUnderMouse.obj[ x + 20 ] = game->core->CreateObject( str, game->core->GetObject( "tooltips/mouse-grid" ) );
          quad = ( RenderableQuad* ) game->gridTooltipUnderMouse.obj[ x + 20 ]
          ->SetPosition( Vec3( pos ) * float( WORLD_GRID_BLOCK_SIZE ) * 2.0f - Vec3( 0, WORLD_GRID_BLOCK_SIZE, 0 ) * 2.0f * ( float( x ) - 9.0f ) )
            ->EnableRenderable( RENDERABLE_TYPE_QUAD );
          quad->SetColor( Vec4( 0, 1.0f, 0, 0.2f ) )
            ->SetSize( Vec2( 1000.0f, WORLD_GRID_BLOCK_SIZE ) )
            ->SetTexture( "data/temp/blank.bmp" );
        }
      }
      else
      {
        for( long q = 0; q < 40; ++q )
          DEF_DELETE( game->gridTooltipUnderMouse.obj[ q ] );
      }
    }

    if( game->gridTooltipUnderMouse.show )
    {
      Vec3 pos = game->GetGridPosUnderCamera( 2.0f );

      char str[ 1024 ];
      for( long x = 0; x < 20; ++x )
      {
        sprintf_s( str, sizeof( str ), "tooltips/mouse-grid/grid-%d", x );
        game->core->GetObject( str )->SetPosition( Vec3( pos ) * float( WORLD_GRID_BLOCK_SIZE ) * 2.0f - Vec3( WORLD_GRID_BLOCK_SIZE, 0, 0 ) * 2.0f * ( float( x ) - 9.0f ) );
        sprintf_s( str, sizeof( str ), "tooltips/mouse-grid/grid-%d", x + 20 );
        game->core->GetObject( str )->SetPosition( Vec3( pos ) * float( WORLD_GRID_BLOCK_SIZE ) * 2.0f - Vec3( 0, WORLD_GRID_BLOCK_SIZE, 0 ) * 2.0f * ( float( x ) - 9.0f ) );
      }
    }

    if( obj = game->core->GetObject( "/lifts/lift" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 5.0f ) * 300.0f, 0.0f ) );
    if( obj = game->core->GetObject( "/lifts/lift-2" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 3.0f ) * 100.0f, 0.0f ) );
    if( obj = game->core->GetObject( "/player/player-halo" ) )
    {
      obj->SetPosition( Vec3( Math::Sin16( rot * 3.0f ) * 20.0f, -Math::Cos16( rot * 3.0f ) * 20.0f , 0.0f ) );
      ( ( RenderableQuad* ) obj->GetRenderable() )->SetColor( Vec4( Math::Sin16( rot * 5.0f ) * 0.3f + 0.7f, -Math::Sin16( rot * 3.0f ) * 0.3f + 0.7f, 1.0f, 0.5f ) )
        ->SetRotation( -rot * 3.0f );

      if( obj = game->core->GetObject( "/gui/mouse-cursor/halo" ) )
      {
        Vec3 cursorPos = game->core->PixelToTexel( Vec2( 9.0f, 0 ) );
        obj->SetPosition( Vec3( Math::Sin16( rot * 15.0f ) - cursorPos.x, Math::Cos16( rot * 15.0f ) - cursorPos.y, 0.0f ) );
        //( ( RenderableQuad* ) obj->GetRenderable() )->SetColor( Vec4( Math::Sin16( rot * 5.0f ) * 0.3f + 0.7f, -Math::Sin16( rot * 3.0f ) * 0.3f + 0.7f, 1.0f, 0.99f ) );
      }
    }

    if( obj = game->core->GetObject( "test-bg-2" ) )
    {
      quad = ( RenderableQuad* ) obj->GetRenderable();
      float scale = Math::Sin16( rot * 2.0f ) * 1.0f + 5.0f;
      quad->SetScale( Vec2( scale, scale ) )->SetRotation( -rot );
    }
      //obj->SetPosition( Vec3( -20.0f, 50.0f + Math::Sin16( rot * 5.0f ) * 50.0f, 0.0f ) );

    ++currentFps;

    sprintf_s( tempChar, 1024, "FPS[%d], quads[%d/%d], cursor[%3.0f; %3.0f]", fps, __coreRenderableListIndicies->size(), __coreGUIIndicies->size(), game->core->mouse.GetCursorPosition().x, game->core->mouse.GetCursorPosition().y );
    game->core->SetWindowTitle( tempChar );
  }
  //MessageBox( NULL, "Ok", "Debug", MB_OK );

  DEF_DELETE( game );
  DEF_DELETE( world );

  return 0;
}
