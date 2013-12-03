//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <windows.h>
#include "core/core.h"
#include "core/imageloader.h"
#include "core/config.h"
#include "worldgridmgr.h"
#include "game.h"
#include "core/animationpack.h"
#include "core/textparser.h"
#include "core/interngl.h"
#include "lua.h"

Game *game = NULL;

extern CoreRenderableListIndicies *__coreRenderableListIndicies;
extern CoreRenderableListIndicies *__coreGUIIndicies;
extern CoreRenderableListIndicies *__coreGUIFreeIndicies;

File __log;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
  //_CrtSetDbgFlag(  _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  game = new Game();
  float gridSize = 1024.0f;
  game->core->CheckGLError( __LINE__, __FILE__ );
  game->core->Init( 0, 0, false, "FlatGL" );
  game->core->CheckGLError( __LINE__, __FILE__ );
  game->core->keyboard.AddListener( Game::KeyboardProc );
  game->core->mouse.AddListener( Game::MouseKeyProc );
  game->core->mouse.AddMoveListener( Game::MouseMoveProc );
  game->core->CheckGLError( __LINE__, __FILE__ );
  Short gridsAroundObject = ( Short )  __config->GetNumber( "grids_preload_range", 0.0f );
  if( !gridsAroundObject ) {
    gridsAroundObject = ( Short ) Math::Ceil( 0.5f * ( max( __config->GetNumber( "gl_screen_width" ), __config->GetNumber( "gl_screen_height" ) ) / gridSize ) );
  }
    //gridsAroundObject = ( Short ) Math::Ceil( 0.5f * ( ( __config->GetNumber( "gl_screen_width" ) + float( WORLD_GRID_BLOCK_SIZE ) ) / ( float( WORLD_GRID_BLOCK_SIZE ) * float( max( blocksPerGrid.x, blocksPerGrid.y ) ) ) ) );
  //__log.PrintInfo( Filelevel_DEBUG, "gridsAroundObject: %d", gridsAroundObject );
  //__log.PrintInfo( Filelevel_DEBUG, "sizeof( RenderableQuad ) = %d", sizeof( RenderableQuad ) );
  game->world = new WorldGridManager( game->core->GetRootObject(), gridsAroundObject );

  bool isDebug = __config->GetBoolean( "dbg_low_alpha" );
  Object *obj;
  Collision *col;
  float worldAlpha = ( isDebug ? 0.1f : 1.0f );
  //CollisionElementPolygon::PointList polyPoints;

  game->core->CheckGLError( __LINE__, __FILE__ );
  game->lua->Init();

  game->core->CreateObject( "gui" )->SetLockToDelete( true )->SetSaveable( false );

  game->core->CheckGLError( __LINE__, __FILE__ );
  game->world->LoadFromFile( "data/temp/testworld.fu" );

  game->core->CheckGLError( __LINE__, __FILE__ );
  game->core->mouse.SetCursor( "data/temp/cursor_hw.png", game->core->CreateObject( "mouse-cursor", game->core->GetObject( "gui" ) )->SetLockToDelete( true )->SetSaveable( false ), __config->GetBoolean( "cursor_hardware", true ) );
  //__log.PrintInfo( Filelevel_DEBUG, "test: %s", core->GetObject( "gui" )->GetNameFull().c_str() );
  //game->core->CreateObject( "bottom-block", game->core->GetObject( "gui" ) )->SetPosition( Vec3( 50.0f, 85.0f, 2.0f ) )->EnableRenderableGUI()->SetSize( Vec2( 100.0f, 30.0f ) )->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha * 0.5f ) )
  //  ->SetTexture( "data/temp/ui-bg-0.png" );
  //game->core->CreateObject( "tooltips" )->SetLockToDelete( true );
  //game->core->CreateObject( "mouse-grid", game->core->GetObject( "tooltips" ) );
  game->core->CreateObject( "defaults" )->SetLockToDelete( true )->SetSaveable( false );
  game->core->CreateObject( "camera", game->core->GetObject( "defaults" ) )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->SetLockToDelete( true )->SetSaveable( false );
  game->core->CreateObject( "active-object", game->core->GetObject( "defaults" ) )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
  game->core->SetCamera( game->core->GetObject( "defaults/camera" ) );
  game->world->AddActiveObject( game->core->GetObject( "defaults/camera" ) );

  /*
  obj = game->core->CreateObject( "test-bg-2" );
  obj->SetPosition( Vec3( 50.0f, 50.0f, -9.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/AUCHINDOUN_VORTEXCLOUD01.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 100.0f, 100.0f ) );
  */


  obj = game->core->CreateObject( "player" );
  obj->SetLockToDelete( true );
  col = obj->EnableCollision();
  col->InitSquare( Vec3( 14.0f, 20.0f, 0.0f ) );
  /*
  polyPoints.clear();
  polyPoints.push_back( Vec2( -8.0f,  10.0f ) );
  polyPoints.push_back( Vec2( -8.0f, -10.0f ) );
  polyPoints.push_back( Vec2(  8.0f, -10.0f ) );
  polyPoints.push_back( Vec2(  8.0f,  10.0f ) );
  col->InitPolygon( polyPoints );
  */
  //col->InitCircle( 20.0f );
  col->SetPosition( Vec3( 65.0f, 30.0f, 5.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 500.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );
  /*
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/mario.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  */
  //quad->scale.Set( 0.5f, 1.0f );
  game->core->SetCamera( obj );
  obj->__Test();
  //obj->EnableLightBlockByCollision();


  Object *background = game->core->CreateObject( "background" );
  background->SetLockToDelete( true );
  background->SetPosition( Vec3( 0.0f, 0.0f, -10.0f ) );
  RenderableQuad *render = ( RenderableQuad* ) background->EnableRenderable( RENDERABLE_TYPE_QUAD );
  render->SetTexture( "data/temp/bg0.png" );
  render->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  render->SetSize( Vec2( ( float ) game->core->GetWindowSize().width * 2.0f, ( float ) game->core->GetWindowSize().height ) );

  game->world->AddActiveObject( game->core->GetObject( "player" ) );

  __log.PrintInfo( Filelevel_DEBUG, "sizeof( RenderableQuad ) = %d", sizeof( RenderableQuad ) );

  //фоновая картинка

  /*
  obj = game->core->CreateObject( "bg-0" );
  obj->SetPosition( Vec3( 160.0f, 100.0f, 0.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD, -4.0f );
  quad->SetTexture( "data/temp/thu128.jpg" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 320.0f, 320.0f ) );
  */

  float t = 0.0f, rot = 0.0f;
  if( wglSwapIntervalEXT ) {
    wglSwapIntervalEXT( __config->GetBoolean( "gl_vsync" ) );
  }
  char tempChar[ 1024 ];
  DWORD fps = 0, currentFps = 0;
  //__textureAtlas->__Dump( "data/temp/__atlas.tga" );

  sTimer.Update();
  game->world->Update( true );
  game->lua->RunFile( "data/scripts/autorun.lua" );
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
  float renderTime = 0.0f, renderPeriod = 1.0f / 60.0f;

  while( game->core->Update() )
  {

    if( false ) { //max FPS
      game->core->Redraw();
      ++currentFps;
    } else { //limited FPS
      renderTime += sTimer.GetDeltaF();
      if( renderTime >= renderPeriod ) {
        /*
        while( renderTime >= renderPeriod ) {
          renderTime -= renderPeriod;
        }
        */
        renderTime = 0.0f;
        game->core->Redraw();
        ++currentFps;
      }
    }
    game->Update();

    //__log.PrintInfo( Filelevel_DEBUG, "=> world->Update" );
    game->world->Update();
    //__log.PrintInfo( Filelevel_DEBUG, "=> game->Update" );

    t += sTimer.GetDeltaF();
    rot += sTimer.GetDeltaF();
    if( t > 1.0f )
    {
      t -= 1.0f;
      fps = currentFps;
      currentFps = 0;
    }

    //background->SetPosition2D( Vec2( game->core->GetObject( "player" )->GetPosition().x, game->core->GetObject( "player" )->GetPosition().y * 0.75f ) );

    if( game->core->keyboard.IsPressed( VK_F9 ) ) { //Clear world
      game->world->ClearWorld();
      game->core->ClearScene();
      game->core->Update();
      game->core->SetCamera( game->core->GetObject( "player" ) );
      //game->world->AddActiveObject( game->core->GetObject( "player" ) );
      game->SetActive( false );
    }


    if( game->core->keyboard.IsPressed( VK_F2 ) ) { //Save world
      char fileName[ MAX_PATH ];
      fileName[ 0 ] = 0;
      OPENFILENAME ofn;
      memset( &ofn, 0, sizeof( ofn ) );
      ofn.lStructSize = sizeof( ofn );
      ofn.hwndOwner = game->core->GetWindowHandle();
      ofn.lpstrFilter = "Flat-universe file (*.fu)\0*.fu\0";
      ofn.lpstrFile = fileName;
      ofn.nMaxFile = MAX_PATH;
      ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
      ofn.lpstrDefExt = "fu";
      ofn.lpstrInitialDir = "data";
      if( GetSaveFileName( &ofn ) == TRUE ) {
        __log.PrintInfo( Filelevel_DEBUG, "GetSaveFileName => true fileName['%s']", fileName );
        std::string name = fileName;
        std::string ext = tools::GetFileExtension( name );
        if( ext != "fu" ) {
          name += ".fu";
        }
        File f;
        bool doSave = false;
        if( f.Open( name, File_mode_READ ) == aOK ) { //file exists. replace?
          f.Close();
          auto result = MessageBox( game->core->GetWindowHandle(), ( "Файл '" + name + "' уже существует. Заменить?" ).c_str(), "Внимание!", MB_YESNO | MB_ICONWARNING );
          doSave = ( result == IDYES );
        } else {
          doSave = true;
        }
        if( doSave ) {
          game->world->SaveToFile( name );
        }
      }
      sTimer.Update();
      game->core->Update();
      game->core->keyboard.ResetState();
      game->core->mouse.ResetState();
    }


    if( game->core->keyboard.IsPressed( VK_F3 ) ) { //Load world
      char fileName[ MAX_PATH ];
      fileName[ 0 ] = 0;
      OPENFILENAME ofn;
      memset( &ofn, 0, sizeof( ofn ) );
      ofn.lStructSize = sizeof( ofn );
      ofn.hwndOwner = game->core->GetWindowHandle();
      ofn.lpstrFilter = "Flat-universe file (*.fu)\0*.fu\0";
      ofn.lpstrFile = fileName;
      ofn.nMaxFile = MAX_PATH;
      ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
      ofn.lpstrDefExt = "fu";
      ofn.lpstrInitialDir = "data";
      if( GetOpenFileName( &ofn ) == TRUE ) {
        __log.PrintInfo( Filelevel_DEBUG, "GetOpenFileName => true fileName['%s']", fileName );
        game->world->ClearWorld();
        game->core->ClearScene();
        game->ClearLuaTimers();
        //game->luaCollisionListeners.clear();
        //game->luaTriggerListeners.clear();
        game->core->Update();
        game->core->SetCamera( game->core->GetObject( "player" ) );
        game->world->LoadFromFile( fileName );
        game->core->GetObject( "player" )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->GetCollision()->SetVelocity( Vec3( 0.0f, 0.0f, 0.0f ) );
        //game->world->AddActiveObject( game->core->GetObject( "player" ) );
        game->SetActive( false );
      }
      sTimer.Update();
      game->core->Update();
      game->core->keyboard.ResetState();
      game->core->mouse.ResetState();
    }

    //if( game->core->keyboard.IsPressed( VK_RETURN ) ) { //active/deactive game process
    //  game->SetActive( !game->isActive );
    //}

    /*
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

    /*
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
        quad->SetPosition( Vec3( ( rand() % 1000 ) / 10.0f, ( rand() % 1000 ) / 10.0f, ( rand() % 10 - 5 ) / 10.0f ) );
        obj->SetPosition( Vec3( ( rand() % 10000 - 5000 ) / 25.0f, ( rand() % 10000 - 5000 ) / 25.0f, 0.0f ) );
        quad->SetSize( Vec2( ( rand() % 500 ) / 10.0f, ( rand() % 500 ) / 10.0f ) );
        quad->SetTexture( "data/temp/null.png" );
        //obj->EnableCollision();
      }
      //game->core->RemoveObject( "/gui/button-exit" );
    }
    */

    //mouse tooltips
    /*
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
    */

    /*
    if( obj = game->core->GetObject( "/lifts/lift" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 5.0f ) * 300.0f, 0.0f ) );
    if( obj = game->core->GetObject( "/lifts/lift-2" ) )
      obj->GetCollision()->SetVelocity( Vec3( 0.0f, Math::Sin16( rot * 3.0f ) * 100.0f, 0.0f ) );
      */

      /*
    if( obj = game->core->GetObject( "test-bg-2" ) )
    {
      quad = ( RenderableQuad* ) obj->GetRenderable();
      if( quad ) {
        float scale = Math::Sin16( rot * 2.0f ) * 1.0f + 5.0f;
        quad->SetScale( Vec2( scale, scale ) )->SetRotation( -rot );
      }
    }
    */
      //obj->SetPosition( Vec3( -20.0f, 50.0f + Math::Sin16( rot * 5.0f ) * 50.0f, 0.0f ) );

      sprintf_s( tempChar, 1024, "FPS[%d] quads[%d/%d] grids[%d] cListeners[%d]", fps, __coreRenderableListIndicies->size(), __coreGUIIndicies->size(), __worldGridList->size(), game->luaCollisionListeners.size() );
    game->core->SetWindowTitle( tempChar );
    game->core->__Test();
  }
  //MessageBox( NULL, "Ok", "Debug", MB_OK );

  game->world->SaveToFile( "data/temp/testworld.fu" );
  DEF_DELETE( game );

  //_CrtDumpMemoryLeaks();
  return 0;
}



Game::Game()
{
  this->isActive = true;
  this->gridTooltipUnderMouse.show = false;
  memset( this->gridTooltipUnderMouse.obj, 0, sizeof( this->gridTooltipUnderMouse.obj ) );
  this->core = new Core();
  this->lua = new Lua();
  Object::OnLoad = Game::ObjectOnLoad;
  Object::OnUnload = Game::ObjectOnUnload;

  LUAFUNC_RemoveObject      = Game::LUA_RemoveObject;
  LUAFUNC_GetObjectPos      = Game::LUA_GetObjectPos;
  LUAFUNC_SetObjectPos      = Game::LUA_SetObjectPos;
  LUAFUNC_SetTimer          = Game::LUA_SetTimer;
  LUAFUNC_StopTimer         = Game::LUA_StopTimer;
  LUAFUNC_CreateObject      = Game::LUA_CreateObject;
  LUAFUNC_ListenKeyboard    = Game::LUA_ListenKeyboard;
  LUAFUNC_ListenMouseKey    = Game::LUA_ListenMouseKey;
  LUAFUNC_ListenMouseMove   = Game::LUA_ListenMouseMove;
  LUAFUNC_GameExit          = Game::LUA_GameExit;
  LUAFUNC_GetMousePos       = Game::LUA_GetMousePos;
  LUAFUNC_GetCameraPos      = Game::LUA_GetCameraPos;
  LUAFUNC_GetGridSize       = Game::LUA_GetGridSize;
  LUAFUNC_GetWindowSize     = Game::LUA_GetWindowSize;
  LUAFUNC_ObjectAddTrigger  = Game::LUA_ObjectAddTrigger;
  LUAFUNC_SetCamera         = Game::LUA_SetCamera;
  LUAFUNC_GetCamera         = Game::LUA_GetCamera;
  LUAFUNC_ClearScene        = Game::LUA_ClearScene;
  LUAFUNC_ObjectEnableRenderable  = Game::LUA_ObjectEnableRenderable;
  LUAFUNC_ObjectDisableRenderable = Game::LUA_ObjectDisableRenderable;
  LUAFUNC_ObjectEnableCollision   = Game::LUA_ObjectEnableCollision;
  LUAFUNC_ObjectDisableCollision  = Game::LUA_ObjectDisableCollision;
  LUAFUNC_ObjectEnableTrigger     = Game::LUA_ObjectEnableTrigger;
  LUAFUNC_ObjectDisableTrigger    = Game::LUA_ObjectDisableTrigger;
  LUAFUNC_ObjectEnableLightBlockByCollision   = Game::LUA_ObjectEnableLightBlockByCollision;
  LUAFUNC_ObjectDisableLightBlockByCollision  = Game::LUA_ObjectDisableLightBlockByCollision;
  LUAFUNC_GetCollisionStatic= Game::LUA_GetCollisionStatic;
  LUAFUNC_SetCollisionStatic= Game::LUA_SetCollisionStatic;
  LUAFUNC_DebugRender       = Game::LUA_DebugRender;
  LUAFUNC_GetObjectByPoint  = Game::LUA_GetObjectByPoint;
  LUAFUNC_GetObjectByRect   = Game::LUA_GetObjectByRect;
  LUAFUNC_SelectObject      = Game::LUA_SelectObject;
  LUAFUNC_GetSelectedObject = Game::LUA_GetSelectedObject;
  LUAFUNC_LoadScript        = Game::LUA_LoadScript;
  LUAFUNC_ObjectAttr        = Game::LUA_ObjectAttr;
  LUAFUNC_ListenCollision   = Game::LUA_ListenCollision;
  LUAFUNC_ListenTrigger     = Game::LUA_ListenTrigger;
  LUAFUNC_SetObjectForce    = Game::LUA_SetObjectForce;
  LUAFUNC_RemoveObjectForce = Game::LUA_RemoveObjectForce;
  LUAFUNC_ObjectHasTag      = Game::LUA_ObjectHasTag;
  LUAFUNC_ObjectAddTag      = Game::LUA_ObjectAddTag;
  LUAFUNC_ObjectRemoveTag   = Game::LUA_ObjectRemoveTag;
  LUAFUNC_ObjectSetAnimation= Game::LUA_ObjectSetAnimation;
  LUAFUNC_ObjectStopAnimation = Game::LUA_ObjectStopAnimation;
  LUAFUNC_SetPause          = Game::LUA_SetPause;
  LUAFUNC_SetLightAmbient   = Game::LUA_SetLightAmbient;

  Collision::SetInitCollisionHandler( Game::LUA_ListenCollision );
  Collision::SetDefaultCollisionHandler( Game::CollisionProc );
  Collision::SetCollisionListenerList( &this->luaCollisionListeners );
  ObjectTrigger::SetInitTriggerHandler( Game::LUA_ListenTrigger );
  ObjectTrigger::SetDefaultTriggerHandler( Game::TriggerProc );
  ObjectTrigger::SetTriggerListenerList( &this->luaTriggerListeners );

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
  ClearLuaTimers
=============
*/
void Game::ClearLuaTimers() {
  bool deleted = true;
  while( deleted ) {
    deleted = false;
    auto iterEnd = game->luaTimers.end();
    for( auto iter = game->luaTimers.begin(); iter != iterEnd; ++iter ) {
      if( !iter->dontPause ) {
        game->luaTimers.erase( iter );
        deleted = true;
        break;
      }
    }
  }
}//ClearLuaTimers



void Game::SetActive( bool setActive ) {
  this->isActive = setActive;
  auto player = this->core->GetObject( "player" );
  //__log.PrintInfo( Filelevel_DEBUG, "Game::SetActive => %d", setActive );
  if( setActive ) {
    this->core->SetState( CORE_STATE_RUN );
  } else { //not active
    this->core->SetState( CORE_STATE_PAUSED );
  }
}//SetActive



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
  std::vector< GameLuaTimer > timerProcs;
  for( auto &timer: this->luaTimers ) {
    if( timer.active ) {
      timer.time -= ( this->core->GetState() == CORE_STATE_PAUSED && !timer.dontPause ? 0.0f : sTimer.GetDeltaF() );
      if( timer.time <= 0.0f ) {
        timer.active = false;
        if( !timer.funcName.empty() ) {
          timerProcs.push_back( GameLuaTimer( timer.id, timer.funcName ) );
        }
      }
    }
  }
  for( auto &timer: timerProcs ) {
    //__log.PrintInfo( Filelevel_DEBUG, "Game::UpdateLuaTimers => id[%d] func['%s']", timer.id, timer.funcName.c_str() );
    LUACALLBACK_Timer( this->lua, timer.id, timer.funcName );
  }
}//UpdateLuaTimers



/*
=============
  StringToObjectList
=============
*/
void Game::StringToObjectList( const std::string &str, ObjectList &list ) const {
  list.clear();
  auto nameList = tools::Explode( str, "//" );
  for( auto &oneName: nameList ) {
    Object *object = this->core->GetObject( oneName );
    if( object ) {
      list.push_back( object );
    } else {
      __log.PrintInfo( Filelevel_WARNING, "Game::StringToObjectList => object '%s' not found", oneName.c_str() );
    }
  }
}//StringToObjectList



/*
=============
  ObjectListToString
=============
*/
std::string Game::ObjectListToString( const ObjectList &list ) const {
  std::string result = "";
  int num = 0;
  for( auto &obj: list ) {
    if( num ) {
      result += "//";
    }
    result += obj->GetNameFull();
    ++num;
  }
  return result;
}//ObjectListToString



/*
=============
  ObjectOnLoad
=============
*/
void Game::ObjectOnLoad( Object* obj ) {
  if( !obj->GetLuaScript().empty() ) {
    game->lua->RunScript( obj->GetLuaScript() );
    game->lua->CallTableTableFunction( "FUObjectList", obj->GetNameFull(), "OnLoad" );
  }
}//ObjectOnLoad




/*
=============
  ObjectOnUnload
=============
*/
void Game::ObjectOnUnload( Object* obj ) {
  if( !obj->GetLuaScript().empty() ) {
    game->lua->CallTableTableFunction( "FUObjectList", obj->GetNameFull(), "OnUnload" );
    game->lua->CallTableTableFunction( "FUObjectList", obj->GetNameFull(), "__fu_destroy" );
  }
}//ObjectOnUnload



void Game::OnRemoveObject( const std::string& objectName ) {
  Object *object = this->core->GetObject( objectName );
  if( !object ) {
    return;
  }

  //collision
  if( object->IsCollision() ) {
    Collision *collision = object->GetCollision();
    auto
      iter = this->luaCollisionListeners.begin(),
      iterEnd = this->luaCollisionListeners.end();
    for( ;iter != iterEnd; ++iter ) {
      if( iter->object == collision ) {
        this->luaCollisionListeners.erase( iter );
        break;
      }
    }
  }

  //trigger
  if( object->IsTrigger() ) {
    ObjectTrigger *trigger = object->GetTrigger();
    auto
      iter = this->luaTriggerListeners.begin(),
      iterEnd = this->luaTriggerListeners.end();
    for( ;iter != iterEnd; ++iter ) {
      if( iter->object == trigger ) {
        this->luaTriggerListeners.erase( iter );
        break;
      }
    }
  }
}//OnRemoveObject



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
  if( obj ) {
    obj->SetPosition2D( pos );
    obj->Update( 0.0f );
  }
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
Dword Game::LUA_SetTimer( float time, const std::string &funcName, bool dontPause )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_SetTimer => func['%s']", funcName.c_str() );
  Dword id, count = game->luaTimers.size();
  bool setted = false;
  for( Dword q = 0; q < count; ++q ) {
    if( !game->luaTimers[ q ].active ) {
      id = q;
      setted = true;
      break;
    }
  }
  if( !setted ) {
    game->luaTimers.push_back( GameLuaTimer() );
    id = game->luaTimers.size() - 1;
  }
  game->luaTimers[ id ].active    = 1;
  game->luaTimers[ id ].time      = time;
  game->luaTimers[ id ].funcName  = funcName;
  game->luaTimers[ id ].dontPause = dontPause;
  game->luaTimers[ id ].id = id;
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_SetTimer => func['%s'] id[%d] done", funcName.c_str(), id );
  return id;
}//LUA_SetTimer


/*
=============
  LUA_StopTimer
=============
*/
void Game::LUA_StopTimer( Dword id )
{
  if( id < game->luaTimers.size() ) {
    game->luaTimers[ id ].active = false;
  }
}//LUA_StopTimer



/*
=============
  LUA_RemoveObject
=============
*/
bool Game::LUA_RemoveObject( const std::string &name )
{
  auto nameList = tools::Explode( name, "//" );
  for( auto &oneName: nameList ) {
    game->OnRemoveObject( oneName );
    game->core->RemoveObject( oneName, true );
  }
  //game->luaCollisionListeners.push_back( listener );
  //collision->AddHandler( Game::CollisionProc );
  return true;
}//LUA_RemoveObject


/*
=============
  LUA_CreateObject
=============
*/
void Game::LUA_CreateObject( const std::string &name, const Vec3 &pos, int notInGrid )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_CreateObject..." );
  long slashPos = name.find_last_of( "/" );
  Object *parentObj = NULL;
  Object *obj       = NULL;
  if( slashPos > 0 ) {
    std::string fullPath = &name[ slashPos ];
    std::vector< std::string > path = tools::Explode( name, "/" );
    std::vector< std::string >::iterator iter, iterEnd = path.end();
    std::string currentPath = "";
    for( iter = path.begin(); iter != iterEnd; ++iter ) {
      //__log.PrintInfo( Filelevel_DEBUG, ". iter: '%s'", iter->c_str() );
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
    obj->Update( 0.0f );
    if( !notInGrid ) {
      WorldGrid::WorldGridPosition gridPos = game->world->GetGridPositionByObject( *obj );
      //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_CreateObject => gridPos[%d; %d]", gridPos.x, gridPos.y );
      game->world->AttachObjectToGrid( gridPos, obj );
    }
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_CreateObject => object['%s'] parent['%s']", obj->GetNameFull().c_str(), obj->GetParentNameFull().c_str() );

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
  collision->InitSquare( size );
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
  LUA_ObjectEnableLightBlockByCollision
=============
*/
void Game::LUA_ObjectEnableLightBlockByCollision( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectEnableLightBlockByCollision => object '%s' not found", objectName.c_str() );
    return;
  }
  if( object->GetCollision() ) {
    object->EnableLightBlockByCollision();
  }
}//LUA_ObjectEnableLightBlockByCollision



/*
=============
  LUA_ObjectDisableLightBlockByCollision
=============
*/
void Game::LUA_ObjectDisableLightBlockByCollision( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_WARNING, "Game::LUA_ObjectDisableLightBlockByCollision => object '%s' not found", objectName.c_str() );
    return;
  }
  object->DisableLightBlockByCollision();
}//LUA_ObjectDisableLightBlockByCollision




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
  KeyboardProc
=============
*/
void Game::KeyboardProc( Dword keyId, bool isPressed )
{
  if( game->luaKeyboardListeners.empty() )
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
  if( game->luaMouseKeyListeners.empty() )
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
  if( game->luaMouseMoveListeners.empty() )
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
  LUA_GetGridSize
=============
*/
float Game::LUA_GetGridSize()
{
  return game->world->GetGridSize();
}//LUA_GetGridSize


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
  LUA_ObjectSetAnimation
=============
*/
void Game::LUA_ObjectSetAnimation( const std::string &objectName, const std::string &templateName, const std::string &animation )
{
  Object *object = game->core->GetObject( objectName );
  if( !object )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectSetAnimation => object '%s' not found", objectName.c_str() );
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ObjectSetAnimation => object['%s'] template['%s'] animation['%s']", objectName.c_str(), templateName.c_str(), animation.c_str() );
  Animation::AnimationPack *pack = object->ApplyAnimation( templateName, animation );
  if( pack ) {
    pack->SetEnabled( true );
  }
}//LUA_ObjectSetAnimation


/*
=============
  LUA_ObjectStopAnimation
=============
*/
void Game::LUA_ObjectStopAnimation( const std::string &objectName )
{
  Object *object = game->core->GetObject( objectName );
  if( !object )
  {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectStopAnimation => object '%s' not found", objectName.c_str() );
    return;
  }
  object->StopAnimation();
}//LUA_ObjectStopAnimation


/*
=============
  LUA_SetPause
=============
*/
void Game::LUA_SetPause( bool isPause )
{
  game->SetActive( isPause );
}//LUA_SetPause


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
  switch( type ) {
    case 1: { //renderable
      __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_GetObjectByPoint => type[%d] point[%3.3f; %3.3f] after['%s'] ...", type, point.x, point.y, afterObject.c_str() );
      Object *object = game->core->GetRenderableInPoint( point, afterObject );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
        __log.PrintInfo( Filelevel_DEBUG, ". found['%s']", object->GetNameFull().c_str() );
        return object->GetNameFull();
      } else {
        object = game->core->GetRenderableInPoint( point, afterObject );
        while( object && !object->IsSaveable() ) {
          object = object->GetParent();
        }
        if( object ) {
          __log.PrintInfo( Filelevel_DEBUG, ". found['%s']", object->GetNameFull().c_str() );
          return object->GetNameFull();
        }
      }
      __log.PrintInfo( Filelevel_DEBUG, ". not found" );
      break;
    }//renderable
    case 2: { //collision
      Object *object = game->core->GetCollisionInPoint( point, afterObject );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
        return object->GetNameFull();
      } else {
        if( afterObject.length() ) {
          object = game->core->GetCollisionInPoint( point, "" );
          while( object && !object->IsSaveable() ) {
            object = object->GetParent();
          }
          if( object ) {
            return object->GetNameFull();
          }
        }
      }
      break;
    }//collision
    case 3: { //trigger
      Object *object = game->core->GetTriggerInPoint( point, afterObject );
      while( object && !object->IsSaveable() ) {
        object = object->GetParent();
      }
      if( object ) {
        return object->GetNameFull();
      } else {
        if( afterObject.length() ) {
          object = game->core->GetTriggerInPoint( point, "" );
          while( object && !object->IsSaveable() ) {
            object = object->GetParent();
          }
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
  LUA_GetObjectByRect
=============
*/
std::string Game::LUA_GetObjectByRect( int type, const Vec2 &leftTop, const Vec2 &rightBottom )
{
  ObjectList objectList;
  game->core->GetObjectsInRect( type, leftTop, rightBottom, objectList );
  std::string result = "";
  int num = 0;

  for( auto &object: objectList ) {
    if( num ) {
      result += "//";
    }
    result += object->GetNameFull();
    ++num;
  }
  return result;
}//LUA_GetObjectByRect



/*
=============
  LUA_SelectObject
=============
*/
void Game::LUA_SelectObject( const std::string &name )
{
  game->core->debug.selectedObjects.clear();
  if( !name.length() ) {
    //game->core->debug.selectedObject = NULL;
    game->core->debug.selectedObjects.clear();
  } else {
    auto nameList = tools::Explode( name, "//" );
    for( auto &oneName: nameList ) {
      Object *object = game->core->GetObject( oneName );
      if( !object ) {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_SelectObject => object '%s' not found", oneName.c_str() );
      } else {
        __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_SelectObject => object '%s'", object->GetNameFull().c_str() );
        game->core->debug.selectedObjects.push_back( object );
      }
    }
    /*
    Object *object = game->core->GetObject( name );
    if( !object ) {
      __log.PrintInfo( Filelevel_ERROR, "Game::LUA_SelectObject => object '%s' not found", name.c_str() );
    } else {
      //game->core->debug.selectedObject = object;
      game->core->debug.selectedObjects.clear();
      game->core->debug.selectedObjects.push_back( object );
    }
    */
  }
}//LUA_SelectObject


/*
=============
  LUA_GetSelectedObject
=============
*/
std::string Game::LUA_GetSelectedObject()
{
  std::string result = "";
  int num = 0;
  for( auto &obj: game->core->debug.selectedObjects ) {
    if( num ) {
      result += "//";
    }
    result += obj->GetNameFull();
    ++num;
  }
  return result;
  //return ( game->core->debug.selectedObject ? game->core->debug.selectedObject->GetNameFull() : "" );
}//LUA_GetSelectedObject


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
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ObjectAttr: %d", setAttributes.size() );

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
    //__log.PrintInfo( Filelevel_DEBUG, ". set %s = '%s'", ( *iter )->name.c_str(), ( *iter )->value.GetString().c_str() );

    const std::string &name = ( *iter )->name;
    const Variable &value   = ( *iter )->value;

    //object
    if( name == "renderable" ) {
      if( value.GetBoolean() ) {
        //__log.PrintInfo( Filelevel_DEBUG, ". EnableRenderable['%s']", object->GetNameFull().c_str() );
        object->EnableRenderable( RENDERABLE_TYPE_QUAD );
      } else {
        //__log.PrintInfo( Filelevel_DEBUG, ". DisableRenderable['%s']", object->GetNameFull().c_str() );
        object->DisableRenderable();
      }
      continue;
    }//renderable
    if( name == "collision" ) {
      if( value.GetBoolean() ) {
        //__log.PrintInfo( Filelevel_DEBUG, ". EnableCollision['%s']", object->GetNameFull().c_str() );
        object->EnableCollision();
      } else {
        //__log.PrintInfo( Filelevel_DEBUG, ". DisableCollision['%s']", object->GetNameFull().c_str() );
        object->DisableCollision();
      }
      continue;
    }//collision
    if( name == "trigger" ) {
      if( value.GetBoolean() ) {
        //__log.PrintInfo( Filelevel_DEBUG, ". EnableTrigger['%s']", object->GetNameFull().c_str() );
        object->EnableTrigger();
      } else {
        //__log.PrintInfo( Filelevel_DEBUG, ". DisableTrigger['%s']", object->GetNameFull().c_str() );
        object->DisableTrigger();
      }
      continue;
    }//trigger
    if( name == "lightBlockByCollision" ) {
      if( value.GetBoolean() ) {
        object->EnableLightBlockByCollision();
      } else {
        object->DisableLightBlockByCollision();
      }
      continue;
    }//lightBlockByCollision
    if( name == "lightPoint" ) {
      if( value.GetBoolean() ) {
        object->EnableLightPoint();
      } else {
        object->DisableLightPoint();
      }
      continue;
    }//lightPoint
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
        collision->InitSquare( size );
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
    if( name == "triggerPolygon" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementPolygon::PointList points;
        TextParser parser( value.GetString().c_str(), value.GetString().size() );
        TextParser::Result parserValue;
        int step = 0;
        Vec2 point;
        while( parser.GetNext( parserValue ) ) {
          if( parserValue.type != TPL_NUMBER ) {
            __log.PrintInfo( Filelevel_WARNING, "triggerPolygon => bad format of string '%s'", value.GetString().c_str() );
            break;
          } else {
            if( step == 0 ) {
              point.x = parserValue.GetFloat();
            } else {
              point.y = parserValue.GetFloat();
              //__log.PrintInfo( Filelevel_DEBUG, "triggerPolygon => point[%3.3f; %3.3f]", point.x, point.y );
              points.push_back( point );
            }
          }
          step ^= 1;
        }
        trigger->SetPolygon( points );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => triggerPolygon: object '%s' not trigger", object->GetNameFull().c_str() );
      }
      continue;
    }//triggerPolygon
    if( name == "lightPointSize" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        Vec2 size( 1.0f, 1.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f", &size.x, &size.y );
        __log.PrintInfo( Filelevel_DEBUG, "lightPointSize => [%3.3f; %3.3f]", size.x, size.y );
        if( res < 2 ) {
          size.y = size.x;
        }
        widget->SetSize( size );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => lightPointSize: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointSize
    if( name == "lightPointColor" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        Vec4 color( 1.0f, 1.0f, 1.0f, 1.0f );
        int res = sscanf_s( value.GetString().c_str(), "%f %f %f %f", &color.x, &color.y, &color.z, &color.w );
        if( res < 4 ) {
          color.y = color.z = color.w = color.x;
        }
        __log.PrintInfo( Filelevel_DEBUG, "lightPointColor[%3.3f; %3.3f; %3.3f; %3.3f]", color.x, color.y, color.z, color.w );
        widget->SetColor( color );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => color: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointColor
    if( name == "lightPointPenetration" ) {
      ObjectWidget::WidgetLightPoint *widget = object->GetLightPoint();
      if( widget ) {
        float penetration;
        int res = sscanf_s( value.GetString().c_str(), "%f", &penetration );
        __log.PrintInfo( Filelevel_DEBUG, "lightPointPenetration => %3.3f", penetration );
        widget->SetPenetration( penetration );
      } else {
        __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAttr => lightPointPenetration: object '%s' not lightPoint", object->GetNameFull().c_str() );
      }
      continue;
    }//lightPointPenetration
    if( name == "z" ) {
      object->SetZ( value.GetNumber() );
      continue;
    }//z

  }//for iter

  //get
  iterEnd = getAttributes.end();
  for( iter = getAttributes.begin(); iter != iterEnd; ++iter ) {
    //__log.PrintInfo( Filelevel_DEBUG, ". get '%s'", ( *iter )->name.c_str() );

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
    if( name == "lightBlockByCollision" ) {
      value.SetBoolean( object->IsLightBlockByCollision() );
      continue;
    }//lightBlockByCollision
    if( name == "lightPoint" ) {
      value.SetBoolean( object->IsLightPoint() );
      continue;
    }//lightPoint
    if( name == "position" ) {
      value.SetVec3( object->GetPosition() );
      continue;
    }//position

    //renderable
    if( name == "textureName" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const std::string &texture = renderable->GetTextureFileName();
        value.SetString( texture.empty() ? texture : &renderable->GetTextureFileName()[ strlen( FileManager_DATADIRECTORY ) + 1 ] );
      } else {
        value.SetString( "" );
      }
      continue;
    }//textureName
    if( name == "renderableSize" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec2( renderable->GetSize() );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//renderableSize
    if( name == "renderablePosition" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        const Vec3 &pos = renderable->GetPosition();
        value.SetVec2( Vec2( pos.x, pos.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//renderablePosition
    if( name == "color" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec4( renderable->GetColor() );
      } else {
        value.SetVec4( Vec4Null );
      }
      continue;
    }//color
    if( name == "renderableScale" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetVec2( renderable->GetScale() );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//renderableScale
    if( name == "renderableRotation" ) {
      renderable = ( RenderableQuad* ) object->GetRenderable();
      if( renderable ) {
        value.SetNumber( renderable->GetRotation() );
      } else {
        value.SetNumber( 0.0f );
      }
      continue;
    }//renderableRotation

    //collision
    if( name == "collisionSize" ) {
      collision = object->GetCollision();
      if( collision ) {
        const Vec3 &size = collision->GetSize();
        value.SetVec2( Vec2( size.x, size.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//collisionSize
    if( name == "collisionAcceleration" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 acceleration = collision->GetAcceleration();
        value.SetVec2( Vec2( acceleration.x, acceleration.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//collisionAcceleration
    if( name == "collisionVelocity" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 velocity = collision->GetVelocity();
        value.SetVec2( Vec2( velocity.x, velocity.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//collisionVelocity
    if( name == "collisionStatic" ) {
      collision = object->GetCollision();
      if( collision ) {
        value.SetBoolean( collision->IsStatic() );
      } else {
        value.SetBoolean( false );
      }
      continue;
    }//collisionStatic
    if( name == "collisionForce" ) {
      collision = object->GetCollision();
      if( collision ) {
        Vec3 force = collision->GetForce();
        value.SetVec2( Vec2( force.x, force.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//collisionForce

    //trigger
    if( name == "triggerType" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementType type = trigger->GetType();
        std::string typeName = "";
        switch( type ) {
        case COLLISION_ELEMENT_TYPE_SQUARE:
          typeName = "square";
          break;
        case COLLISION_ELEMENT_TYPE_CIRCLE:
          typeName = "circle";
          break;
        case COLLISION_ELEMENT_TYPE_POLYGON:
          typeName = "polygon";
          break;
        }
        value.SetString( typeName );
      } else {
        value.SetString( "" );
      }
      continue;
    }//triggerType
    if( name == "triggerSize" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        Vec3 size = trigger->GetSize();
        value.SetVec2( Vec2( size.x, size.y ) );
      } else {
        value.SetVec2( Vec2Null );
      }
      continue;
    }//triggerSize
    if( name == "triggerPolygon" ) {
      trigger = object->GetTrigger();
      if( trigger ) {
        CollisionElementPolygon::PointList pointList;
        if( trigger->GetPolygon( pointList ) ) {
          std::string result = "";
          char temp[ 1024 ];
          for( auto &point: pointList ) {
            sprintf_s( temp, sizeof( temp ), "%s%3.3f %3.3f", result.size() ? " " : "", point.x, point.y );
            result += temp;
          }
          value.SetString( result );
        } else {
          value.SetString( "" );
        }
      } else {
        value.SetString( "" );
      }
      continue;
    }//triggerPolygon
    if( name == "z" ) {
      float z = object->GetPosition().z;
      value.SetNumber( z );
      continue;
    }//z

  }
}//LUA_ObjectAttr




/*
=============
  LUA_ListenCollision
=============
*/
void Game::LUA_ListenCollision( const std::string &objectName, const std::string &funcName ) {
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ListenCollision => object '%s' not found", objectName.c_str() );
    return;
  }
  Collision *collision = object->GetCollision();
  if( !collision ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ListenCollision => object '%s' don't have collision", objectName.c_str() );
    return;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ListenCollision => object['%s'] collision[x%p] function['%s']", object->GetNameFull().c_str(), collision, funcName.c_str() );

  luaCollisionListenerStruct listener( collision, funcName );

  game->luaCollisionListeners.push_back( listener );
  collision->AddHandler( Game::CollisionProc );
}//LUA_ListenCollision




/*
=============
  LUA_ListenTrigger
=============
*/
void Game::LUA_ListenTrigger( const std::string &objectName, const std::string &funcName ) {
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ListenTrigger => object '%s' not found", objectName.c_str() );
    return;
  }
  ObjectTrigger *trigger = object->GetTrigger();
  if( !trigger ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ListenTrigger => object '%s' don't have collision", objectName.c_str() );
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Game::LUA_ListenTrigger => object['%s'] collision[x%p] function['%s']", object->GetNameFull().c_str(), trigger, funcName.c_str() );

  luaTriggerListenerStruct listener( trigger, funcName );

  game->luaTriggerListeners.push_back( listener );
  trigger->AddHandler( Game::TriggerProc );
}//LUA_ListenTrigger




/*
=============
  CollisionProc
=============
*/
void Game::CollisionProc( Collision *a, Collision *b, Byte flags, const Vec3 &velocity ) {
  Object *objectA = game->core->GetObjectByCollision( a );
  if( !objectA ) {
    //__log.PrintInfo( Filelevel_ERROR, "Game::CollisionProc => object A by collision x%p not found", a );
    return;
  }
  Object *objectB = game->core->GetObjectByCollision( b );
  if( !objectB ) {
    //__log.PrintInfo( Filelevel_ERROR, "Game::CollisionProc => object B by collision x%p not found", b );
    return;
  }
  if( game->luaCollisionListeners.empty() ) {
    return;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "Game::CollisionProc => a[x%p] b[x%p] objectA['%s'] objectB['%s']", a, b, objectA->GetNameFull().c_str(), objectB->GetNameFull().c_str() );
  luaCollisionListenersList::iterator iter, iterEnd = game->luaCollisionListeners.end();
  for( iter = game->luaCollisionListeners.begin(); iter != iterEnd; ++iter ) {
    if( iter->object == a ) {
      LUACALLBACK_ListenCollision( game->lua, iter->funcName, objectA->GetNameFull(), objectB->GetNameFull(), flags, velocity );
    }
  }
}//CollisionProc




/*
=============
  TriggerProc
=============
*/
void Game::TriggerProc( ObjectTrigger *trigger, Collision *collision, bool isInTrigger ) {
  Object *objectA = game->core->GetObjectByTrigger( trigger );
  if( !objectA ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::TriggerProc => object A by trigger x%p not found", trigger );
    return;
  }
  Object *objectB = game->core->GetObjectByCollision( collision );
  if( !objectB ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::TriggerProc => object B by trigger x%p not found", collision );
    return;
  }
  if( game->luaTriggerListeners.empty() ) {
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Game::TriggerProc => a[x%p] b[x%p] objectA['%s'] objectB['%s']", trigger, collision, objectA->GetNameFull().c_str(), objectB->GetNameFull().c_str() );
  luaTriggerListenersList::iterator iter, iterEnd = game->luaTriggerListeners.end();
  for( iter = game->luaTriggerListeners.begin(); iter != iterEnd; ++iter ) {
    if( iter->object == trigger ) {
      LUACALLBACK_ListenTrigger( game->lua, iter->funcName, objectA->GetNameFull(), objectB->GetNameFull(), isInTrigger );
    }
  }
}//TriggerProc


/*
=============
  LUA_SetObjectForce
=============
*/
void Game::LUA_SetObjectForce( const std::string &objectName, int num, const Vec2& force )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_SetObjectForce => object '%s' not found", objectName.c_str() );
    return;
  }
  object->SetForce( num, Vec3( force.x, force.y, 0.0f ) );
}//LUA_SetObjectForce


/*
=============
  LUA_RemoveObjectForce
=============
*/
void Game::LUA_RemoveObjectForce( const std::string &objectName, int num )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_RemoveObjectForce => object '%s' not found", objectName.c_str() );
    return;
  }
  object->RemoveForce( num );
}//LUA_RemoveObjectForce


/*
=============
  LUA_ObjectHasTag
=============
*/
bool Game::LUA_ObjectHasTag( const std::string &objectName, const std::string &tag )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectHasTag => object '%s' not found", objectName.c_str() );
    return false;
  }
  return object->IsHasTag( tag );
}//LUA_ObjectHasTag


/*
=============
  LUA_ObjectHasTag
=============
*/
void  Game::LUA_ObjectAddTag( const std::string &objectName, const std::string &tag )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectAddTag => object '%s' not found", objectName.c_str() );
    return;
  }
  return object->AddTag( tag );
}//LUA_ObjectAddTag


/*
=============
  LUA_ObjectRemoveTag
=============
*/
void  Game::LUA_ObjectRemoveTag( const std::string &objectName, const std::string &tag )
{
  Object *object = game->core->GetObject( objectName );
  if( !object ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::LUA_ObjectRemoveTag => object '%s' not found", objectName.c_str() );
    return;
  }
  return object->RemoveTag( tag );
}//LUA_ObjectRemoveTag


/*
=============
  LUA_SetLightAmbient
=============
*/
void Game::LUA_SetLightAmbient( const Vec4 &color )
{
  game->core->GetLightRenderer()->GetLightManager()->lightAmbient = color;
}//LUA_SetLightAmbient
