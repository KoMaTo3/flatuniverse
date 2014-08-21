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
#include "game/luaobject.h"
#include "game/luaobjects/object.h"

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
  game->core->SetOnCollisionMgrUpdate( Game::OnCollisionMgrUpdate );
  game->core->CheckGLError( __LINE__, __FILE__ );
  game->core->keyboard.AddListener( Game::KeyboardProc );
  game->core->mouse.AddListener( Game::MouseKeyProc );
  game->core->mouse.AddMoveListener( Game::MouseMoveProc );
  game->core->CheckGLError( __LINE__, __FILE__ );
  Short gridsAroundObject = ( Short )  __config->GetNumber( "grids_preload_range", 0.0f, true );
  if( !gridsAroundObject ) {
    gridsAroundObject = ( Short ) Math::Ceil( 0.5f * ( max( __config->GetNumber( "gl_screen_width" ), __config->GetNumber( "gl_screen_height" ) ) / gridSize ) );
  }
    //gridsAroundObject = ( Short ) Math::Ceil( 0.5f * ( ( __config->GetNumber( "gl_screen_width" ) + float( WORLD_GRID_BLOCK_SIZE ) ) / ( float( WORLD_GRID_BLOCK_SIZE ) * float( max( blocksPerGrid.x, blocksPerGrid.y ) ) ) ) );
  //__log.PrintInfo( Filelevel_DEBUG, "gridsAroundObject: %d", gridsAroundObject );
  //__log.PrintInfo( Filelevel_DEBUG, "sizeof( RenderableQuad ) = %d", sizeof( RenderableQuad ) );
  game->world = new World::GridManager( game->core->GetRootObject(), gridsAroundObject );

  bool isDebug = __config->GetBoolean( "is_debug" );
  Object *obj;
  Collision *col;
  float worldAlpha = ( isDebug ? 0.1f : 1.0f );
  //CollisionElementPolygon::PointList polyPoints;

  game->core->CheckGLError( __LINE__, __FILE__ );
  game->lua->Init( game );

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
  game->world->AddActiveObject( game->core->GetObject( "defaults/camera" ), true );

  /*
  obj = game->core->CreateObject( "test-bg-2" );
  obj->SetPosition( Vec3( 50.0f, 50.0f, -9.0f ) );
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetTexture( "data/temp/AUCHINDOUN_VORTEXCLOUD01.png" );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  quad->SetSize( Vec2( 100.0f, 100.0f ) );
  */

  /*
  obj = game->core->CreateObject( "player", NULL, true );
  obj->SetLuaScript( "data/scripts/objects/player.lua" );
  obj->SetLockToDelete( true );
  col = obj->EnableCollision();
  col->InitSquare( Vec3( 14.0f, 20.0f, 0.0f ) );
  col->SetPosition( Vec3( 65.0f, 30.0f, 5.0f ) );
  col->SetAcceleration( Vec3( 0.0f, 500.0f, 0.0f ) );
  col->SetIsStatic( false );
  col->SetMass( 1000.0f );

  memory scriptPlayer;
  if( __fileManager->GetFile( "data/scripts/objects/player.lua", scriptPlayer, true ) ) {
    Engine::LuaObject::BindTemplateToObject( obj->GetLuaObjectId(), obj, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, scriptPlayer.getData() );
    memory playerData;
    if( __fileManager->GetFile( "data/temp/player_data.txt", playerData ) ) {
      Engine::LuaObject::LoadObjectDataFromDump( obj->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, ( unsigned char* ) playerData.getData(), playerData.getLength() );
      //Engine::LuaObject::CallFunction( obj->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONCREATE );
      __log.PrintInfo( Filelevel_DEBUG, "player->ObjectOnLoad()..." );
      game->ObjectOnLoad( obj );
    }
  }
  */

  /*
  quad = ( RenderableQuad* ) obj->EnableRenderable( RENDERABLE_TYPE_QUAD );
  quad->SetColor( Vec4( 1.0f, 1.0f, 1.0f, worldAlpha ) );
  //quad->SetPosition( Vec3( 50.0f, 50.0f, 0.0f ) );
  quad->SetSize( Vec2( 20.0f, 20.0f ) );
  quad->SetTexture( "data/temp/mario.png", Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
  */
  //quad->scale.Set( 0.5f, 1.0f );
  //game->core->SetCamera( obj );
  //obj->__Test();
  //obj->EnableLightBlockByCollision();


  Object *background = game->core->CreateObject( "background" );
  background->SetLockToDelete( true );
  background->SetPosition( Vec3( 0.0f, 0.0f, -10.0f ) );
  RenderableQuad *render = ( RenderableQuad* ) background->EnableRenderable( RENDERABLE_TYPE_QUAD );
  render->SetTexture( "data/temp/bg0.png" );
  render->SetColor( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  render->SetSize( Vec2( ( float ) game->core->GetWindowSize().width * 2.0f, ( float ) game->core->GetWindowSize().height ) );

  //game->world->AddActiveObject( game->core->GetObject( "player" ), true );

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
  float renderTime = 0.0f, renderPeriod = 1.0f / 50.0f;

  while( game->core->Update() )
  {
    if( true ) { //max FPS
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
      game->core->SetCamera( game->core->GetObject( "defaults/camera" ) );
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
        //game->collisionListeners.clear();
        //game->luaTriggerListeners.clear();
        game->core->Update();
        game->core->SetCamera( game->core->GetObject( "defaults/camera" ) );
        game->world->LoadFromFile( fileName );
        //game->core->GetObject( "player" )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->GetCollision()->SetVelocity( Vec3( 0.0f, 0.0f, 0.0f ) );
        game->core->GetObject( "defaults/camera" )->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) )->GetCollision()->SetVelocity( Vec3( 0.0f, 0.0f, 0.0f ) );
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

      sprintf_s( tempChar, 1024, "FPS[%d] quads[%d/%d] grids[%d] cListeners[%d] %d", fps, __coreRenderableListIndicies->size(), __coreGUIIndicies->size(), World::__worldGridList->size(), game->collisionListeners.size(), game->lua->GetStackParmsCount() );
    game->core->SetWindowTitle( tempChar );
    game->core->__Test();
  }
  //MessageBox( NULL, "Ok", "Debug", MB_OK );

  /*
  __log.PrintInfo( Filelevel_DEBUG, "Player: x%p", game->core->GetObject( "player" ) );
  __log.PrintInfo( Filelevel_DEBUG, "Player id: %d", game->core->GetObject( "player" )->GetLuaObjectId() );
  Engine::LuaObject::CallFunction( game->core->GetObject( "player" )->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONDESTROY );
  char *playerSaveData = 0;
  size_t playerSaveDataSize = 0;
  Engine::LuaObject::SaveObjectDataToDump( game->core->GetObject( "player" )->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, playerSaveData, playerSaveDataSize );
  File f( "data/temp/player_data.txt", File_mode_WRITE );
  f.Write( playerSaveData, playerSaveDataSize );
  f.Close();
  */

  game->world->SaveToFile( "data/temp/testworld.fu" );
  __log.PrintInfo( Filelevel_DEBUG, "game->world->SaveToFile() done" );
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
  Object::OnDestroy = Game::ObjectOnDestroy;
  Object::OnIsActiveObject = Game::ObjectOnIsActive;

  //LUAFUNC_RemoveObject      = Game::LUA_RemoveObject;

  Engine::LuaObject_callback_SetOnSetScript( Game::LuaObject_OnSetScript );

  //Collision::SetInitCollisionHandler( Game::LUA_ListenCollision );
  Collision::SetDefaultCollisionHandler( Game::CollisionProc );
  Collision::SetCollisionListenerList( &this->collisionListeners );
  //ObjectTrigger::SetInitTriggerHandler( Game::LUA_ListenTrigger );
  ObjectTrigger::SetDefaultTriggerHandler( Game::TriggerProc );
  ObjectTrigger::SetTriggerListenerList( &this->luaTriggerListeners );

  __ObjectTriggerOnRemoveGlobalHandler = Game::OnRemoveTrigger;

  this->animationSetActionByName.insert( std::make_pair( "stop", Animation::ANIMATION_SET_ACTION_STOP ) );
  this->animationSetActionByName.insert( std::make_pair( "die", Animation::ANIMATION_SET_ACTION_DIE ) );
  this->animationSetActionByName.insert( std::make_pair( "repeat", Animation::ANIMATION_SET_ACTION_REPEAT ) );
  this->animationSetActionByName.insert( std::make_pair( "next", Animation::ANIMATION_SET_ACTION_SET_ANIMATION ) );
}//constructor

Game::~Game()
{
  __log.PrintInfo( Filelevel_DEBUG, "~Game => core->Destroy..." );
  this->core->Destroy();
  __log.PrintInfo( Filelevel_DEBUG, "~Game => delete lua..." );
  DEF_DELETE( this->lua );
  __log.PrintInfo( Filelevel_DEBUG, "~Game => delete world..." );
  DEF_DELETE( this->world );
  __log.PrintInfo( Filelevel_DEBUG, "~Game => delete core..." );
  DEF_DELETE( this->core );
  __log.PrintInfo( Filelevel_DEBUG, "~Game done" );
}//destructor



void Game::LuaObject_OnSetScript( Object *object ) {
  __log.PrintInfo( Filelevel_DEBUG, "LuaObject_OnSetScript => %s", object->GetNameFull().c_str() );
  if( !Engine::LuaObject::CallFunction( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONCREATE ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  object->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONCREATE.c_str() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONUPDATE ) ) {
    game->luaObjectOnUpdate.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONUPDATECORE ) ) {
    game->luaObjectOnUpdateCore.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONFRAME ) ) {
    game->luaObjectOnFrame.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONFRAMECORE ) ) {
    game->luaObjectOnFrameCore.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONKEYPRESS ) ) {
    game->luaObjectOnKeyPress.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONKEYRELEASE ) ) {
    game->luaObjectOnKeyRelease.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEKEYPRESS ) ) {
    game->luaObjectOnMouseKeyPress.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEKEYRELEASE ) ) {
    game->luaObjectOnMouseKeyRelease.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEMOVE ) ) {
    game->luaObjectOnMouseMove.push_back( object->GetLuaObjectId() );
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONCOLLISION ) ) {
    Collision *collision = object->GetCollision();
    if( collision ) {
      game->luaObjectOnCollision.push_back( object->GetLuaObjectId() );
      game->collisionListeners.push_back( luaCollisionListenerStruct( object->GetCollision(), object->GetLuaObjectId() ) );
      collision->AddHandler( Game::CollisionProc );
    } else {
      //__log.PrintInfo( Filelevel_ERROR, "Game::LuaObject_OnSetScript => object '%s' has OnCollision handler, but don't have collision", object->GetNameFull().c_str() );
    }
  }
  if( Engine::LuaObject::IsFunctionExists( object->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONTRIGGER ) ) {
    if( object->IsTrigger() ) {
      game->luaObjectOnTrigger.push_back( object->GetLuaObjectId() );
      object->GetTrigger()->AddHandler( Game::LuaObject_OnTrigger );
    } else {
      //__log.PrintInfo( Filelevel_ERROR, "Game::LuaObject_OnSetScript => function '%s' exists, but object '%s' is not trigger", GAME_OBJECT_HANDLER_ONTRIGGER.c_str(), object->GetNameFull().c_str() );
    }
  }
}//LuaObject_OnSetScript


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
        if( iter->luaFunctionId ) {
          game->lua->Unref( iter->luaFunctionId );
          iter->luaFunctionId = 0;
        }
        game->luaTimers.erase( iter );
        deleted = true;
        break;
      }
    }
  }
}//ClearLuaTimers



void Game::SetActive( bool setActive ) {
  this->isActive = setActive;
  //auto player = this->core->GetObject( "player" );
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
  if( game->isActive ) {
    for( auto &reference: this->luaObjectOnFrame ) {
      Engine::LuaObject::CallFunction( reference.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONFRAME );
    }
  }
  for( auto &reference: this->luaObjectOnFrameCore ) {
    Engine::LuaObject::CallFunction( reference.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONFRAMECORE );
  }
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
        if( !timer.funcName.empty() || timer.luaFunctionId ) {
          timerProcs.push_back( GameLuaTimer( timer.id, timer.funcName, timer.luaFunctionId ) );
        }
      }
    }
  }
  for( auto &timer: timerProcs ) {
    //LUACALLBACK_Timer( this->lua, timer.id, timer.funcName, timer.luaFunctionId );
    if( timer.luaFunctionId ) {
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) timer.id ) );
      game->lua->CallFunction( timer.luaFunctionId, &parameters );
      game->lua->Unref( timer.luaFunctionId );
    }
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
  if( obj->GetLuaObjectId() && !obj->GetLuaScript().empty() ) {
    Game::LuaObject_OnSetScript( obj );
  }
}//ObjectOnLoad




/*
=============
  ObjectOnUnload
=============
*/
void Game::ObjectOnUnload( Object* obj, bool isClean ) {
  if( isClean ) {
    if( obj->GetLuaObjectId() ) {
      //erase lua function references
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONUPDATE, game->luaObjectOnUpdate );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONUPDATECORE, game->luaObjectOnUpdateCore );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONFRAME, game->luaObjectOnFrame );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONFRAMECORE, game->luaObjectOnFrameCore );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONKEYPRESS, game->luaObjectOnKeyPress );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONKEYRELEASE, game->luaObjectOnKeyRelease );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONMOUSEKEYPRESS, game->luaObjectOnMouseKeyPress );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONMOUSEKEYRELEASE, game->luaObjectOnMouseKeyRelease );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONMOUSEMOVE, game->luaObjectOnMouseMove );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONCOLLISION, game->luaObjectOnCollision );
      game->EraseLuaHandler( obj->GetLuaObjectId(), GAME_OBJECT_HANDLER_ONTRIGGER, game->luaObjectOnTrigger );

      Engine::LuaObject::RemoveFromLuaTable( obj->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT );
      obj->SetLuaObjectId( 0 );
      __log.PrintInfo( Filelevel_DEBUG, "Game::ObjectOnUnload => object '%s' unloaded", obj->GetNameFull().c_str() );
    }
  } else {
    if( obj->GetLuaObjectId() ) {
      if( !Engine::LuaObject::CallFunction( obj->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONDESTROY ) ) {
        __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  obj->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONDESTROY.c_str() );
      }
    }
  }
}//ObjectOnUnload



void Game::ObjectOnDestroy( Object* obj ) {
  if( obj->GetLuaObjectId() ) {
    Game::ObjectOnUnload( obj, true );
  }
}//ObjectOnDestroy



void Game::OnRemoveObject( const std::string& objectName ) {
  Object *object = this->core->GetObject( objectName );
  if( !object ) {
    return;
  }

  //collision
  if( object->IsCollision() ) {
    Collision *collision = object->GetCollision();
    auto
      iter = this->collisionListeners.begin(),
      iterEnd = this->collisionListeners.end();
    for( ;iter != iterEnd; ++iter ) {
      if( iter->object == collision ) {
        this->collisionListeners.erase( iter );
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

  this->ObjectOnUnload( object, true );
}//OnRemoveObject


void Game::ObjectOnIsActive( Object* obj, bool isActive ) {
  if( isActive ) {
    game->world->AddActiveObject( obj );
  } else {
    game->world->RemoveActiveObject( obj );
  }
}//ObjectOnIsActive



void Game::EraseLuaHandler( const int luaReferenceId, const std::string &functionName, LuaFunctionHandlersList &handlersList ) {
  if( !luaReferenceId /* || !Engine::LuaObject::IsFunctionExists( luaReferenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, functionName ) */ ) {
    return;
  }

  auto
    iter = handlersList.begin(),
    iterEnd = handlersList.end();
  while( iter != iterEnd ) {
    if( iter->referenceId == luaReferenceId ) {
      handlersList.erase( iter );
      break;
    }
    ++iter;
  }
}//EraseLuaHandler




/*
=============
  KeyboardProc
=============
*/
void Game::KeyboardProc( Dword keyId, bool isPressed )
{
  //if( game->isActive )
  {
    if( isPressed ) {
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) keyId ) );
      for( auto &handler: game->luaObjectOnKeyPress ) {
        Engine::LuaObject::CallFunction( handler.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONKEYPRESS, &parameters );
      }
    } else {
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) keyId ) );
      for( auto &handler: game->luaObjectOnKeyRelease ) {
        Engine::LuaObject::CallFunction( handler.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONKEYRELEASE, &parameters );
      }
    }
  }
  /*
  if( !game->luaKeyboardListeners.empty() ) {
    luaKeyboardListenersList::iterator iter, iterEnd = game->luaKeyboardListeners.end();
    for( iter = game->luaKeyboardListeners.begin(); iter != iterEnd; ++iter ) {
      LUACALLBACK_ListenKeyboard( game->lua, *iter, keyId, isPressed );
    }
  }
  */
}//KeyboardProc


/*
=============
  MouseKeyProc
=============
*/
void Game::MouseKeyProc( Dword keyId, bool isPressed )
{
  //if( game->isActive )
  {
    if( isPressed ) {
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) keyId ) );
      for( auto &handler: game->luaObjectOnMouseKeyPress ) {
        Engine::LuaObject::CallFunction( handler.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEKEYPRESS, &parameters );
        /*
        bool supressCallbacks = game->lua->GetBoolean( -1, true );
        __log.PrintInfo( Filelevel_DEBUG, "Game::MouseKeyProc => press supressCallbacks[%d]", supressCallbacks );
        if( supressCallbacks ) {
          break;
        }
        */
      }
    } else {
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) keyId ) );
      __log.PrintInfo( Filelevel_DEBUG, "game->luaObjectOnMouseKeyRelease => %d:", game->luaObjectOnMouseKeyRelease.size() );
      for( auto &handler: game->luaObjectOnMouseKeyRelease ) {
        __log.PrintInfo( Filelevel_DEBUG, ". %d", handler.referenceId );
      }

      for( auto &handler: game->luaObjectOnMouseKeyRelease ) {
        Engine::LuaObject::CallFunction( handler.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEKEYRELEASE, &parameters );
        /*
        bool supressCallbacks = game->lua->GetBoolean( -1, true );
        __log.PrintInfo( Filelevel_DEBUG, "Game::MouseKeyProc => release supressCallbacks[%d]", supressCallbacks );
        if( supressCallbacks ) {
          break;
        }
        */
      }
    }
  }

  /*
  if( game->luaMouseKeyListeners.empty() ) {
    return;
  }
  luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseKeyListeners.end();
  for( iter = game->luaMouseKeyListeners.begin(); iter != iterEnd; ++iter ) {
    LUACALLBACK_ListenMouseKey( game->lua, *iter, keyId, isPressed );
  }
  */
}//MouseKeyProc


/*
=============
  MouseMoveProc
=============
*/
void Game::MouseMoveProc( const Vec2 &pos )
{
  //if( game->isActive )
  {
    Engine::LuaObject::FunctionCallParametersList parameters;
    parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) pos.x ) );
    parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) pos.y ) );
    for( auto &handler: game->luaObjectOnMouseMove ) {
      Engine::LuaObject::CallFunction( handler.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONMOUSEMOVE, &parameters );
    }
  }

  /*
  if( !game->luaMouseMoveListeners.empty() ) {
    luaKeyboardListenersList::iterator iter, iterEnd = game->luaMouseMoveListeners.end();
    for( iter = game->luaMouseMoveListeners.begin(); iter != iterEnd; ++iter ) {
      LUACALLBACK_ListenMouseMove( game->lua, *iter, pos );
    }
  }
  */
}//MouseMoveProc



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
  if( game->collisionListeners.empty() ) {
    return;
  }
  if( !objectA->GetLuaObjectId() ) {
    objectA->InitLuaUserData();
  }
  if( !objectB->GetLuaObjectId() ) {
    objectB->InitLuaUserData();
  }
  int objectAluaObjectId = objectA->GetLuaObjectId();
  int objectBluaObjectId = objectB->GetLuaObjectId();
  bool bIsStatic = b->IsStatic();
  for( auto &listener: game->luaObjectOnCollision ) {
    if( listener.referenceId == objectAluaObjectId ) {
      /*
      if( !iter->luaHandlerId && !iter->funcName.empty() ) {
        LUACALLBACK_ListenCollision( game->lua, iter->funcName, objectA->GetNameFull(), objectB->GetNameFull(), flags, velocity );
      }
      */
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( Engine::LUAOBJECT_PARAMETERS_LIST::LUAOBJECT_PARAMETER_REFERENCE, objectBluaObjectId ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( ( int ) flags ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( velocity.x ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( velocity.y ) );
      for( auto &luaHandler: game->luaObjectOnCollision ) {
        if( luaHandler.referenceId == objectAluaObjectId ) {
          if( !Engine::LuaObject::CallFunction( objectAluaObjectId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONCOLLISION, &parameters ) ) {
            __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  objectA->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONCOLLISION.c_str() );
          }
        }
      }
    }//a
    if( objectBluaObjectId && bIsStatic ) {
      int inverseFlags = ( ( flags & (1|4) ) ? flags ^ (1|4) : 0 ) | ( ( flags & (2|8) ) ? flags ^ (2|8) : 0 );
      //__log.PrintInfo( Filelevel_DEBUG, "OnCollision => static, flags[%d] inverseFlags[%d] script[%s]", flags, inverseFlags, objectB->GetLuaScript().c_str() );
      Engine::LuaObject::FunctionCallParametersList parameters;
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( Engine::LUAOBJECT_PARAMETERS_LIST::LUAOBJECT_PARAMETER_REFERENCE, objectAluaObjectId ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( inverseFlags ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( velocity.x ) );
      parameters.push_back( Engine::LuaObject::FunctionCallParameter( velocity.y ) );
      if( !Engine::LuaObject::CallFunction( objectBluaObjectId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONCOLLISION, &parameters ) ) {
        __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  objectB->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONCOLLISION.c_str() );
      }
    }//b
  }//for
}//CollisionProc




int Game::IsListeningCollision( Object *object ) {
  int luaObjectId = object->GetLuaObjectId();
  if( !luaObjectId ) {
    return 0;
  }
  for( auto &listener: this->luaObjectOnCollision ) {
    if( listener.referenceId == luaObjectId ) {
      return luaObjectId;
    }
  }
  return 0;
}//IsListeningCollision




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

  int luaReferenceId = objectA->GetLuaObjectId();
  if( !objectB->GetLuaObjectId() ) {
    objectB->InitLuaUserData();
  }
  Engine::LuaObject::FunctionCallParametersList parameters;
  parameters.push_back( Engine::LuaObject::FunctionCallParameter( Engine::LUAOBJECT_PARAMETERS_LIST::LUAOBJECT_PARAMETER_REFERENCE, objectB->GetLuaObjectId() ) );
  parameters.push_back( Engine::LuaObject::FunctionCallParameter( isInTrigger ) );
  for( auto& luaHandler: game->luaObjectOnTrigger ) {
    if( luaReferenceId == luaHandler.referenceId ) {
      if( !Engine::LuaObject::CallFunction( luaReferenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONTRIGGER, &parameters ) ) {
        __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  objectA->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONTRIGGER.c_str() );
      }
      break;
    }
  }
  /*
  for( iter = game->luaTriggerListeners.begin(); iter != iterEnd; ++iter ) {
    if( iter->object == trigger ) {
      LUACALLBACK_ListenTrigger( game->lua, iter->funcName, objectA->GetNameFull(), objectB->GetNameFull(), isInTrigger );
    }
  }
  */
}//TriggerProc





void Game::OnCollisionMgrUpdate() {
  //lua Object.OnUpdate
  if( game->isActive && !game->luaObjectOnUpdate.empty() ) {
    for( auto &reference: game->luaObjectOnUpdate ) {
      Engine::LuaObject::CallFunction( reference.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONUPDATE );
    }
  }

  //lua Object.OnUpdateCore
  if( !game->luaObjectOnUpdateCore.empty() ) {
    for( auto &reference: game->luaObjectOnUpdateCore ) {
      Engine::LuaObject::CallFunction( reference.referenceId, Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONUPDATECORE );
    }
  }
}//OnCollisionMgrUpdate


void Game::LuaObject_OnTrigger( ObjectTrigger* trigger, Collision* collision, bool isInTrigger ) {
  Object *objectA = game->core->GetObjectByTrigger( trigger );
  if( !objectA ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::TriggerProc => object A by trigger x%p not found", trigger );
    return;
  }

  Object *objectB = game->core->GetObjectByCollision( collision );
  if( !objectB ) {
    __log.PrintInfo( Filelevel_ERROR, "Game::TriggerProc => object B by collision x%p not found", collision );
    return;
  }

  //__log.PrintInfo( Filelevel_DEBUG, "Game::LuaObject_OnTrigger => objectA['%s'] GetLuaObjectId[%d]", objectA->GetNameFull().c_str(), objectA->GetLuaObjectId() );
  //__log.PrintInfo( Filelevel_DEBUG, "Game::LuaObject_OnTrigger => objectB['%s'] GetLuaObjectId[%d]", objectB->GetNameFull().c_str(), objectB->GetLuaObjectId() );
  if( !objectA->GetLuaObjectId() ) {
    objectA->InitLuaUserData();
    //__log.PrintInfo( Filelevel_DEBUG, "Game::LuaObject_OnTrigger => objectA initializing userdata" );
  }

  if( !objectB->GetLuaObjectId() ) {
    objectB->InitLuaUserData();
    //__log.PrintInfo( Filelevel_DEBUG, "Game::LuaObject_OnTrigger => objectB initializing userdata" );
  }

  Engine::LuaObject::FunctionCallParametersList parameters;
  parameters.push_back( Engine::LuaObject::FunctionCallParameter( Engine::LUAOBJECT_PARAMETERS_LIST::LUAOBJECT_PARAMETER_REFERENCE, objectB->GetLuaObjectId() ) );
  parameters.push_back( Engine::LuaObject::FunctionCallParameter( !isInTrigger ) );
  if( !Engine::LuaObject::CallFunction( objectA->GetLuaObjectId(), Engine::LUAOBJECT_LIBRARIESLIST::LUAOBJECT_LIBRARY_OBJECT, GAME_OBJECT_HANDLER_ONTRIGGER, &parameters ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Engine::LuaObject::CallFunction => %s:%s failed",  objectA->GetNameFull().c_str(), GAME_OBJECT_HANDLER_ONTRIGGER.c_str() );
  }
}//LuaObject_OnTrigger
