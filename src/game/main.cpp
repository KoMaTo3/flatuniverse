#include <windows.h>
#include "core/core.h"
#include "core/imageloader.h"
#include "core/config.h"
#include "worldgridmgr.h"
#include "game.h"

Game *game = NULL;

extern CoreRenderableListIndicies *__coreRenderableListIndicies;
extern CoreRenderableListIndicies *__coreGUIIndicies;
extern CoreRenderableListIndicies *__coreGUIFreeIndicies;

File __log;

void testButton( g2Controller *controller )
{
  __log.PrintInfo( Filelevel_DEBUG, "testButton: x%p", controller );
  //MessageBox( 0, "ok", 0, 0 );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
  game = new Game();
  Pos< Short> blocksPerGrid( 8, 8 );
  game->core->Init( 0, 0, false, "FlatGL" );
  game->core->keyboard.AddListener( Game::KeyboardProc );
  game->core->mouse.AddListener( Game::MouseKeyProc );
  game->core->mouse.AddMoveListener( Game::MouseMoveProc );
  Short gridsAroundObject = ( Short )  __config->GetNumber( "grids_preload_range", 0.0f );
  if( !gridsAroundObject )
    gridsAroundObject = ( Short ) Math::Ceil( 0.5f * ( ( __config->GetNumber( "gl_screen_width" ) + float( WORLD_GRID_BLOCK_SIZE ) ) / ( float( WORLD_GRID_BLOCK_SIZE ) * float( max( blocksPerGrid.x, blocksPerGrid.y ) ) ) ) );
  __log.PrintInfo( Filelevel_DEBUG, "gridsAroundObject: %d", gridsAroundObject );
  game->world = new WorldGridManager( game->core->GetRootObject(), gridsAroundObject );

  bool isDebug = __config->GetBoolean( "dbg_low_alpha" );
  Object *obj;
  RenderableQuad *quad;
  Collision *col;
  float worldAlpha = ( isDebug ? 0.1f : 1.0f );

  game->lua->Init();

  game->core->CreateObject( "gui" )->SetLockToDelete( true );

  game->world->LoadFromFile( "data/temp/testworld.fu" );

  game->core->mouse.SetCursor( "data/temp/cursor_hw.png", game->core->CreateObject( "mouse-cursor", game->core->GetObject( "gui" ) )->SetLockToDelete( true ) );
  //__log.PrintInfo( Filelevel_DEBUG, "test: %s", core->GetObject( "gui" )->GetNameFull().c_str() );
  //game->core->CreateObject( "bottom-block", game->core->GetObject( "gui" ) )->SetPosition( Vec3( 50.0f, 85.0f, 2.0f ) )->EnableRenderableGUI()->SetSize( Vec2( 100.0f, 30.0f ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha * 0.5f ) )
  //  ->SetTexture( "data/temp/ui-bg-0.png" );
  game->core->CreateObject( "tooltips" )->SetLockToDelete( true );
  game->core->CreateObject( "mouse-grid", game->core->GetObject( "tooltips" ) );
  game->core->CreateObject( "defaults" )->SetLockToDelete( true );
  game->core->CreateObject( "camera", game->core->GetObject( "defaults" ) )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  game->core->CreateObject( "active-object", game->core->GetObject( "defaults" ) )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  game->core->SetCamera( game->core->GetObject( "defaults/camera" ) );
  game->world->AddActiveObject( game->core->GetObject( "defaults/active-object" ) );

  obj = game->core->CreateObject( "test-bg-2" );
  obj->SetPosition( Vec3( 50.0f, 50.0f, -9.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/AUCHINDOUN_VORTEXCLOUD01.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 100.0f, 100.0f ) );


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

  /*
  obj = game->core->CreateObject( "wall-top" );
  obj->SetPosition( Vec3( 53.0f, 0.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 50.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 50.0f, 10.0f ) );
  game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
  obj = game->core->CreateObject( "wall-bottom" );
  obj->SetPosition( Vec3( 103.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 150.0f, 10.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 150.0f, 10.0f ) );
  game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
  obj = game->core->CreateObject( "wall-left" );
  obj->SetPosition( Vec3( 15.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 55.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 55.0f ) );
  ObjectTrigger* trig = obj->EnableTrigger()->SetSize( Vec3( 10.0f, 55.0f, 0.0f ) + Vec3( 2.0f, 2.0f, 0.0f ) );
  //trig->AddHandler( Game::TestTrigger );
  //game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
  obj = game->core->CreateObject( "wall-right" );
  obj->SetPosition( Vec3( 185.0f, 50.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/brick3.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  col = obj->EnableCollision();
  col->SetSize( Vec3( 10.0f, 50.0f, 0.0f ) );
  col->SetIsStatic( true );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 10.0f, 50.0f ) );
  game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
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
  game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
  obj = game->core->CreateObject( "enemy" );
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
  game->world->AttachObjectToGrid( 0, 0, obj );
  */

  /*
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
  */

  //game->world->AttachObjectToGrid( WorldGrid::WorldGridPosition( 0, 0 ), game->core->GetObject( "wall-left" ) );
  game->world->AddActiveObject( game->core->GetObject( "player" ) );


  /*
  for( int q = 0; q < 10; ++q )
  {
    char t[ 1024 ];
    sprintf_s( t, sizeof( t ), "block%d", q );
    Vec3 pos = Vec3( float( q ), -2.0f, 0.0f ) * float( WORLD_GRID_BLOCK_SIZE );
    obj = game->core->CreateObject( t );
    col = obj->EnableCollision();
    col->SetPosition( pos )->SetIsStatic( true )->SetSize( Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0.0f ) );
    quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
    quad->SetColor( Vec4( ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, ( rand() % 256 ) / 256.0f, 1.0f ) );
    quad->SetSize( Vec2( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE ) );
    quad->SetTexture( "data/temp/brick0.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
    game->world->AttachObjectToGrid( game->world->GetGridPositionByObject( *obj ), obj );
  }
  */

  //фоновая картинка

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
  wglSwapIntervalEXT( __config->GetBoolean( "gl_vsync" ) );
  char tempChar[ 1024 ];
  DWORD fps = 0, currentFps = 0;
  //__textureAtlas->__Dump( "data/temp/__atlas.tga" );

  //if edit-mode
  game->InitGui();

  sTimer.Update();
  game->world->Update( true );
  game->lua->RunFile( "data/scripts/game.lua" );
  __log.PrintInfo( Filelevel_INFO, "Run" );

  /*
  Font *font = game->core->CreateFont( "test-font-prop" );
  font->SetPosition2D( Vec2( 8.0f, 8.0f ) );
  font->SetRenderPipeline( __coreGUI, __coreGUIIndicies, __coreGUIFreeIndicies );
  font->SetFont( "data/temp/font_default.tga", "data/temp/font_default.flw", 1.0f )->SetScale( Vec2( 100.0f / float( game->core->GetWindowSize().width ), 100.0f / float( game->core->GetWindowSize().height ) ) );
  font->SetText( "Test message =\\" );

  Font *font2 = game->core->CreateFont( "test-font" );
  font2->SetPosition2D( Vec2( 8.0f, 20.0f ) );
  font2->SetRenderPipeline( __coreGUI, __coreGUIIndicies, __coreGUIFreeIndicies );
  font2->SetFont( "data/temp/font_default.tga" )->SetScale( Vec2( 100.0f / float( game->core->GetWindowSize().width ), 100.0f / float( game->core->GetWindowSize().height ) ) );
  font2->SetText( "Test message =\\" );
  */

  /*
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.01f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  */
  //g2Button* MyButton = game->core->gui->AddButton( 5, 5, "Hello, World!", testButton );
  //g2TextField *it = game->core->gui->AddTextField( 100, 100, "test" );
  //MyButton = game->core->gui->AddButton( 5, 30, "test2", testButton );

  while( game->core->Update() )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "=> core->Redraw" );
    game->core->Redraw();

    //__log.PrintInfo( Filelevel_DEBUG, "=> world->Update" );
    game->world->Update();
    //__log.PrintInfo( Filelevel_DEBUG, "=> game->Update" );
    game->Update();

    t += sTimer.GetDeltaF();
    rot += sTimer.GetDeltaF();
    if( t > 1.0f )
    {
      t -= 1.0f;
      fps = currentFps;
      currentFps = 0;
    }

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

    if( game->core->keyboard.IsPressed( VK_SPACE ) )
    {
      //font->SetText( "Omfg, is work ЭO_oE" );
    }

    /*
    if( game->core->mouse.IsPressed( VK_RBUTTON ) )
    {
      Vec3 pos( game->core->mouse.GetCursorPosition().x, game->core->mouse.GetCursorPosition().y, 0.0f );
      pos += game->core->GetCamera()->GetPosition() - Vec3( game->core->GetWindowHalfSize().x, game->core->GetWindowHalfSize().y, 0.0f );
      game->core->GetObject( "player" )->SetPosition( pos );
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

      game->world->AttachObjectToGrid( game->world->GetGridPositionByObject( *obj ), obj );
    }
    */

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

    /*
    if( obj = game->core->GetObject( "/lifts/lift" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 5.0f ) * 300.0f, 0.0f ) );
    if( obj = game->core->GetObject( "/lifts/lift-2" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 3.0f ) * 100.0f, 0.0f ) );
      */

    if( obj = game->core->GetObject( "test-bg-2" ) )
    {
      quad = ( RenderableQuad* ) obj->GetRenderable();
      if( quad ) {
        float scale = Math::Sin16( rot * 2.0f ) * 1.0f + 5.0f;
        quad->SetScale( Vec2( scale, scale ) )->SetRotation( -rot );
      }
    }
      //obj->SetPosition( Vec3( -20.0f, 50.0f + Math::Sin16( rot * 5.0f ) * 50.0f, 0.0f ) );

    ++currentFps;

    sprintf_s( tempChar, 1024, "FPS[%d] quads[%d/%d] grids[%d]", fps, __coreRenderableListIndicies->size(), __coreGUIIndicies->size(), __worldGridList->size() );
    game->core->SetWindowTitle( tempChar );
  }
  //MessageBox( NULL, "Ok", "Debug", MB_OK );

  game->world->SaveToFile( "data/temp/testworld.fu" );
  DEF_DELETE( game );

  return 0;
}



Game::Game()
{
  this->gridTooltipUnderMouse.show = false;
  memset( this->gridTooltipUnderMouse.obj, 0, sizeof( this->gridTooltipUnderMouse.obj ) );
  this->core = new Core();
  this->lua = new Lua();

  LUAFUNC_RemoveObject      = Game::LUA_RemoveObject;
  LUAFUNC_GetObjectPos      = Game::LUA_GetObjectPos;
  LUAFUNC_SetObjectPos      = Game::LUA_SetObjectPos;
  LUAFUNC_SetTimer          = Game::LUA_SetTimer;
  LUAFUNC_CreateObject      = Game::LUA_CreateObject;
  LUAFUNC_ListenKeyboard    = Game::LUA_ListenKeyboard;
  LUAFUNC_ListenMouseKey    = Game::LUA_ListenMouseKey;
  LUAFUNC_ListenMouseMove   = Game::LUA_ListenMouseMove;
  LUAFUNC_GameExit          = Game::LUA_GameExit;
  LUAFUNC_GetMousePos       = Game::LUA_GetMousePos;
  LUAFUNC_GetCameraPos      = Game::LUA_GetCameraPos;
  LUAFUNC_GetWindowSize     = Game::LUA_GetWindowSize;
  LUAFUNC_ObjectAddTrigger  = Game::LUA_ObjectAddTrigger;
  LUAFUNC_GuiAddTrigger     = Game::LUA_GuiAddTrigger;
  LUAFUNC_SetCamera         = Game::LUA_SetCamera;
  LUAFUNC_GetCamera         = Game::LUA_GetCamera;
  LUAFUNC_ClearScene        = Game::LUA_ClearScene;
  LUAFUNC_GuiGetText        = Game::LUA_GuiGetText;
  LUAFUNC_GuiSetText        = Game::LUA_GuiSetText;
  LUAFUNC_ObjectEnableRenderable  = Game::LUA_ObjectEnableRenderable;
  LUAFUNC_ObjectDisableRenderable = Game::LUA_ObjectDisableRenderable;
  LUAFUNC_ObjectEnableCollision   = Game::LUA_ObjectEnableCollision;
  LUAFUNC_ObjectDisableCollision  = Game::LUA_ObjectDisableCollision;
  LUAFUNC_ObjectEnableTrigger     = Game::LUA_ObjectEnableTrigger;
  LUAFUNC_ObjectDisableTrigger    = Game::LUA_ObjectDisableTrigger;
  LUAFUNC_GuiGetChecked     = Game::LUA_GuiGetChecked;
  LUAFUNC_GetCollisionStatic= Game::LUA_GetCollisionStatic;
  LUAFUNC_SetCollisionStatic= Game::LUA_SetCollisionStatic;
  LUAFUNC_DebugRender       = Game::LUA_DebugRender;
  LUAFUNC_GetObjectByPoint  = Game::LUA_GetObjectByPoint;
  LUAFUNC_SetGuiVisibility  = Game::LUA_SetGuiVisibility;
  LUAFUNC_SelectObject      = Game::LUA_SelectObject;
  LUAFUNC_GetSelectedObject = Game::LUA_GetSelectedObject;
  LUAFUNC_GuiAttr           = Game::LUA_GuiAttr;
  LUAFUNC_LoadScript        = Game::LUA_LoadScript;
  LUAFUNC_ObjectAttr        = Game::LUA_ObjectAttr;

  __ObjectTriggerOnRemoveGlobalHandler = Game::OnRemoveTrigger;
}//constructor

Game::~Game()
{
  this->core->Destroy();
  DEF_DELETE( this->lua );
  DEF_DELETE( this->world );
  DEF_DELETE( this->core );
}//destructor



/*
=============
  Update
=============
*/
void Game::Update()
{
  this->UpdateLuaTimers();
}//Update


/*
=============
  UpdateLuaTimers
=============
*/
void Game::UpdateLuaTimers()
{
  Dword q, count = this->luaTimers.size();
  GameLuaTimer *timer;
  for( q = 0; q < count; ++q )
  {
    timer = &this->luaTimers[ q ];
    if( timer->active )
    {
      timer->time -= sTimer.GetDeltaF();
      if( timer->time <= 0.0f )
      {
        timer->active = false;
        if( timer->funcName.length() )
          LUACALLBACK_Timer( this->lua, q, timer->funcName );
      }
    }
  }//for q
}//UpdateLuaTimers


/*
=============
  LUA_GetObjectPos
=============
*/
Vec2 Game::LUA_GetObjectPos( const std::string &name )
{
  Vec2 res;
  Object *obj = game->core->GetObject( name );
  if( obj )
    res.Set( obj->GetPosition().x, obj->GetPosition().y );
  return res;
}//LUA_GetObjectPos


/*
=============
  LUA_SetObjectPos
=============
*/
void Game::LUA_SetObjectPos( const std::string &name, const Vec2 &pos )
{
  Object *obj = game->core->GetObject( name );
  if( obj )
    obj->SetPosition2D( pos );
}//LUA_SetObjectPos


/*
=============
  LUA_GetCollisionStatic
=============
*/
bool Game::LUA_GetCollisionStatic( const std::string &name )
{
  Object *obj = game->core->GetObject( name );
  if( obj && obj->GetCollision() ) {
    return obj->GetCollision()->IsStatic();
  }
  return false;
}//LUA_GetCollisionStatic


/*
=============
  LUA_SetCollisionStatic
=============
*/
void Game::LUA_SetCollisionStatic( const std::string &name, bool isStatic )
{
  Object *obj = game->core->GetObject( name );
  if( obj && obj->GetCollision() ) {
    obj->GetCollision()->SetIsStatic( isStatic );
  }
}//LUA_SetCollisionStatic


/*
=============
  LUA_SetTimer
=============
*/
Dword Game::LUA_SetTimer( float time, const std::string &funcName )
{
  Dword id, count = game->luaTimers.size();
  bool setted = false;
  for( Dword q = 0; q < count; ++q )
    if( !game->luaTimers[ q ].active )
    {
      id = q;
      setted = true;
      break;
    }
  if( !setted )
  {
    game->luaTimers.push_back( GameLuaTimer() );
    id = game->luaTimers.size() - 1;
  }
  game->luaTimers[ id ].active    = 1;
  game->luaTimers[ id ].time      = time;
  game->luaTimers[ id ].funcName  = funcName;
  return id;
}//LUA_SetTimer



/*
=============
  LUA_RemoveObject
=============
*/
bool Game::LUA_RemoveObject( const std::string &name )
{
  return game->core->RemoveObject( name );
}//LUA_RemoveObject


/*
=============
  LUA_CreateObject
=============
*/
void Game::LUA_CreateObject( const std::string &name, const Vec3 &pos )
{
    __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_CreateObject..." );
  long slashPos = name.find_last_of( "/" );
  Object *parentObj = NULL;
  Object *obj       = NULL;
  if( slashPos > 0 ) {
    std::string fullPath = &name[ slashPos ];
    std::deque< std::string > path = tools::Explode( name, "/" );
    std::deque< std::string >::iterator iter, iterEnd = path.end();
    std::string currentPath = "";
    for( iter = path.begin(); iter != iterEnd; ++iter ) {
      __log.PrintInfo( Filelevel_DEBUG, ". iter: '%s'", iter->c_str() );
      obj = game->core->CreateObject( *iter, ( currentPath.length() ? game->core->GetObject( currentPath ) : NULL ) );
      obj->SetPosition( pos );
      currentPath += ( currentPath.length() ? "/" : "" ) + *iter;
      game->world->AttachObjectToGrid( game->world->GetGridPositionByObject( *obj ), obj );
    }
    //std::string nextLevel = &name[ slashPos ];
    //std::string currentLevel = name.substr( 0, slashPos );
    //parentObj = game->core->GetObject( currentLevel );
  } else {
    obj = game->core->CreateObject( name );
    obj->SetPosition( pos );
    game->world->AttachObjectToGrid( game->world->GetGridPositionByObject( *obj ), obj );
  }
  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_CreateObject => object['%s'] parent['%s']", obj->GetNameFull().c_str(), obj->GetParentNameFull().c_str() );

  //Object *parentObj = ( parentName.length() ? game->core->GetObject( parentName ) : NULL );
  //Object *obj = game->core->CreateObject( name, parentObj );
  /*
  Collision *col = obj->EnableCollision();
  col->SetPosition( pos )->SetIsStatic( true )->SetSize( Vec3( size.x, size.y, 0.0f ) );
  RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( color );
  quad->SetSize( Vec2( size.x, size.y ) );
  quad->SetTexture( textureName, Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  */

}//LUA_CreateObject



/*
=============
  LUA_ObjectEnableRenderable
=============
*/
void Game::LUA_ObjectEnableRenderable( const std::string &objectName, const std::string &texture, const Vec2 &size, const Vec4 &color )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectEnableRenderable => object '%s' not found", objectName.c_str() );
    return;
  }
  RenderableQuad *render = ( RenderableQuad* ) object->EnableRenderable( RENDERABLE_TYPE_QUAD );
  render->SetSize( size );
  render->SetColor( color );
  render->SetTexture( texture );
}//LUA_ObjectEnableRenderable



/*
=============
  LUA_ObjectDisableRenderable
=============
*/
void Game::LUA_ObjectDisableRenderable( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectDisableRenderable => object '%s' not found", objectName.c_str() );
    return;
  }
  object->DisableRenderable();
}//LUA_ObjectDisableRenderable



/*
=============
  LUA_ObjectEnableCollision
=============
*/
void Game::LUA_ObjectEnableCollision( const std::string &objectName, bool isStatic, const Vec3 &size, const Vec3 &velocity, const Vec3 &acceleration )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectEnableCollision => object '%s' not found", objectName.c_str() );
    return;
  }
  Collision *collision = object->EnableCollision();
  collision->SetIsStatic( isStatic );
  collision->SetSize( size );
  collision->SetVelocity( velocity );
  collision->SetAcceleration( acceleration );
}//LUA_ObjectEnableCollision



/*
=============
  LUA_ObjectDisableCollision
=============
*/
void Game::LUA_ObjectDisableCollision( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectDisableCollision => object '%s' not found", objectName.c_str() );
    return;
  }
  object->DisableCollision();
}//LUA_ObjectDisableCollision



/*
=============
  LUA_ObjectEnableTrigger
=============
*/
void Game::LUA_ObjectEnableTrigger( const std::string &objectName, const Vec3 &size )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectEnableTrigger => object '%s' not found", objectName.c_str() );
    return;
  }
  ObjectTrigger *trigger = object->EnableTrigger();
  trigger->SetSize( size );
}//LUA_ObjectEnableTrigger



/*
=============
  LUA_ObjectDisableTrigger
=============
*/
void Game::LUA_ObjectDisableTrigger( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectDisableTrigger => object '%s' not found", objectName.c_str() );
    return;
  }
  object->DisableTrigger();
}//LUA_ObjectDisableTrigger




/*
=============
  LUA_ListenKeyboard
=============
*/
void Game::LUA_ListenKeyboard( const std::string &funcName )
{
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaKeyboardListeners.end();
  for( iter = game->luaKeyboardListeners.begin(); iter != iterEnd; ++iter )
    if( *iter == funcName )
      return;
  game->luaKeyboardListeners.push_back( funcName );
}//LUA_ListenKeyboard


/*
=============
  LUA_ListenMouseKey
=============
*/
void Game::LUA_ListenMouseKey( const std::string &funcName )
{
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseKeyListeners.end();
  for( iter = game->luaMouseKeyListeners.begin(); iter != iterEnd; ++iter )
    if( *iter == funcName )
      return;
  game->luaMouseKeyListeners.push_back( funcName );
}//LUA_ListenMouseKey


/*
=============
  LUA_ListenMouseMove
=============
*/
void Game::LUA_ListenMouseMove( const std::string &funcName )
{
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseMoveListeners.end();
  for( iter = game->luaMouseMoveListeners.begin(); iter != iterEnd; ++iter ) {
    if( *iter == funcName ) {
      return;
    }
  }
  game->luaMouseMoveListeners.push_back( funcName );
}//LUA_ListenMouseMove



/*
=============
  GetGridPosUnderCamera
=============
*/
Vec3 Game::GetGridPosUnderCamera( float scale )
{
  Vec3 pos( this->core->GetCamera()->GetPosition() );
  pos += Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0 ) * scale * 0.5f;
  pos /= float( WORLD_GRID_BLOCK_SIZE ) * scale;
  pos = Vec3( Math::Floor( pos.x ), Math::Floor( pos.y ), 0.0f );
  return pos;
}//GetGridPosUnderCamera


/*
=============
  GetGridPosUnderCursor
=============
*/
Vec3 Game::GetGridPosUnderCursor()
{
  Vec3 pos( this->core->mouse.GetCursorPosition().x, this->core->mouse.GetCursorPosition().y, 0.0f );
  pos += ( this->core->GetCamera()->GetPosition() - Vec3( this->core->GetWindowHalfSize().x, this->core->GetWindowHalfSize().y, 0.0f ) ) + Vec3( WORLD_GRID_BLOCK_SIZE, WORLD_GRID_BLOCK_SIZE, 0 ) * 0.5f;
  pos /= float( WORLD_GRID_BLOCK_SIZE );
  pos = Vec3( Math::Floor( pos.x ), Math::Floor( pos.y ), 0.0f );
  return pos;
}//GetGridPosUnderCursor


/*
=============
  KeyboardProc
=============
*/
void Game::KeyboardProc( Dword keyId, bool isPressed )
{
  if( !game->luaKeyboardListeners.size() )
    return;
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaKeyboardListeners.end();
  for( iter = game->luaKeyboardListeners.begin(); iter != iterEnd; ++iter )
    LUACALLBACK_ListenKeyboard( game->lua, *iter, keyId, isPressed );
}//KeyboardProc


/*
=============
  MouseKeyProc
=============
*/
void Game::MouseKeyProc( Dword keyId, bool isPressed )
{
  if( !game->luaMouseKeyListeners.size() )
    return;
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseKeyListeners.end();
  for( iter = game->luaMouseKeyListeners.begin(); iter != iterEnd; ++iter )
    LUACALLBACK_ListenMouseKey( game->lua, *iter, keyId, isPressed );
}//MouseKeyProc


/*
=============
  MouseMoveProc
=============
*/
void Game::MouseMoveProc( const Vec2 &pos )
{
  if( !game->luaMouseMoveListeners.size() )
    return;
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseMoveListeners.end();
  for( iter = game->luaMouseMoveListeners.begin(); iter != iterEnd; ++iter )
    LUACALLBACK_ListenMouseMove( game->lua, *iter, pos );
}//MouseMoveProc


/*
=============
  LUA_GameExit
=============
*/
void Game::LUA_GameExit()
{
  game->core->SetState( CORE_STATE_EXIT );
}//LUA_GameExit


/*
=============
  LUA_GetMousePos
=============
*/
Vec2 Game::LUA_GetMousePos()
{
  return game->core->mouse.GetCursorPosition();
}//LUA_GetMousePos


/*
=============
  LUA_GetCameraPos
=============
*/
Vec2 Game::LUA_GetCameraPos()
{
  Object *camera = game->core->GetCamera();
  if( camera )
    return Vec2( camera->GetPosition().x, camera->GetPosition().y );
  return Vec2Null;
}//LUA_GetCameraPos


/*
=============
  LUA_GetWindowSize
=============
*/
Size Game::LUA_GetWindowSize()
{
  return game->core->GetWindowSize();
}//LUA_GetWindowSize



/*
=============
  LUA_ObjectTrigger_Handler
  ObjectTrigger call this (ObjectTriggerHandler)
=============
*/
void Game::LUA_ObjectTrigger_Handler( ObjectTrigger *trigger, Collision *collision, bool isInTrigger )
{
  GameObjectTriggerList::iterator iter, iterEnd = game->objectTriggers.end();
  for( iter = game->objectTriggers.begin(); iter != iterEnd; ++iter )
    if( iter->trigger == trigger )
      LUACALLBACK_ObjectTrigger( game->lua, iter->funcName, game->core->GetObjectByTrigger( iter->trigger )->GetNameFull(), game->core->GetObjectByCollision( collision )->GetNameFull(), isInTrigger );
}



/*
=============
  LUA_GuiTrigger_Handler
  ObjectTrigger call this (ObjectTriggerHandler)
=============
*/
void Game::LUA_GuiTrigger_Handler( Object *guiObject )
{
  GuiTriggerList::iterator iter, iterEnd = game->guiTriggers.end();
  for( iter = game->guiTriggers.begin(); iter != iterEnd; ++iter )
    if( iter->object == guiObject )
      LUACALLBACK_GuiTrigger( game->lua, iter->funcName, guiObject->GetNameFull() );
}



/*
=============
  LUA_ObjectAddTrigger
=============
*/
void Game::LUA_ObjectAddTrigger( const std::string &triggerName, const std::string &funcName )
{
  Object *triggerObject = game->core->GetObject( triggerName );
  if( !triggerObject )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAddTrigger => trigger '%s' not found", triggerName.c_str() );
    return;
  }

  ObjectTrigger *trigger = triggerObject->GetTrigger();
  if( !trigger )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAddTrigger => object '%s' don't have trigger", triggerName.c_str() );
    return;
  }

  trigger->AddHandler( Game::LUA_ObjectTrigger_Handler );

  //GameObjectTrigger objectTrigger;
  //objectTrigger.funcName = funcName;
  //objectTrigger.trigger = game->core->GetObject( triggerName );
  //game->core->SetState( CORE_STATE_EXIT );
  game->objectTriggers.push_back( GameObjectTrigger() );
  GameObjectTrigger *objectTrigger = &( *game->objectTriggers.rbegin() );
  objectTrigger->funcName = funcName;
  objectTrigger->trigger  = trigger;

  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ObjectAddTrigger => triggerName[%s] funcName[%s]", triggerName.c_str(), funcName.c_str() );
}//LUA_ObjectAddTrigger



/*
=============
  LUA_GuiAddTrigger
=============
*/
void Game::LUA_GuiAddTrigger( const std::string &objectName, const std::string &funcName )
{
  Object *guiObject = game->core->GetObject( objectName );
  if( !guiObject )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GuiAddTrigger => gui element '%s' not found", objectName.c_str() );
    return;
  }

  /*
  trigger->AddHandler( Game::LUA_ObjectTrigger_Handler );
  */
  game->guiTriggers.push_back( GuiTrigger() );
  GuiTrigger *guiTrigger = &( *game->guiTriggers.rbegin() );
  guiTrigger->funcName  = funcName;
  guiTrigger->object    = guiObject;

  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAddTrigger => guiName[%s] funcName[%s]", objectName.c_str(), funcName.c_str() );
}//LUA_GuiAddTrigger



/*
=============
  OnRemoveTrigger
  Подчищаем в игре всё, что связано с триггерами
=============
*/
void Game::OnRemoveTrigger( ObjectTrigger *trigger )
{
  GameObjectTriggerList::iterator iter, iterEnd = game->objectTriggers.end();
  for( iter = game->objectTriggers.begin(); iter != iterEnd; ++iter )
    if( ( *iter ).trigger == trigger )
    {
      game->objectTriggers.erase( iter );
      break;
    }
}//OnRemoveTrigger


/*
=============
  LUA_SetCamera
=============
*/
void Game::LUA_SetCamera( const std::string &name )
{
  Object *camera = game->core->GetObject( name );
  if( !camera )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_SetCamera => object '%s' not found", name.c_str() );
    return;
  }
  game->core->SetCamera( camera );
}//LUA_CreateObject


/*
=============
  LUA_GetCamera
=============
*/
std::string Game::LUA_GetCamera()
{
  Object *camera = game->core->GetCamera();
  if( !camera )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GetCamera => camera not attached to object" );
    return "";
  }
  return camera->GetNameFull();
}//LUA_GetCamera


/*
=============
  LUA_ClearScene
=============
*/
void Game::LUA_ClearScene()
{
  /*
    1. Чистим потомков core->root
    2. Чистим ссылки на объекты в мирах __worldGridList (ObjectPointer)
      
  */
  __log.PrintInfo( Filelevel_DEBUG, "LUA_ClearScene core" );
  game->core->ClearScene();
  __log.PrintInfo( Filelevel_DEBUG, "LUA_ClearScene world" );
  game->world->ClearWorld();
  __log.PrintInfo( Filelevel_DEBUG, "LUA_ClearScene done" );
}//LUA_ClearScene



/*
=============
  GuiProc
  Обработка событий всяких gui-объектов
=============
*/
void Game::GuiProc( Object *obj )
{
  LUA_GuiTrigger_Handler( obj );
  /*
  __log.PrintInfo( Filelevel_DEBUG, "Game::GuiProc => triggers[%d]", game->guiTriggers.size() );
  GuiTriggerList::iterator iter, iterEnd = game->guiTriggers.end();
  for( iter = game->guiTriggers.begin(); iter != iterEnd; ++iter ) {
    if( iter->object == obj ) {
      Game::LUA_GuiTrigger_Handler( obj );
    }
  }
  */
}//GuiProc



/*
=============
  LUA_GuiGetText
=============
*/
std::string Game::LUA_GuiGetText( const std::string &guiName )
{
  Object *object = game->core->GetObject( guiName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetText => gui element '%s' not found", guiName.c_str() );
    return "";
  }

  g2Controller *controller = object->GetGui();
  if( !controller ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetText => object '%s' is not gui element", guiName.c_str() );
    return "";
  }

  switch( object->GetGuiType() ) {
    case OBJECT_GUI_TEXTFIELD: {
      std::string str = ( ( g2TextField* ) controller )->GetText();
      return str;
    }
    case OBJECT_GUI_SPINNER: {
      Variable v;
      v.SetNumber( ( ( g2Spinner* ) controller )->GetFloat() );
      std::string str = v.GetString();
      return str;
    }
    case OBJECT_GUI_DROPDOWN: {
      dropDownListsType::iterator iter = game->dropDownLists.find( controller );
      std::string str = "";
      if( iter != game->dropDownLists.end() ) {
        str = iter->second[ ( ( g2DropDown* ) controller )->GetSelectionIndex() ];
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GuiGetText => controller not found" );
      }
      return str;
    }
  }//switch

  __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetText => object '%s' don't have text", guiName.c_str() );
  return "";
}//LUA_GuiGetText



/*
=============
  LUA_GuiSetText
=============
*/
void Game::LUA_GuiSetText( const std::string &guiName, const std::string &text )
{
  Object *object = game->core->GetObject( guiName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiSetText => gui element '%s' not found", guiName.c_str() );
    return;
  }

  g2Controller *controller = object->GetGui();
  if( !controller ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiSetText => object '%s' is not gui element", guiName.c_str() );
    return;
  }

  switch( object->GetGuiType() ) {
    case OBJECT_GUI_TEXTFIELD: {
      ( ( g2TextField* ) controller )->SetText( text.c_str() );
      return;
    }
    case OBJECT_GUI_SPINNER: {
      Variable v;
      v.SetNumber( text );
      ( ( g2Spinner* ) controller )->SetFloat( v.GetNumber() );
      return;
    }
    case OBJECT_GUI_DROPDOWN: {
      dropDownListsType::iterator iter = game->dropDownLists.find( controller );
      std::string str = "";
      if( iter != game->dropDownLists.end() ) {
        stdDequeString::iterator iterStr, iterStrEnd = iter->second.end();
        int index = 0;
        for( iterStr = iter->second.begin(); iterStr != iterStrEnd; ++iterStr, ++index ) {
          if( *iterStr == text ) {
            ( ( g2DropDown* ) controller )->SetSelectionIndex( index );
            break;
          }
        }
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GuiSetText => controller not found" );
      }
      return;
    }
  }//switch

  __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiSetText => object '%s' don't have text", guiName.c_str() );
}//LUA_GuiSetText



/*
=============
  LUA_GuiGetChecked
=============
*/
bool Game::LUA_GuiGetChecked( const std::string &guiName )
{
  Object *object = game->core->GetObject( guiName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetChecked => gui element '%s' not found", guiName.c_str() );
    return false;
  }

  g2Controller *controller = object->GetGui();
  if( !controller ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetChecked => object '%s' is not gui element", guiName.c_str() );
    return false;
  }

  switch( object->GetGuiType() ) {
    case OBJECT_GUI_CHECKBOX: {
      bool checked = ( ( g2CheckBox* ) controller )->IsChecked();
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiGetChecked => object '%s': %d", guiName.c_str(), checked );
      return checked;
    }
  }//switch

  __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiGetChecked => object '%s' is not checkbox", guiName.c_str() );
  return false;
}//LUA_GuiGetChecked


/*
=============
  LUA_DebugRender
=============
*/
void Game::LUA_DebugRender( int flags )
{
  game->core->debug.renderRenderable  = ( ( flags&1 ) == 1 );
  game->core->debug.renderCollision   = ( ( flags&2 ) == 2 );
  game->core->debug.renderTrigger     = ( ( flags&4 ) == 4 );
}//LUA_DebugRender


/*
=============
  LUA_GetObjectByPoint
=============
*/
std::string Game::LUA_GetObjectByPoint( int type, const Vec2 &point, const std::string &afterObject )
{
  __log.PrintInfo( Filelevel_DEBUG, "test123: point[%3.3f; %3.3f]", point.x, point.y );
  switch( type ) {
    case 1: { //renderable
      Object *object = game->core->GetRenderableInPoint( point, afterObject );
      if( object ) {
        return object->GetNameFull();
      } else {
        if( afterObject.length() ) {
          object = game->core->GetRenderableInPoint( point, "" );
          if( object ) {
            return object->GetNameFull();
          }
        }
      }
      break;
    }//renderable
    case 2: { //collision
      Object *object = game->core->GetCollisionInPoint( point, afterObject );
      if( object ) {
        return object->GetNameFull();
      } else {
        if( afterObject.length() ) {
          object = game->core->GetCollisionInPoint( point, "" );
          if( object ) {
            return object->GetNameFull();
          }
        }
      }
      break;
    }//collision
    case 3: { //trigger
      Object *object = game->core->GetTriggerInPoint( point, afterObject );
      if( object ) {
        return object->GetNameFull();
      } else {
        if( afterObject.length() ) {
          object = game->core->GetTriggerInPoint( point, "" );
          if( object ) {
            return object->GetNameFull();
          }
        }
      }
      break;
    }//trigger
  }//
  return "";
}//LUA_GetObjectByPoint


/*
=============
  LUA_SetGuiVisibility
=============
*/
void Game::LUA_SetGuiVisibility( int show )
{
  game->core->SetGuiVisibility( show ? true : false );
}//LUA_SetGuiVisibility


/*
=============
  LUA_SelectObject
=============
*/
void Game::LUA_SelectObject( const std::string &name )
{
  if( !name.length() ) {
    game->core->debug.selectedObject = NULL;
  } else {
    Object *object = game->core->GetObject( name );
    if( !object ) {
      __log.PrintInfo( Filelevel_ERROR, "Game::LUA_SelectObject => object '%s' not found", name.c_str() );
    } else {
      game->core->debug.selectedObject = object;
    }
  }
}//LUA_SelectObject


/*
=============
  LUA_GetSelectedObject
=============
*/
std::string Game::LUA_GetSelectedObject()
{
  return ( game->core->debug.selectedObject ? game->core->debug.selectedObject->GetNameFull() : "" );
}//LUA_GetSelectedObject



/*
=============
  LUA_GuiAttr
=============
*/
void Game::LUA_GuiAttr( const std::string &guiName, const std::string &parameter, Variable &value, bool isSet )
{
  Object *object = game->core->GetObject( guiName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiAttr => gui element '%s' not found", guiName.c_str() );
    return;
  }

  g2Controller *controller = object->GetGui();
  if( !controller ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_GuiAttr => object '%s' is not gui element", guiName.c_str() );
    return;
  }

  if( parameter == "val" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set val" );
      switch( object->GetGuiType() ) {
        case OBJECT_GUI_BUTTON: {
          g2Button *item = ( g2Button* ) controller;
          item->SetText( value.GetString().c_str() );
          break;
        }
        case OBJECT_GUI_TEXTFIELD: {
          g2TextField *item = ( g2TextField* ) controller;
          item->SetText( value.GetString().c_str() );
          break;
        }
        case OBJECT_GUI_CHECKBOX: {
          g2CheckBox *item = ( g2CheckBox* ) controller;
          item->SetChecked( value.GetBoolean() );
          break;
        }
        case OBJECT_GUI_DROPDOWN: {
          dropDownListsType::iterator iter = game->dropDownLists.find( controller );
          std::string str = "", text = value.GetString();
          if( iter != game->dropDownLists.end() ) {
            stdDequeString::iterator iterStr, iterStrEnd = iter->second.end();
            int index = 0;
            for( iterStr = iter->second.begin(); iterStr != iterStrEnd; ++iterStr, ++index ) {
              if( *iterStr == text ) {
                ( ( g2DropDown* ) controller )->SetSelectionIndex( index );
                break;
              }
            }
          } else {
            __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GuiAttr => controller not found" );
          }
          break;
        }
        case OBJECT_GUI_LABEL: {
          g2Label *item = ( g2Label* ) controller;
          item->SetText( value.GetString().c_str() );
          break;
        }
        case OBJECT_GUI_PANEL: {
          g2Panel *item = ( g2Panel* ) controller;
          item->SetTitle( value.GetString().c_str() );
          break;
        }
      }
    } else {  //get
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get val" );
      switch( object->GetGuiType() ) {
        case OBJECT_GUI_BUTTON: {
          value.SetString( ( ( g2Button* ) controller )->GetLabel()->GetText() );
          break;
        }
        case OBJECT_GUI_TEXTFIELD: {
          value.SetString( ( ( g2TextField* ) controller )->GetText() );
          break;
        }
        case OBJECT_GUI_CHECKBOX: {
          value.SetBoolean( ( ( g2CheckBox* ) controller )->IsChecked() );
          break;
        }
        case OBJECT_GUI_LABEL: {
          value.SetString( ( ( g2Label* ) controller )->GetText() );
          break;
        }
        case OBJECT_GUI_PANEL: {
          value.SetString( ( ( g2Panel* ) controller )->GetTitle()->GetText() );
          break;
        }
        case OBJECT_GUI_DROPDOWN: {
          dropDownListsType::iterator iter = game->dropDownLists.find( controller );
          if( iter != game->dropDownLists.end() ) {
            value.SetString( iter->second[ ( ( g2DropDown* ) controller )->GetSelectionIndex() ] );
          } else {
            value.SetString( "" );
            __log.PrintInfo( Filelevel_ERROR, "Game::LUA_GuiAttr => controller not found" );
          }
          break;
        }
      }
    }//get
    //val
  } else if( parameter == "enabled" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set enabled" );
      controller->SetDisabled( !value.GetBoolean( false ) );
    } else {  //get
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get enabled" );
      value.SetBoolean( !controller->GetDisabled() );
    }
    //enabled
  } else if( parameter == "disabled" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set disabled" );
      controller->SetDisabled( value.GetBoolean( false ) );
    } else {  //get
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get disabled" );
      value.SetBoolean( controller->GetDisabled() );
    }
    //disabled
  } else if( parameter == "visible" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set visible" );
      controller->SetVisibility( value.GetBoolean( false ) );
    } else {  //get
      value.SetBoolean( controller->GetVisibility() );
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get visible" );
    }
    //visible
  } else if( parameter == "active" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set active" );
    } else {  //get
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get active" );
      value.SetBoolean( controller->GetActive() );
    }
    //active
  } else if( parameter == "checked" ) {
    if( isSet ) { //set
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => set checked" );
      switch( object->GetGuiType() ) {
        case OBJECT_GUI_CHECKBOX: {
          ( ( g2CheckBox* ) controller )->SetChecked( value.GetBoolean() );
          break;
        }
      }
    } else {  //get
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GuiAttr => get checked" );
      switch( object->GetGuiType() ) {
        case OBJECT_GUI_CHECKBOX: {
          value.SetBoolean( ( ( g2CheckBox* ) controller )->IsChecked() );
          break;
        }
        default: {
        }
      }
    }
    //checked
  }
}//LUA_GuiAttr

/*
=============
  LUA_LoadScript
=============
*/
void Game::LUA_LoadScript( const std::string &fileName )
{
  game->lua->RunFile( fileName );
}//LUA_LoadScript


/*
=============
  LUA_ObjectAttr
=============
*/
void Game::LUA_ObjectAttr( const std::string &objectName, VariableAttributesList &setAttributes, VariableAttributesList &getAttributes )
{
  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ObjectAttr: %d", setAttributes.size() );

  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectAttr => object '%s' not found", objectName.c_str() );
    return;
  }

  VariableAttributesList::iterator iter, iterEnd = setAttributes.end();
  RenderableQuad  *renderable;
  Collision       *collision;
  ObjectTrigger   *trigger;
  //set
  for( iter = setAttributes.begin(); iter != iterEnd; ++iter ) {
    __log.PrintInfo( Filelevel_DEBUG, ". set %s = '%s'", ( *iter )->name.c_str(), ( *iter )->value.GetString().c_str() );

    const std::string &name = ( *iter )->name;
    const Variable &value   = ( *iter )->value;

    //object
    if( name == "renderable" ) {
      if( value.GetBoolean() ) {
        __log.PrintInfo( Filelevel_DEBUG, ". EnableRenderable['%s']", object->GetNameFull().c_str() );
        object->EnableRenderable( RENDERABLE_TYPE_QUAD );
      } else {
        __log.PrintInfo( Filelevel_DEBUG, ". DisableRenderable['%s']", object->GetNameFull().c_str() );
        object->DisableRenderable();
      }
      continue;
    }//renderable
    if( name == "collision" ) {
      if( value.GetBoolean() ) {
        __log.PrintInfo( Filelevel_DEBUG, ". EnableCollision['%s']", object->GetNameFull().c_str() );
        object->EnableCollision();
      } else {
        __log.PrintInfo( Filelevel_DEBUG, ". DisableCollision['%s']", object->GetNameFull().c_str() );
        object->DisableCollision();
      }
      continue;
    }//collision
    if( name == "trigger" ) {
      if( value.GetBoolean() ) {
        __log.PrintInfo( Filelevel_DEBUG, ". EnableTrigger['%s']", object->GetNameFull().c_str() );
        object->EnableTrigger();
      } else {
        __log.PrintInfo( Filelevel_DEBUG, ". DisableTrigger['%s']", object->GetNameFull().c_str() );
        object->DisableTrigger();
      }
      continue;
    }//trigger
    if( name == "position" ) {
      Vec3 pos = object->GetPosition();
      sscanf_s( value.GetString().c_str(), "%f %f", &pos.x, &pos.y );
      object->SetPosition( pos );
      continue;
    }//position

    //renderable
    if( name == "textureName" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      std::string textureFileName = std::string( FileManager_DATADIRECTORY ) + "/" + value.GetString();
      if( renderable && __fileManager->FileExists( textureFileName ) ) {
        renderable->SetTexture( textureFileName );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => texture '%s' not found or object not renderable", textureFileName.c_str() );
      }
      continue;
    }//textureName
    if( name == "renderableSize" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec2 size( 1.0f, 1.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        renderable->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableSize: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableSize
    if( name == "renderablePosition" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec3 pos( 0, 0, 0 );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &pos.x, &pos.y );
        if( res < 2 ) {
          pos.y = pos.x;
        }
        renderable->SetPosition( pos );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderablePosition: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderablePosition
    if( name == "color" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f %f %f", &color.x, &color.y, &color.z, &color.w );
        if( res < 4 ) {
          color.y = color.z = color.w = color.x;
        }
        renderable->SetColor( color );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => color: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//color
    if( name == "renderableScale" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        Vec2 scale( 1.0f, 1.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &scale.x, &scale.y );
        if( res < 2 ) {
          scale.y = scale.x;
        }
        renderable->SetScale( scale );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableScale: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableScale
    if( name == "renderableRotation" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        float angle = value.GetNumber();
        renderable->SetRotation( DEG2RAD( angle ) );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => renderableRotation: object '%s' not renderable", object->GetNameFull().c_str() );
      }
      continue;
    }//renderableRotation

    //collision
    if( name == "collisionSize" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 size( 1.0f, 1.0f, 0.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        collision->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionSize: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionSize
    if( name == "collisionAcceleration" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec( 0.0f, 0.0f, 0.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetAcceleration( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionAcceleration: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionAcceleration
    if( name == "collisionVelocity" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec( 0.0f, 0.0f, 0.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetVelocity( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionVelocity: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionVelocity
    if( name == "collisionStatic" ) {
      collision = object->GetCollision();
      if( collision ) {
        collision->SetIsStatic( value.GetBoolean( true ) );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionStatic: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionStatic
    if( name == "collisionForce" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 vec = collision->GetForce();
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &vec.x, &vec.y );
        if( res < 2 ) {
          vec.y = vec.x;
        }
        collision->SetForce( vec );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => collisionForce: object '%s' not collision", object->GetNameFull().c_str() );
      }
      continue;
    }//collisionForce

    //trigger
    if( name == "triggerSize" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 size( 1.0f, 1.0f, 0.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &size.x, &size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        trigger->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => triggerSize: object '%s' not triger", object->GetNameFull().c_str() );
      }
      continue;
    }//triggerSize

  }//for iter

  //get
  iterEnd = getAttributes.end();
  for( iter = getAttributes.begin(); iter != iterEnd; ++iter ) {
    __log.PrintInfo( Filelevel_DEBUG, ". get '%s'", ( *iter )->name.c_str() );

    const std::string &name = ( *iter )->name;
    Variable &value         = ( *iter )->value;

    //object
    if( name == "renderable" ) {
      value.SetBoolean( object->IsRenderable() );
      continue;
    }//renderable
    if( name == "collision" ) {
      value.SetBoolean( object->IsCollision() );
      continue;
    }//collision
    if( name == "trigger" ) {
      value.SetBoolean( object->IsTrigger() );
      continue;
    }//trigger
    if( name == "position" ) {
      value.SetVec3( object->GetPosition() );
      continue;
    }//position

    //renderable
    if( name == "textureName" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const std::string &texture = renderable->GetTextureFileName();
        value.SetString( texture.length() ? &renderable->GetTextureFileName()[ strlen( FileManager_DATADIRECTORY ) + 1 ] : texture );
      }
      continue;
    }//textureName
    if( name == "renderableSize" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec2( renderable->GetSize() );
      }
      continue;
    }//renderableSize
    if( name == "renderablePosition" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const Vec3 &pos = renderable->GetPosition();
        value.SetVec2( Vec2( pos.x, pos.y ) );
      }
      continue;
    }//renderablePosition
    if( name == "color" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec4( renderable->GetColor() );
      }
      continue;
    }//color
    if( name == "renderableScale" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec2( renderable->GetScale() );
      }
      continue;
    }//renderableScale
    if( name == "renderableRotation" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetNumber( renderable->GetRotation() );
      }
      continue;
    }//renderableRotation

    //collision
    if( name == "collisionSize" ) {
      collision = object->GetCollision();
      if( collision ) {
        const Vec3 &size = collision->GetSize();
        value.SetVec2( Vec2( size.x, size.y ) );
      }
      continue;
    }//collisionSize
    if( name == "collisionAcceleration" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 acceleration = collision->GetAcceleration();
        value.SetVec2( Vec2( acceleration.x, acceleration.y ) );
      }
      continue;
    }//collisionAcceleration
    if( name == "collisionVelocity" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 velocity = collision->GetVelocity();
        value.SetVec2( Vec2( velocity.x, velocity.y ) );
      }
      continue;
    }//collisionVelocity
    if( name == "collisionStatic" ) {
      collision = object->GetCollision();
      if( collision ) {
        value.SetBoolean( collision->IsStatic() );
      }
      continue;
    }//collisionStatic
    if( name == "collisionForce" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 force = collision->GetForce();
        value.SetVec2( Vec2( force.x, force.y ) );
      }
      continue;
    }//collisionForce

    //trigger
    if( name == "triggerSize" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 size = trigger->GetSize();
        value.SetVec2( Vec2( size.x, size.y ) );
      }
      continue;
    }//triggerSize

  }
}//LUA_ObjectAttr


/*
=============
  InitGui
=============
*/
void Game::InitGui() {
  Object *obj;

  Object *guiRoot = game->core->CreateObject( "editor" );
  int windowWidth = game->core->GetWindowSize().width,
      width = 200,
      left = windowWidth - width,
      y = 0
      ;
  Object::GuiConstructor gui;

  obj = game->core->CreateObject( "debug", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_TEXTFIELD;
  gui.position.x = 0;
  gui.position.y = 0;
  gui.width = 200;
  gui.label = "";
  obj->EnableGui( &gui );

  //panels
  obj = game->core->CreateObject( "object.window_main", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_PANEL;
  gui.panelAnchor = g2Anchor_None;
  gui.label = "Settings";
  gui.position.x = left;
  gui.position.y = y;
  gui.width = width;
  gui.height = 145;
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "object.window_object", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_PANEL;
  gui.panelAnchor = g2Anchor_None;
  gui.label = "Object";
  gui.position.x = left;
  gui.position.y = 150;
  gui.width = width;
  gui.height = 95;
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "object.window_renderable", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_PANEL;
  gui.panelAnchor = g2Anchor_None;
  gui.label = "Object renderable";
  gui.position.x = left;
  gui.position.y = 250;
  gui.width = width;
  gui.height = 95;
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "object.window_collision", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_PANEL;
  gui.panelAnchor = g2Anchor_None;
  gui.label = "Object collision";
  gui.position.x = left;
  gui.position.y = 350;
  gui.width = width;
  gui.height = 95;
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "object.window_trigger", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_PANEL;
  gui.panelAnchor = g2Anchor_None;
  gui.label = "Object trigger";
  gui.position.x = left;
  gui.position.y = 450;
  gui.width = width;
  gui.height = 95;
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  //panel: object
  obj = game->core->CreateObject( "object.object_name", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_TEXTFIELD;
  gui.position.x = 5;
  gui.position.y = 25;
  gui.width = width - 10;
  gui.label = "ObjectName";
  gui.panel = "editor/object.window_object";
  obj->EnableGui( &gui );

  //panel: renderable
  obj = game->core->CreateObject( "object.is_renderable", guiRoot );
  gui.type = OBJECT_GUI_CHECKBOX;
  gui.position.x = 5;
  gui.position.y = 25;
  gui.label = "Renderable";
  gui.funCallback = Game::GuiProc;
  gui.panel = "editor/object.window_renderable";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "renderable.texture_label", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_LABEL;
  gui.position.x = 5;
  gui.position.y = 40;
  gui.label = "Texture:";
  gui.panel = "editor/object.window_renderable";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "renderable.texture_name", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_TEXTFIELD;
  gui.position.x = 60;
  gui.position.y = 37;
  gui.width = width - 70;
  gui.label = "temp/brick0.png";
  gui.panel = "editor/object.window_renderable";
  obj->EnableGui( &gui );

  //renderable size
  obj = game->core->CreateObject( "renderable.width_label", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_LABEL;
  gui.position.x = 5;
  gui.position.y = 55;
  gui.label = "Size:";
  gui.panel = "editor/object.window_renderable";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "renderable.width", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_SPINNER;
  gui.position.x = 35;
  gui.position.y = 52;
  gui.width = 40;
  gui.spinnerType = g2SpinnerType_Float;
  gui.bounds.min = 0.0f;
  gui.bounds.max = 65536.0f;
  gui.panel = "editor/object.window_renderable";
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "renderable.height_label", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_LABEL;
  gui.position.x = 95;
  gui.position.y = 55;
  gui.label = "X";
  gui.panel = "editor/object.window_renderable";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "renderable.height", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_SPINNER;
  gui.position.x = 105;
  gui.position.y = 52;
  gui.width = 40;
  gui.spinnerType = g2SpinnerType_Float;
  gui.bounds.min = 0.0f;
  gui.bounds.max = 65536.0f;
  gui.panel = "editor/object.window_renderable";
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  //panel: collision
  obj = game->core->CreateObject( "object.is_collision", guiRoot );
  gui.type = OBJECT_GUI_CHECKBOX;
  gui.position.x = 5;
  gui.position.y = 25;
  gui.label = "Collision";
  gui.funCallback = Game::GuiProc;
  gui.panel = "editor/object.window_collision";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "object.is_static", guiRoot );
  gui.type = OBJECT_GUI_CHECKBOX;
  gui.position.x = 90;
  gui.position.y = 25;
  gui.label = "Static";
  gui.funCallback = Game::GuiProc;
  gui.panel = "editor/object.window_collision";
  obj->EnableGui( &gui );

  //panel: trigger
  obj = game->core->CreateObject( "object.is_trigger", guiRoot );
  gui.type = OBJECT_GUI_CHECKBOX;
  gui.position.x = 5;
  gui.position.y = 25;
  gui.label = "Trigger";
  gui.funCallback = Game::GuiProc;
  gui.panel = "editor/object.window_trigger";
  obj->EnableGui( &gui );

  //panel: settings
  obj = game->core->CreateObject( "world.tile_label", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_LABEL;
  gui.position.x = 5;
  gui.position.y = 38;
  gui.label = "Tile size:";
  gui.panel = "editor/object.window_main";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "world.tile_size", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_DROPDOWN;
  gui.position.x = 60;
  gui.position.y = 35;
  stdDequeString dropDownList;
  dropDownList.push_back( "8" );
  dropDownList.push_back( "16" );
  dropDownList.push_back( "24" );
  dropDownList.push_back( "32" );
  dropDownList.push_back( "48" );
  dropDownList.push_back( "64" );
  gui.stringList = dropDownList;
  gui.panel = "editor/object.window_main";
  g2Controller *controller = obj->EnableGui( &gui );
  ( ( g2DropDown* ) controller )->SetSelectionIndex( 2 );
  game->dropDownLists[ controller ] = dropDownList;
  //game->dropDownLists.insert( game->dropDownLists.vavalue_type( controller, dropDownList ) );

  obj = game->core->CreateObject( "object.tile_apply", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_BUTTON;
  gui.position.x = 120;
  gui.position.y = 35;
  gui.label = "Apply";
  gui.panel = "editor/object.window_main";
  gui.funCallback = Game::GuiProc;
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "settings.layer_label", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_LABEL;
  gui.position.x = 5;
  gui.position.y = 20;
  gui.label = "Layer:";
  gui.panel = "editor/object.window_main";
  obj->EnableGui( &gui );

  obj = game->core->CreateObject( "settings.layer", guiRoot );
  gui.Reset();
  gui.type = OBJECT_GUI_DROPDOWN;
  gui.position.x = 60;
  gui.position.y = 17;
  gui.width = 100;
  dropDownList.clear();
  dropDownList.push_back( "default" );
  dropDownList.push_back( "renderable" );
  dropDownList.push_back( "collision" );
  dropDownList.push_back( "trigger" );
  gui.stringList = dropDownList;
  gui.panel = "editor/object.window_main";
  gui.funCallback = Game::GuiProc;
  controller = obj->EnableGui( &gui );
  game->dropDownLists[ controller ] = dropDownList;
}//InitGui
