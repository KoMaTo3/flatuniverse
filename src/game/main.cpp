#include <windows.h>
#include "core/core.h"
#include "core/imageloader.h"
#include "core/config.h"
#include "worldgridmgr.h"
#include "game.h"

Game *game = NULL;

extern CoreRenderableListIndicies  *__coreRenderableListIndicies;
extern CoreRenderableListIndicies  *__coreGUIIndicies;


File __log;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
  game = new Game();
  Pos< Short> blocksPerGrid( 8, 8 );
  game->core->Init( 0, 0, false, "FlatGL" );
  game->core->keyboard.AddListener( Game::KeyboardProc );
  game->core->mouse.AddListener( Game::MouseKeyProc );
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

  game->core->CreateObject( "gui" );

  game->world->LoadFromFile( "data/temp/testworld.fu" );

  game->core->CreateObject( "lifts" );

  game->core->mouse.SetCursor( "data/temp/cursor_hw.png", game->core->CreateObject( "mouse-cursor", game->core->GetObject( "gui" ) ) );
  //__log.PrintInfo( Filelevel_DEBUG, "test: %s", core->GetObject( "gui" )->GetNameFull().c_str() );
  game->core->CreateObject( "bottom-block", game->core->GetObject( "gui" ) )->SetPosition( Vec3( 50.0f, 85.0f, 2.0f ) )->EnableRenderableGUI()->SetSize( Vec2( 100.0f, 30.0f ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha * 0.5f ) )
    ->SetTexture( "data/temp/ui-bg-0.png" );
  game->core->CreateObject( "tooltips" );
  game->core->CreateObject( "mouse-grid", game->core->GetObject( "tooltips" ) );

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
  game->world->AttachObjectToGrid( 0, 0, obj );

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

  sTimer.Update();
  game->world->Update( true );
  game->lua->RunFile( "data/scripts/test.lua" );

  while( game->core->Update() )
  {
    game->core->Redraw();
    game->world->Update();
    game->Update();

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

    /*
    if( game->core->keyboard.IsPressed( VK_SPACE ) )
    {
      if( game->core->keyboard.IsHold( VK_SHIFT ) )
        game->lua->RunFile( "data/scripts/test.lua" );
      else
        game->lua->CallFunction( "SpacePressed" );
    }
    */

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

    /*
    if( obj = game->core->GetObject( "/lifts/lift" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 5.0f ) * 300.0f, 0.0f ) );
    if( obj = game->core->GetObject( "/lifts/lift-2" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 3.0f ) * 100.0f, 0.0f ) );
      */

    if( obj = game->core->GetObject( "test-bg-2" ) )
    {
      quad = ( RenderableQuad* ) obj->GetRenderable();
      float scale = Math::Sin16( rot * 2.0f ) * 1.0f + 5.0f;
      quad->SetScale( Vec2( scale, scale ) )->SetRotation( -rot );
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

  LUAFUNC_RemoveObject  = Game::LUA_RemoveObject;
  LUAFUNC_GetObjectPos  = Game::LUA_GetObjectPos;
  LUAFUNC_SetObjectPos  = Game::LUA_SetObjectPos;
  LUAFUNC_SetTimer      = Game::LUA_SetTimer;
  LUAFUNC_CreateObject  = Game::LUA_CreateObject;
  LUAFUNC_ListenKeyboard= Game::LUA_ListenKeyboard;
  LUAFUNC_ListenMouseKey= Game::LUA_ListenMouseKey;
  LUAFUNC_GameExit      = Game::LUA_GameExit;
  LUAFUNC_GetMousePos   = Game::LUA_GetMousePos;
  LUAFUNC_GetCameraPos  = Game::LUA_GetCameraPos;
  LUAFUNC_GetWindowSize = Game::LUA_GetWindowSize;
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
  LUA_RemoveObject
=============
*/
bool Game::LUA_RemoveObject( const std::string &name )
{
  return game->core->RemoveObject( name );
}//LUA_RemoveObject


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
  LUA_CreateObject
=============
*/
void Game::LUA_CreateObject( const std::string &name, const std::string &parentName, const Vec3 &pos, const Vec2 &size, const std::string &textureName, const Vec4 &color )
{
  Object *parentObj = ( parentName.length() ? game->core->GetObject( parentName ) : NULL );
  Object *obj = game->core->CreateObject( name, parentObj );
  Collision *col = obj->EnableCollision();
  col->SetPosition( pos )->SetIsStatic( true )->SetSize( Vec3( size.x, size.y, 0.0f ) );
  RenderableQuad *quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( color );
  quad->SetSize( Vec2( size.x, size.y ) );
  quad->SetTexture( textureName, Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );

  game->world->AttachObjectToGrid( game->world->GetGridPositionByObject( *obj ), obj );
}//LUA_CreateObject


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
