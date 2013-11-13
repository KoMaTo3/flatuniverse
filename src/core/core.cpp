#include <string>
#include <vector>
#include <time.h>


#include "core.h"
#include "tools.h"

#define INTERNGL_H_PREFIX
#define INTERNGL_H_POSTFIX = NULL
#include "interngl.h"

#include "file.h"
#include "config.h"
#include "debugrenderer.h"
#include "animationmanager.h"
#include "objectwidget.h"

#pragma comment( lib, "opengl32.lib" )

//
CoreRenderableList *__coreRenderableList = NULL;                      //список рендерейблов, отправляемых на рендер
CoreRenderableListIndicies  *__coreRenderableListIndicies = NULL;     //индексы рендерейблов
CoreRenderableListIndicies  *__coreRenderableListFreeIndicies = NULL; //свободные индексы рендерейблов

CoreRenderableList *__coreGUI = NULL;                                 //список рендерейблов GUI
CoreRenderableListIndicies  *__coreGUIIndicies = NULL;                //индексы рендерейблов GUI
CoreRenderableListIndicies  *__coreGUIFreeIndicies = NULL;            //свободные индексы рендерейблов GUI

//GuiList *__guiList  = NULL; //Glui2 элементы
//Glui2   *__coreGlui = NULL; //указатель на объект Glui2
//

ConfigFile* __config = NULL;

extern DebugRenderer* __debugRender;
LightRenderer *__lightLenderer = NULL;


class LightBlock: public LightMap::ILightBlock {
public:
  Vec2 position;
  Vec2
    size,
    lastSize;
  Vec2 halfSize;
  const float epsilon;

  LightBlock( const Vec2& setPosition, const Vec2& setSize )
  :position( setPosition ), size( setSize ), halfSize( setSize * 0.5f ), epsilon( 1.0f ), lastSize( -setSize )
  {
  }

  virtual inline const Vec2& GetPosition() const {
    return this->position;
  }
  virtual inline const Vec2& GetSize() const {
    return this->size;
  }
  virtual inline const Vec2& GetHalfSize() const {
    return this->halfSize;
  }

  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
    //default
    if( lightPosition.x < this->position.x + this->halfSize.x ) { //add right edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x + this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x + this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.x > this->position.x - this->halfSize.x ) { //add left edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x - this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.y < this->position.y + this->halfSize.y ) { //add bottom edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y + this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y + this->halfSize.y ), cache );
    }
    if( lightPosition.y > this->position.y - this->halfSize.y ) { //add top edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y - this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y - this->halfSize.y ), cache );
    }

    /*/darkness
    if( lightPosition.x < this->position.x - this->halfSize.x ) { //add right edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x - this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ) );
    }
    if( lightPosition.x > this->position.x + this->halfSize.x ) { //add left edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x + this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x + this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ) );
    }
    if( lightPosition.y < this->position.y - this->halfSize.y ) { //add bottom edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y - this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y - this->halfSize.y ) );
    }
    if( lightPosition.y > this->position.y + this->halfSize.y ) { //add top edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y + this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y + this->halfSize.y ) );
    }
    */
  }

  virtual void SetPosition( const Vec2& setPosition ) {
    this->position = setPosition;
  }

  void SetSize( const Vec2& setSize ) {
    this->size = setSize;
  }

  virtual void Update() {
    if( this->lastSize != this->size ) {
      this->halfSize = this->size * 0.5f;
      this->lastSize = this->size;
    }
  }

  virtual void Redraw() {
    glBegin( GL_QUADS );
    Vec2 v2 = this->GetPosition();
    Vec2 sz = this->GetHalfSize();

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( v2.x - sz.x, v2.y - sz.y, 0.0f );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( v2.x + sz.x, v2.y - sz.y, 0.0f );

    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( v2.x + sz.x, v2.y + sz.y, 0.0f );

    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( v2.x - sz.x, v2.y + sz.y, 0.0f );
    glEnd();
  }
};



Core::Core()
:_state( CORE_STATE_UNKNOWN ), _rootObject( NULL ), /*_rootGUIObject( NULL ), */ collisionManager( NULL ), triggerManager( NULL ), camera( NULL ), animationMgr( NULL )
{
  this->_window.isActive  = true;
  this->_window.dc        = NULL;
  this->_window.hwnd      = NULL;
  this->_window.glRC      = NULL;
  this->_window.palette   = NULL;

  this->_buffers.vbo      = NULL;

  this->_build.time = __TIME__;
  this->_build.date = __DATE__;

  this->_rootObject = new Object( "root", NULL );
  this->SetCamera( this->_rootObject );

  this->debug.renderRenderable  = false;
  this->debug.renderCollision   = false;
  this->debug.renderTrigger     = false;
  //this->debug.selectedObject    = NULL;

  //this->gui.context = NULL;
  //this->gui.show    = true;
}//constructor



Core::~Core()
{
}//destructor




/*
=============
  Destroy
=============
*/
bool Core::Destroy()
{
  Animation::Destroy();
  DEF_DELETE( this->_rootObject );
  //DEF_DELETE( this->_rootGUIObject );
  DEF_DELETE( this->collisionManager );
  DEF_DELETE( this->triggerManager );
  DEF_DELETE( this->animationMgr );
  DEF_DELETE( this->lightList );
  DEF_DELETE( this->lightRenderer );
  __lightLenderer = NULL;
  //DEF_DELETE( this->gui.context );
  DEF_DELETE( __coreRenderableList );
  DEF_DELETE( __coreRenderableListIndicies );
  DEF_DELETE( __coreRenderableListFreeIndicies );
  DEF_DELETE( __coreGUI );
  DEF_DELETE( __coreGUIIndicies );
  DEF_DELETE( __coreGUIFreeIndicies );
  DEF_DELETE( __fileManager );
  DEF_DELETE( __textureAtlas );
  DEF_DELETE( __triggerList );
  DEF_DELETE( __objectByCollision );
  DEF_DELETE( __objectByTrigger );
  ObjectWidget::Destroy();
  glDeleteBuffers( 1, &this->_buffers.vbo );
  glDeleteBuffers( 1, &this->_buffers.ibo );
  glDeleteBuffers( 1, &this->_buffers.iboGUI );
  glDeleteVertexArrays( 1, &this->_buffers.vao );
  glDetachShader( this->_shaders.programm, this->_shaders.vertex );
  glDetachShader( this->_shaders.programm, this->_shaders.fragment );
  glDetachShader( this->_shaders.programm, this->_shaders.geometry );
  glDeleteShader( this->_shaders.vertex );
  glDeleteShader( this->_shaders.fragment );
  glDeleteShader( this->_shaders.geometry );
  glDeleteProgram( this->_shaders.programm );
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( this->_window.glRC );
  this->_window.glRC = NULL;
  ReleaseDC( this->_window.hwnd, this->_window.dc );
  this->_window.dc = NULL;
  //DEF_DELETE( __objectByGui );
  //DEF_DELETE( __guiList );

  return true;
}//Destroy




/*
=============
  SetState
=============
*/
void Core::SetState( CoreStates newState )
{
  CoreStates oldState = this->_state;
  this->_state = newState;
  switch( oldState )  //в зависимости от перехода из состояние в состояние возможны какие-то действия
  {
  case CORE_STATE_UNDEFINED:
  break;
  }
}//SetState




/*
=============
  Init
=============
*/
bool Core::Init( WORD screenWidth, WORD screenHeight, bool isFullScreen, const std::string &windowName )
{
  srand( ( Word ) time( 0 ) );
  Math::Init();

  char tmp[ 1024 ];
  GetModuleFileName( GetModuleHandle( NULL ), tmp, 1024 );
  std::string path = tools::GetPathFromFilePath( tmp );
  tools::SetCurDirectory( path.c_str() );

  __log.EnableAutoFlush( true );
  __log.Open( "logs/core.txt", File_mode_WRITE, Filelevel_ALL );

  SYSTEMTIME curTime;
  GetSystemTime( &curTime );
  __log.PrintUseTime( "Application started at %d.%d.%d\n", curTime.wDay, curTime.wMonth, curTime.wYear );
  __log.PrintUseTime( "Core build at %s %s\n", this->_build.date.c_str(), this->_build.time.c_str() );
  __log.PrintInfo( Filelevel_INFO, "Current directory: %s", path.c_str() );

  __fileManager = new FileManager();
  __fileManager->ScanAll();

  __config = new ConfigFile();
  __config->LoadFromFile( "data/default.cfg" );
  //__config->Dump();

  sTimer.UpdateFreq();
  sTimer.UpdateCounter();
  if( this->_window.hwnd )
    return false;
  //create window
  this->SetState( CORE_STATE_INIT );

  WNDCLASSEX wndClass;
  memset( &wndClass, 0, sizeof( wndClass ) );
  std::string className = "flat-universe-engine";

  wndClass.cbSize       = sizeof( wndClass );
  wndClass.style        = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc  = WindowProc;
  wndClass.cbClsExtra   = 0;
  wndClass.cbWndExtra   = 0 | DLGWINDOWEXTRA;
  wndClass.hInstance    = GetModuleHandle( NULL );
  wndClass.hIcon        = LoadIcon( NULL, IDI_APPLICATION );
  wndClass.hCursor      = 0;//LoadCursor( NULL, IDC_ARROW );
  wndClass.hbrBackground = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
  wndClass.lpszMenuName = 0;
  wndClass.lpszClassName = className.c_str();
  wndClass.hIconSm = 0;
  if( !( RegisterClassEx( &wndClass ) ) )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  if( !screenWidth )
  {
    screenWidth   = ( WORD ) __config->GetNumber( "gl_screen_width", 800 );
    screenHeight  = ( WORD ) __config->GetNumber( "gl_screen_height", 600 );
  }

  DWORD style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;  //WS_POPUP for fullscreen
  DWORD styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //WS_EX_APPWINDOW for fullscreen
  RECT windowRect;
  windowRect.left   = 0;
  windowRect.top    = 0;
  windowRect.right  = screenWidth;
  windowRect.bottom = screenHeight;
  AdjustWindowRectEx( &windowRect, style, false, styleEx );

  this->_window.windowSize.width  = screenWidth;
  this->_window.windowSize.height = screenHeight;

  this->_window.hwnd = CreateWindowEx(
    styleEx,
    className.c_str(),
    windowName.c_str(),
    style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
    ( int ) __config->GetNumber( "gl_screen_x", 900 ),
    ( int ) __config->GetNumber( "gl_screen_y", 700 ),
    windowRect.right - windowRect.left,
    windowRect.bottom - windowRect.top,
    0,
    NULL,
    GetModuleHandle( NULL ),
    NULL
  );
  //this->_wndSize = Pos<Dword>( windowWidth, windowHeight );

  SetWindowLong( this->_window.hwnd, GWL_USERDATA, LONG( this ) );

  ShowWindow  ( this->_window.hwnd, SW_NORMAL );
  UpdateWindow( this->_window.hwnd );

  //if( !this->_rootGUIObject )
  //  this->_rootGUIObject = new Object( "rootGUI", NULL );
  this->mouse.Init( Size( screenWidth, screenHeight ) );

  //
  ObjectWidget::Init();
  __coreRenderableList    = new CoreRenderableList();
  __coreRenderableListIndicies = new CoreRenderableListIndicies();
  __coreRenderableListFreeIndicies = new CoreRenderableListIndicies();
  __coreGUI               = new CoreRenderableList();
  __coreGUIIndicies       = new CoreRenderableListIndicies();
  __coreGUIFreeIndicies   = new CoreRenderableListIndicies();
  __textureAtlas          = new TextureAtlas();
  __triggerList           = new ObjectTriggerList();
  this->collisionManager  = new CollisionManager();
  this->triggerManager    = new ObjectTriggerManager();
  __objectByCollision     = new ObjectByCollisionList();
  __objectByTrigger       = new ObjectByTriggerList();
  this->animationMgr      = new Animation::Manager(  );
  this->lightList         = new LightList;
  //__objectByGui           = new ObjectByGuiList();
  //__guiList               = new GuiList();
  //

  this->_window.windowToWorld.Set( 100.0f / float( screenWidth ), 100.0f / float( screenHeight ), 1.0f );
  this->_window.windowCenter.Set( screenWidth * 0.5f, screenHeight * 0.5f );

  if( !this->_InitGraphics() )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  __log.PrintInfo( Filelevel_DEBUG, "new LightRenderer..." );
  this->lightRenderer     = new LightRenderer(
    Vec2( float( this->_window.windowSize.width ), float( this->_window.windowSize.height ) ),
    "data/shaders/shader0.vs",
    "data/shaders/shader0.fs",
    "data/shaders/shader2.vs",
    "data/shaders/shader2.fs",
    this->lightList
    );
  this->lightRenderer->GetLightManager()->lightAmbient.Set( 0.3f, 0.3f, 1.0f, 0.7f );
  this->lightRenderer->GetLightManager()->lightList->push_back( new LightMap::LightEntity( LightMap::LT_POINT, Vec2( -100.0f, 100.0f ), Vec4( 1.0f, 0.6f, 0.6f, 1.0f ), Vec2( 300.0f, 300.0f ), 0.3f, 1024 ) );
  this->lightRenderer->GetLightManager()->lightList->push_back( new LightMap::LightEntity( LightMap::LT_POINT, Vec2( 350.0f, -40.0f ), Vec4( 0.2f, 1.0f, 0.2f, 1.0f ), Vec2( 400.0f, 300.0f ), 0.3f, 1024 ) );
  //this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 200.0f, 50.0f ), Vec2( 20.0f, 20.0f ) ) );
  __lightLenderer = this->lightRenderer;

  FileManager::FilesList animationFiles;
  __fileManager->FindFiles( "ani", animationFiles );
  if( animationFiles.size() ) {
    for( auto &fileName: animationFiles ) {
      __log.PrintInfo( Filelevel_DEBUG, "Animation template files list: %s", fileName.c_str() );
      this->animationMgr->LoadFile( fileName );
    }
  }
  //MessageBox(0,"ok",0,0);

  //this->gui.context = new Glui2( "g2Blue.cfg", NULL, NULL, Core::_GluiKeyboardFunc, NULL, Core::_GluiMouseFunc, Core::_GluiHoverFunc );
  GL_CHECK_ERROR;
  //this->gui.context->WindowWidth  = this->_window.windowSize.width;
  //this->gui.context->WindowHeight = this->_window.windowSize.height;
  //__coreGlui = this->gui.context;

  this->SetState( CORE_STATE_RUN );
  return true;
}//Init



/*
=============
  _InitGraphics
  Инициализация OpenGL
=============
*/
bool Core::_InitGraphics()
{
  if( !( this->_window.dc = GetDC( this->_window.hwnd ) ) )
  {
    __log.PrintInfo( Filelevel_CRITICALERROR, "Core::_InitGraphics => GetDC failed" );
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
    //__log.PrintInfo( Filelevel_CRITICALERROR, "Core::InitGraphics => GetDC" );
  this->_InitPixelFormat();
  this->_InitPalette();
  //__log.PrintInfo( Filelevel_INFO, "OpenGL version: %s", ( version ? version : "unknown" ) );

#if defined(GL_VERSION_4_1)
  //__log.PrintInfo( Filelevel_INFO, "OpenGL 4.1 detected" );
#endif

  if( !( this->_window.glRC = wglCreateContext( this->_window.dc ) ) )
  {
    __log.PrintInfo( Filelevel_CRITICALERROR, "Core::_InitGraphics => wglCreateContext failed" );
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  //__log.PrintInfo( Filelevel_CRITICALERROR, "Core::InitGraphics => wglCreateContext" );
  wglMakeCurrent( this->_window.dc, this->_window.glRC );
  this->_InitViewport();

  const char* version = ( const char* ) ::glGetString( GL_VERSION );
  GL_CHECK_ERROR;

  //check variable parameters
  //GLint i = 0;
  //::glGetIntegerv( GL_MAX_TEXTURE_SIZE, &i );
  //__log.PrintInfo( Filelevel_DEBUG, "Maximum texture size: %d", i );

  //this->_vertices.insert( pair<Texture*, PolyVertex>( 0, PolyVertex() ) );
  //this->_vertices[ 0 ].Allocate( 0 ); //добавляем в список вершинных буфферов буффер, связанный с NULL-текстурой
                                      //(допустим, debug-спрайт рендерится без текстуры)

  this->_InitExtensions();

  //shaders
  this->_shaders.vertex   = glCreateShader( GL_VERTEX_SHADER );
  GL_CHECK_ERROR;
  this->_shaders.geometry = glCreateShader( GL_GEOMETRY_SHADER );
  GL_CHECK_ERROR;
  this->_shaders.fragment = glCreateShader( GL_FRAGMENT_SHADER );
  GL_CHECK_ERROR;
  memory
    memShaderVertex,
    memShaderGeometry,
    memShaderFragment;
  GLint
    lengthShaderVertex,
    lengthShaderGeometry,
    lengthShaderFragment;
  __fileManager->GetFile( __config->GetString( "gl_shader_vertex", "data/shaders/main.vs" ), memShaderVertex );
  __fileManager->GetFile( __config->GetString( "gl_shader_geometry", "data/shaders/main.gs" ), memShaderGeometry );
  __fileManager->GetFile( __config->GetString( "gl_shader_fragment", "data/shaders/main.fs" ), memShaderFragment );
  lengthShaderVertex    = memShaderVertex.getLength();
  lengthShaderGeometry  = memShaderGeometry.getLength();
  lengthShaderFragment  = memShaderFragment.getLength();
  const GLchar *sourceShaderVertex    = memShaderVertex.getData();
  const GLchar* sourceShaderGeometry  = memShaderGeometry.getData();
  const GLchar* sourceShaderFragment  = memShaderFragment.getData();
  glShaderSource( this->_shaders.vertex,    1, ( const GLchar** ) &sourceShaderVertex, &lengthShaderVertex );
  GL_CHECK_ERROR;
  glShaderSource( this->_shaders.geometry,  1, ( const GLchar** ) &sourceShaderGeometry, &lengthShaderGeometry );
  GL_CHECK_ERROR;
  glShaderSource( this->_shaders.fragment,  1, ( const GLchar** ) &sourceShaderFragment, &lengthShaderFragment );
  GL_CHECK_ERROR;
  glCompileShader( this->_shaders.vertex );
  GL_CHECK_ERROR;
  glCompileShader( this->_shaders.geometry );
  GL_CHECK_ERROR;
  glCompileShader( this->_shaders.fragment );
  GL_CHECK_ERROR;
  if( this->_CheckShaderError( "Vertex shader", this->_shaders.vertex ) ) {
    this->SetState( CORE_STATE_EXIT );
  }
  if( this->_CheckShaderError( "Geometry shader", this->_shaders.geometry ) ) {
    this->SetState( CORE_STATE_EXIT );
  }
  if( this->_CheckShaderError( "Fragment shader", this->_shaders.fragment ) ) {
    this->SetState( CORE_STATE_EXIT );
  }
  this->_shaders.programm = glCreateProgram();
  GL_CHECK_ERROR;
  glAttachShader( this->_shaders.programm, this->_shaders.vertex );
  GL_CHECK_ERROR;
  glAttachShader( this->_shaders.programm, this->_shaders.geometry );
  GL_CHECK_ERROR;
  glAttachShader( this->_shaders.programm, this->_shaders.fragment );
  GL_CHECK_ERROR;
  glLinkProgram( this->_shaders.programm );
  GL_CHECK_ERROR;

  //this->_shaders.locVertexPos = glGetAttribLocation( this->_shaders.programm, "vertexPosition_local" );

  /*
  this->_shaders.locVertexPos = 0;
  glBindAttribLocation( this->_shaders.programm, this->_shaders.locVertexPos, "vertexPosition_local" );
  GL_CHECK_ERROR;
  //this->_shaders.locColor     = glGetAttribLocation( this->_shaders.programm, "colorLocal" );
  this->_shaders.locColor = 1;
  glBindAttribLocation( this->_shaders.programm, this->_shaders.locColor, "colorLocal" );
  GL_CHECK_ERROR;
  //this->_shaders.locSize      = glGetAttribLocation( this->_shaders.programm, "size_local" );
  this->_shaders.locSize = 2;
  glBindAttribLocation( this->_shaders.programm, this->_shaders.locSize, "size_local" );
  GL_CHECK_ERROR;
  //this->_shaders.locTexCoords = glGetAttribLocation( this->_shaders.programm, "texcoordLocal" );
  this->_shaders.locTexCoords = 3;
  glBindAttribLocation( this->_shaders.programm, this->_shaders.locTexCoords, "texcoordLocal" );
  GL_CHECK_ERROR;
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => locVertexPos[ %d ]", this->_shaders.locVertexPos );
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => locColor[ %d ]", this->_shaders.locColor );
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => locSize[ %d ]", this->_shaders.locSize );
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => locTexCoords[ %d ]", this->_shaders.locTexCoords );
  */

  this->_shaders.matrModelLoc = glGetUniformLocation( this->_shaders.programm, "matModel" );
  GL_CHECK_ERROR;
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => matrModelLoc[ %d ]", this->_shaders.matrModelLoc );
  this->_shaders.matrProjectionLoc = glGetUniformLocation( this->_shaders.programm, "matProj" );
  GL_CHECK_ERROR;
  __log.PrintInfo( Filelevel_DEBUG, "Core::_InitGraphics => matrProjectionLoc[ %d ]", this->_shaders.matrProjectionLoc );

  glGenBuffers( 1, &this->_buffers.vbo );
  GL_CHECK_ERROR;
  glGenBuffers( 1, &this->_buffers.ibo );
  GL_CHECK_ERROR;
  glGenBuffers( 1, &this->_buffers.iboGUI );
  GL_CHECK_ERROR;
  glGenVertexArrays( 1, &this->_buffers.vao );
  GL_CHECK_ERROR;

  if( __textureAtlas )
  {
    GLint texSize;
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &texSize );
    GL_CHECK_ERROR;
    texSize = min( texSize, 1024 );
    __textureAtlas->Init( Size( texSize, texSize ), 1 );
    GL_CHECK_ERROR;
  }

  return true;
}//_InitGraphics




/*
=============
  _CheckShaderError
=============
*/
bool Core::_CheckShaderError( const std::string& text, GLuint shader )
{
  bool result = false;
  if( !glIsShader( shader ) )
    return true;
  Int maxLength, logLength = 0;
  glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
  GL_CHECK_ERROR;
  char *log = new Char[ maxLength + 1 ];
  log[ maxLength ] = 0;
  glGetShaderInfoLog( shader, maxLength, &logLength, log );
  if( log[ 0 ] && this->CheckGLError( __LINE__, __FILE__ ) )
  {
    std::string tmpLog = log;
    BYTE errorType = Filelevel_INFO;
    if( tmpLog.find_first_of( "warning" ) >= 0 )
      errorType = Filelevel_WARNING;
    else
      errorType = Filelevel_CRITICALERROR;
    __log.PrintInfo( errorType, "Core::_CheckShaderError => %s: %s", text.c_str(), log );
    //MessageBox(0,log,text.c_str(),0);
    result = true;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "%s: %s", text.c_str(), log );
  delete [] log;
  return result;
}//_CheckShaderError



/*
=============
  IsExtensionExist
=============
*/
bool Core::IsExtensionExist( const std::string extensionName )
{
  //for( unsigned long num = 0; num < this->_extensions.size(); ++num )
  for( auto iter = this->_extensions.begin(); iter != this->_extensions.end(); ++iter )
    if( *iter == extensionName )
      return true;
  return false;
}//IsExtensionExist




/*
=============
  _InitExtensions
=============
*/
void Core::_InitExtensions()
{
  //GL extensions
  this->_extensions = tools::Explode( std::string( ( const char* ) glGetString( GL_EXTENSIONS ) ), " " );
  GL_CHECK_ERROR;
  //__log.PrintInfo( Filelevel_INFO, "OpenGL extensions:\n. %s\n", Implode( this->_extensions, "\n. " ).c_str() );

  /* WGL extensions */
  if( this->IsExtensionExist( "WGL_EXT_extensions_string" ) )
  {
    ::wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress ( "wglGetExtensionsStringARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => wglGetExtensionsStringARB = x%X", wglGetExtensionsStringARB );
    std::vector< std::string > ext = tools::Explode( std::string( ( const char* ) ::wglGetExtensionsStringARB( this->_window.dc ) ), " " );
    //__log.PrintInfo( Filelevel_INFO, "WGL extensions:\n. %s", Implode( ext, "\n. " ).c_str() );
  }

  /* VBO */
  if( this->IsExtensionExist( "GL_ARB_vertex_buffer_object" ) )
  {
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress ( "glBindBufferARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glBindBufferARB = x%X", glBindBufferARB );

    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress ( "glDeleteBuffersARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glDeleteBuffersARB = x%X", glDeleteBuffersARB );

    glGenBuffersARB = (PFNGLGENBUFFERSPROC) wglGetProcAddress ( "glGenBuffersARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glGenBuffersARB = x%X", glGenBuffersARB );

    glIsBufferARB = (PFNGLISBUFFERARBPROC) wglGetProcAddress ( "glIsBufferARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glIsBufferARB = x%X", glIsBufferARB );

    glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress ( "glBufferDataARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glBufferDataARB = x%X", glBufferDataARB );

    glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress ( "glBufferSubDataARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glBufferSubDataARB = x%X", glBufferSubDataARB );

    glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC) wglGetProcAddress ( "glGetBufferSubDataARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glGetBufferSubDataARB = x%X", glGetBufferSubDataARB );

    glMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress ( "glMapBufferARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glMapBufferARB = x%X", glMapBufferARB );

    glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress ( "glUnmapBufferARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glUnmapBufferARB = x%X", glUnmapBufferARB );

    glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC) wglGetProcAddress ( "glGetBufferParameterivARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glGetBufferParameterivARB = x%X", glGetBufferParameterivARB );

    glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC) wglGetProcAddress ( "glGetBufferPointervARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glGetBufferPointervARB = x%X", glGetBufferPointervARB );
  }
  else
    ;//__log.PrintInfo( Filelevel_CRITICALERROR, "Not found OpenGL extension 'GL_ARB_vertex_buffer_object'" );


  /* MultiTexture */
  if( this->IsExtensionExist( "GL_ARB_multitexture" ) )
  {
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress ( "glActiveTextureARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glActiveTextureARB = x%X", glActiveTextureARB );

    glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress ( "glClientActiveTextureARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glClientActiveTextureARB = x%X", glClientActiveTextureARB );

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress ( "glMultiTexCoord2fARB" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => glMultiTexCoord2fARB = x%X", glMultiTexCoord2fARB );
  }
  else
    ;//__log.PrintInfo( Filelevel_CRITICALERROR, "Not found OpenGL extension 'GL_ARB_multitexture'" );

  this->LoadExtension( "glAttachShader", ( void** ) &glAttachShader );
  this->LoadExtension( "glBindAttribLocation", ( void** ) &glBindAttribLocation );
  this->LoadExtension( "glCompileShader", ( void** ) &glCompileShader );
  this->LoadExtension( "glCreateProgram", ( void** ) &glCreateProgram );
  this->LoadExtension( "glCreateShader", ( void** ) &glCreateShader );
  this->LoadExtension( "glDeleteProgram", ( void** ) &glDeleteProgram );
  this->LoadExtension( "glDeleteShader", ( void** ) &glDeleteShader );
  this->LoadExtension( "glDetachShader", ( void** ) &glDetachShader );
  this->LoadExtension( "glIsProgram", ( void** ) &glIsProgram );
  this->LoadExtension( "glIsShader", ( void** ) &glIsShader );
  this->LoadExtension( "glLinkProgram", ( void** ) &glLinkProgram );
  this->LoadExtension( "glShaderSource", ( void** ) &glShaderSource );
  this->LoadExtension( "glUseProgram", ( void** ) &glUseProgram );
  this->LoadExtension( "glUniform1f", ( void** ) &glUniform1f );
  this->LoadExtension( "glUniform2f", ( void** ) &glUniform2f );
  this->LoadExtension( "glUniform3f", ( void** ) &glUniform3f );
  this->LoadExtension( "glUniform4f", ( void** ) &glUniform4f );
  this->LoadExtension( "glUniform4fv", ( void** ) &glUniform4fv );
  this->LoadExtension( "glUniform1i", ( void** ) &glUniform1i );
  this->LoadExtension( "glUniformMatrix4fv", ( void** ) &glUniformMatrix4fv );
  this->LoadExtension( "glVertexAttrib1f", ( void** ) &glVertexAttrib1f );
  this->LoadExtension( "glVertexAttrib2f", ( void** ) &glVertexAttrib2f );
  this->LoadExtension( "glVertexAttrib3f", ( void** ) &glVertexAttrib3f );
  this->LoadExtension( "glVertexAttrib4f", ( void** ) &glVertexAttrib4f );
  this->LoadExtension( "glGetObjectParameterfvARB", ( void** ) &glGetObjectParameterfvARB );
  this->LoadExtension( "glGetObjectParameterivARB", ( void** ) &glGetObjectParameterivARB );
  this->LoadExtension( "glGetInfoLogARB", ( void** ) &glGetInfoLogARB );
  this->LoadExtension( "glGetShaderiv", ( void** ) &glGetShaderiv );
  this->LoadExtension( "glGetShaderInfoLog", ( void** ) &glGetShaderInfoLog );
  this->LoadExtension( "glGetUniformLocation", ( void** ) &glGetUniformLocation );
  this->LoadExtension( "glGenBuffers", ( void** ) &glGenBuffers );
  this->LoadExtension( "glBindBuffer", ( void** ) &glBindBuffer );
  this->LoadExtension( "glBufferData", ( void** ) &glBufferData );
  this->LoadExtension( "glGenVertexArrays", ( void** ) &glGenVertexArrays );
  this->LoadExtension( "glBindVertexArray", ( void** ) &glBindVertexArray );
  this->LoadExtension( "glEnableVertexAttribArray", ( void** ) &glEnableVertexAttribArray );
  this->LoadExtension( "glDisableVertexAttribArray", ( void** ) &glDisableVertexAttribArray );
  this->LoadExtension( "glVertexAttribPointer", ( void** ) &glVertexAttribPointer );
  this->LoadExtension( "glActiveTexture", ( void** ) &glActiveTexture );
  this->LoadExtension( "glGetAttribLocation", ( void** ) &glGetAttribLocation );
  this->LoadExtension( "glGenFramebuffers", ( void** ) &glGenFramebuffers );
  this->LoadExtension( "glBindFramebuffer", ( void** ) &glBindFramebuffer );
  this->LoadExtension( "glFramebufferTexture", ( void** ) &glFramebufferTexture );
  this->LoadExtension( "glFramebufferTexture2D", ( void** ) &glFramebufferTexture2D );
  this->LoadExtension( "glCheckFramebufferStatus", ( void** ) &glCheckFramebufferStatus );
  this->LoadExtension( "glDeleteFramebuffers", ( void** ) &glDeleteFramebuffers );
  this->LoadExtension( "glDrawBuffers", ( void** ) &glDrawBuffers );
  this->LoadExtension( "glDeleteBuffers", ( void** ) &glDeleteBuffers );
  this->LoadExtension( "glDeleteVertexArrays", ( void** ) &glDeleteVertexArrays );


  /* WGL_EXT_swap_control */
  if( this->IsExtensionExist( "WGL_EXT_swap_control" ) )
  {
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress ( "wglSwapIntervalEXT" );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitExtensions => wglSwapIntervalEXT = x%X", wglSwapIntervalEXT );
  }
  else
    ;//__log.PrintInfo( Filelevel_WARNING, "Not found OpenGL extension 'WGL_EXT_swap_control'" );
}//_InitExtensions




/*
=============
  LoadExtension
=============
*/
bool Core::LoadExtension( const std::string &name, void** function )
{
  *function = wglGetProcAddress( name.c_str() );

  //if( !*function )
  //  __log.PrintInfo( Filelevel_CRITICALERROR, "Can't load extension %s", name.c_str() );

  return *function != NULL;
}//LoadExtension




/*
=============
  _InitViewport
=============
*/
void Core::_InitViewport()
{
  /* set viewing projection */
  //glViewport( 0, 0, 320, 200 );
  //glFrustum( -0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 10.0F );
  //glFrustum( -100.0F, 100.0F, -100.0F, 100.0F, -10.0F, 10.0F );
  //glOrtho( 0.0, 320.0, 200.0, 0.0, -10.0, 10.0 );

  //*
  //UPD 2012.12.12: это уже не нужно т.к. проекционная и мировая матрица передаются в шейдер
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, 100.0, 100.0, 0.0, 0.01, 10.0 ); // -10.0f - ближайшая к зрителю точка
  glMatrixMode( GL_MODELVIEW );
  GL_CHECK_ERROR;
  //*/

  /* position viewer */
  //float ambient[4] = {0.5, 0.5, 0.5, 1};

  /* light */
  //glEnable( GL_LIGHTING );
  //glLightModelfv(GL_AMBIENT_AND_DIFFUSE, ambient);
  //glEnable( GL_LIGHT0 );

  /*glEnable(GL_DOUBLEBUFFER);
  glDisable(GL_ALPHA_TEST);
  glEnable(GL_STENCIL_TEST);
  glDisable(GL_DITHER);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_LOGIC_OP);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glEnable(GL_POINT_SMOOTH);
  //glEnable(GL_LINE_SMOOTH);
  //glEnable(GL_POLYGON_SMOOTH);

  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);

  glShadeModel(GL_SMOOTH);
  glClearDepth(1.0f);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glPointSize(1.0f);
  glLineWidth(1.0f);*/

  
  glShadeModel( GL_SMOOTH );
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glClearDepth( 1.0f );
  glDepthFunc( GL_LEQUAL );
  //glDepthFunc( GL_NICEST );
  glEnable( GL_DEPTH_TEST );

  glEnable( GL_TEXTURE_2D );

  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  //glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  //glBlendFunc( GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );
  //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  //glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  //glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, _color.ptr());
  //glDisable( GL_COLOR_MATERIAL );

  //отрезания
  //glScissor(x, y, width, height);
  //glClipPlane( plane, equation )

  //glEnable( GL_ALPHA_TEST ); glAlphaFunc( GL_GREATER, 0.7f ); //вывод только тех пикселей, чья прозрачность выше 0.7f
  //glEnable( GL_ALPHA_TEST );
  //glAlphaFunc();

  //glEnable( GL_POINT_SMOOTH );  //сглаживание точек
  //glEnable( GL_LINE_SMOOTH );   //сглаживание линий
  //glEnable( GL_LINE_STIPPLE );  //вывод линий с маской
  //glEnable( GL_POLYGON_STIPPLE );//вывод полигонов с маской
  //glEnable( GL_COLOR_MATERIAL );//материал
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  //отрисовка всех сторон. м.б. стоит сделать только FRONT, нужно тестировать

  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_VERTEX_ARRAY);
  GL_CHECK_ERROR;
}//_InitViewport




/*
=============
  _InitPalette
=============
*/
void Core::_InitPalette()
{
  int pixelFormat = GetPixelFormat( this->_window.dc );
  PIXELFORMATDESCRIPTOR pfd;
  LOGPALETTE* pPal;
  int paletteSize;

  DescribePixelFormat( this->_window.dc, pixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &pfd );

  if ( pfd.dwFlags & PFD_NEED_PALETTE )
    paletteSize = 1 << pfd.cColorBits;
  else
  {
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitPalette => palette not needed" );
    return; //палитра не нужна
  }

  pPal = ( LOGPALETTE* ) malloc( sizeof( LOGPALETTE ) + paletteSize * sizeof( PALETTEENTRY ) );
  pPal->palVersion = 0x300;
  pPal->palNumEntries = paletteSize;

  /* build a simple RGB color palette */
  {
    int redMask   = ( 1 << pfd.cRedBits   ) - 1;
    int greenMask = ( 1 << pfd.cGreenBits ) - 1;
    int blueMask  = ( 1 << pfd.cBlueBits  ) - 1;
    int i;

    for( i = 0; i < paletteSize; ++i )
    {
      pPal->palPalEntry[ i ].peRed =
        ( ( ( i >> pfd.cRedShift ) & redMask ) * 255 ) / redMask;
      pPal->palPalEntry[ i ].peGreen =
        ( ( ( i >> pfd.cGreenShift ) & greenMask ) * 255 ) / greenMask;
      pPal->palPalEntry[ i ].peBlue =
        ( ( ( i >> pfd.cBlueShift ) & blueMask ) * 255 ) / blueMask;
      pPal->palPalEntry[ i ].peFlags = 0;
    }
  }

  this->_window.palette = CreatePalette( pPal );
  free( pPal );

  if( this->_window.palette )
  {
    SelectPalette( this->_window.dc, this->_window.palette, FALSE );
    RealizePalette( this->_window.dc );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitPalette finished" );
  }
  else
    this->SetState( CORE_STATE_EXIT );
    //__log.PrintInfo( Filelevel_CRITICALERROR, "Core::InitPalette => CreatePalette failed" );
}//_InitPalette



/*
=============
  _InitPixelFormat
=============
*/
void Core::_InitPixelFormat()
{
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof( PIXELFORMATDESCRIPTOR ),/* size */
    1,                              /* version */
    PFD_SUPPORT_OPENGL |
    PFD_DRAW_TO_WINDOW |
    PFD_DOUBLEBUFFER,               /* support double-buffering */
    PFD_TYPE_RGBA,                  /* color type */
    32,                             /* prefered color depth */
    0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
    0,                              /* no alpha buffer */
    0,                              /* alpha bits (ignored) */
    0,                              /* no accumulation buffer */
    0, 0, 0, 0,                     /* accum bits (ignored) */
      32,                             /* depth buffer */
    0,                              /* no stencil buffer */
    0,                              /* no auxiliary buffers */
    PFD_MAIN_PLANE,                 /* main layer */
    0,                              /* reserved */
    0, 0, 0,                        /* no layer, visible, damage masks */
  };
  bool pixelFormatOk = false;
  std::vector<BYTE> depths( 4 );
  depths[ 0 ] = 24;
  depths[ 1 ] = 16;
  depths[ 2 ] = 8;
  depths[ 3 ] = 32;
  for( BYTE num = 0; num < depths.size(); ++num )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "ChoosePixelFormat... %d", depths[ num ] );
    pfd.cDepthBits = depths[ num ];
    int pixelFormat = ChoosePixelFormat( this->_window.dc, &pfd );
    //__log.PrintInfo( Filelevel_DEBUG, "ChoosePixelFormat Done", pixelFormat );
    if ( pixelFormat )
      pixelFormatOk = true;
    else
      continue;

    //__log.PrintInfo( Filelevel_DEBUG, "SetPixelFormat..." );
    if( ::SetPixelFormat( this->_window.dc, pixelFormat, &pfd ) == TRUE )
    {
      pixelFormatOk = true;
      //__log.PrintInfo( Filelevel_INFO , "SetPixelFormat => cDepthBits[ %d ]", depths[ num ] );
      break;
    }
    else
      ;//__log.PrintInfo( Filelevel_DEBUG, "SetPixelFormat Done" );
  }
  if( !pixelFormatOk )
  {
    //__log.PrintInfo( Filelevel_CRITICALERROR, "InitPixelFormat failed" );
    exit( 1 );
    this->SetState( CORE_STATE_EXIT );
  }
}//_InitPixelFormat




/*
=============
  _Resize
=============
*/
void Core::_Resize( WORD screenWidth, WORD screenHeight )
{
  glViewport( 0, 0, screenWidth, screenHeight );
}//_Resize



/*
=============
  _UpdatePalette
=============
*/
bool Core::_UpdatePalette( const HWND window )
{
  if( window != this->_window.hwnd )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
  if ( !this->_window.glRC )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
  if ( !this->_window.palette )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  ::UnrealizeObject ( this->_window.palette );
  ::SelectPalette   ( this->_window.dc, this->_window.palette, FALSE );
  ::RealizePalette  ( this->_window.dc );
  //this->Redraw    ();

  return true;
}//_UpdatePalette




/*
=============
  Redraw
=============
*/
bool Core::Redraw()
{
  if( !this->_window.glRC )
  {
    //__log.PrintInfo( Filelevel_ERROR, "Core::Redraw => glRC is NULL" );
    return false;
  }

  this->lightRenderer->BeginScene();

  //
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_TEXTURE_2D );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  //отрисовка всех сторон. м.б. стоит сделать только FRONT, нужно тестировать
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_VERTEX_ARRAY);
  //

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //GL_STENCIL_BUFFER_BIT
  GL_CHECK_ERROR;

  //glUseProgram( this->_shaders.programm );
  //glBegin( GL_QUADS );
  //  for( CoreRenderableList::iterator renderable = __coreRenderableList->begin(); renderable != __coreRenderableList->end(); ++renderable )
  //    ( *renderable )->Render();
  //glEnd();

  if( __coreRenderableList->size() || __coreGUI->size() )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "this->_shaders.programm = x%X", this->_shaders.programm );
    glUseProgram( this->_shaders.programm );
    glEnableClientState( GL_VERTEX_ARRAY );

    //std::deque< CoreRenderableList* > renderableLists;
    //std::deque< CoreRenderableList* >::iterator renderableListsIter, renderableListsIterEnd;

    //if( __coreRenderableList->size() )
    //  renderableLists.push_back( __coreRenderableList );
    //if( __coreGUI->size() )
    //  renderableLists.push_back( __coreGUI );

    //renderableListsIterEnd = renderableLists.end();

    __textureAtlas->Bind();

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
    glEnableVertexAttribArray( 3 );

    //Основной мир
    if( __coreRenderableList->size() )
    {
      glLoadIdentity();
      //glScalef( this->_window.windowToWorld.x, this->_window.windowToWorld.y, this->_window.windowToWorld.z );
      if( this->camera && this->camera->GetIsValid() )
      {
        Object *objCamera = this->camera->GetObject< Object >();
        objCamera->Update( 0.0f );
        Mat4 matrScale, matrTranslate, matrix;
        matrScale.Identity();
        matrTranslate.Identity();
        matrScale[ 0 ][ 0 ] = 1.0f;
        matrScale[ 1 ][ 1 ] = 1.0f;
        matrTranslate[ 3 ][ 0 ] = float( long( -objCamera->GetPosition().x + this->_window.windowCenter.x ) );
        matrTranslate[ 3 ][ 1 ] = float( long( -objCamera->GetPosition().y + this->_window.windowCenter.y ) );
        matrTranslate[ 3 ][ 2 ] = -objCamera->GetPosition().z;
        matrix = matrScale * matrTranslate;
        glUniformMatrix4fv( this->_shaders.matrModelLoc, 1, false, &matrix[ 0 ][ 0 ] );

        float left = 0.0f;
        float right = this->_window.windowCenter.x * 2.0f;
        float top = 0.0f;
        float bottom = this->_window.windowCenter.y * 2.0f;
        float nearZ = 10.0f;
        float farZ = -10.0f;
	      float r_l = right - left;
	      float t_b = top - bottom;
	      float f_n = farZ - nearZ;
	      float tx = - (right + left) / (right - left);
	      float ty = - (top + bottom) / (top - bottom);
	      float tz = - (farZ + nearZ) / (farZ - nearZ);
        matrix.Identity();
        matrix[ 0 ][ 0 ] = 2.0f / r_l;
        matrix[ 3 ][ 0 ] = tx;
        matrix[ 1 ][ 1 ] = 2.0f / t_b;
        matrix[ 3 ][ 1 ] = ty;
        matrix[ 2 ][ 2 ] = 2.0f / f_n;
        matrix[ 3 ][ 2 ] = tz;
        glUniformMatrix4fv( this->_shaders.matrProjectionLoc, 1, false, &matrix[ 0 ][ 0 ] );

        //glTranslatef( -this->camera->GetPosition().x + this->_window.windowCenter.x, -this->camera->GetPosition().y + this->_window.windowCenter.y, -this->camera->GetPosition().z );
      }

      //__log.PrintInfo( Filelevel_DEBUG, "glBindVertexArray" );
      glBindVertexArray( this->_buffers.vao );
      GL_CHECK_ERROR;

      //__log.PrintInfo( Filelevel_DEBUG, "glBindBuffer:GL_ARRAY_BUFFER => %d", this->_buffers.vbo );
      //__log.PrintInfo( Filelevel_DEBUG, "vbo" );
      glBindBuffer( GL_ARRAY_BUFFER, this->_buffers.vbo );
      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glBufferData: __coreRenderableList[x%X] size[%d]", __coreRenderableList, __coreRenderableList->size() );
      glBufferData( GL_ARRAY_BUFFER, sizeof( RenderableQuad ) * __coreRenderableList->size(), ( *__coreRenderableList->begin() ).GetPointerToVertex(), GL_DYNAMIC_DRAW );
      GL_CHECK_ERROR;

      //__log.PrintInfo( Filelevel_DEBUG, "ibo" );
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_buffers.ibo );
      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glBufferData" );
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( *__coreRenderableListIndicies->begin() ) * __coreRenderableListIndicies->size(), &*__coreRenderableListIndicies->begin(), GL_DYNAMIC_DRAW );
      GL_CHECK_ERROR;

      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glVertexAttribPointer 0" );
      glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), 0 );
      GL_CHECK_ERROR;

      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glVertexAttribPointer 1" );
      glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 4 ) );
      GL_CHECK_ERROR;

      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glVertexAttribPointer 2" );
      glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 8 ) );
      GL_CHECK_ERROR;

      GL_CHECK_ERROR;
      //__log.PrintInfo( Filelevel_DEBUG, "glVertexAttribPointer 3" );
      glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 12 ) );
      GL_CHECK_ERROR;

      //glDrawElements( GL_POINTS, __coreRenderableListIndicies->size(), GL_UNSIGNED_SHORT, &( *( __coreRenderableListIndicies->begin() ) ) );
      glDrawElements( GL_POINTS, __coreRenderableListIndicies->size(), GL_UNSIGNED_SHORT, NULL );
      //glDrawArrays( GL_POINTS, 0, __coreRenderableList->size() );
      GL_CHECK_ERROR;
    }
    //End Основной мир

    __textureAtlas->Unbind();

    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glDisableVertexAttribArray( 2 );
    glDisableVertexAttribArray( 3 );

    glBindBuffer( GL_ARRAY_BUFFER, NULL );

    glDisableClientState( GL_VERTEX_ARRAY );
    glUseProgram( NULL );

    //glEnableClientState( GL_VERTEX_ARRAY );
    //glVertexPointer( 3, GL_FLOAT, sizeof( RenderableQuad ), ( *__coreRenderableList->begin() ).GetPointerToVertex() );
    //glDrawArrays( GL_POINTS, 0, __coreRenderableList->size() );
    //glDisableClientState( GL_VERTEX_ARRAY );
    //glDrawElements(  );
  }

  //далее рисуем интерфейс
  //glLoadIdentity();

  this->lightRenderer->EndScene();

  if( !__coreRenderableList->empty() || !__coreGUI->empty() ) {
    glUseProgram( this->_shaders.programm );
    glEnableClientState( GL_VERTEX_ARRAY );

    //std::deque< CoreRenderableList* > renderableLists;
    //std::deque< CoreRenderableList* >::iterator renderableListsIter, renderableListsIterEnd;

    //if( __coreRenderableList->size() )
    //  renderableLists.push_back( __coreRenderableList );
    //if( __coreGUI->size() )
    //  renderableLists.push_back( __coreGUI );

    //renderableListsIterEnd = renderableLists.end();

    __textureAtlas->Bind();

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
    glEnableVertexAttribArray( 3 );
    //editor gui
    //if( this->gui.context && this->gui.show )
    if( true )
    {
      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
      glOrtho( 0.0, this->_window.windowSize.width, this->_window.windowSize.height, 0.0, -10.0, 10.0 ); // -10.0f - ближайшая к зрителю точка
      glMatrixMode( GL_MODELVIEW );

      //debug-render
      if( this->GetCamera() && ( this->debug.renderRenderable|| this->debug.renderCollision || this->debug.renderTrigger ) ) {
        glUseProgram( 0 );
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        float alpha = Math::Sin16( ( float ) sTimer.GetTime() * 5.0f ) * 0.5f + 0.5f;
        Vec4 selectedRect;

        //renderable
        if( this->debug.renderRenderable && __coreRenderableListIndicies->size() )
        {
          CoreRenderableListIndicies::iterator iterIndex, iterIndexEnd = __coreRenderableListIndicies->end();
          glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
          glLineWidth( 2.0f );
          glBegin( GL_QUADS );
          Vec3 pos, camera = this->GetCamera()->GetPosition() - Vec3( this->GetWindowHalfSize().x, this->GetWindowHalfSize().y, 0.0f );
          Vec2 size;
          RenderableQuad *quad;
          for( iterIndex = __coreRenderableListIndicies->begin(); iterIndex != iterIndexEnd; ++iterIndex ) {
            quad = &( ( *__coreRenderableList )[ *iterIndex ] );
            pos = quad->GetPosition() - camera;
            size = quad->GetSize() * 0.5f;
            size.x *= quad->GetScale().x;
            size.y *= quad->GetScale().y;
            /*
            if( this->debug.selectedObject && this->debug.selectedObject->GetRenderable() == quad ) {
              showSelected = true;
              selectedRect.Set( pos.x - size.x, pos.y - size.y, pos.x + size.x, pos.y + size.y );
            } else */ {
              glColor4f( 0.0f, 1.0f, 0.0f, alpha );
              glVertex3f( pos.x - size.x, pos.y - size.y, 0.0f );
              glVertex3f( pos.x + size.x, pos.y - size.y, 0.0f );
              glVertex3f( pos.x + size.x, pos.y + size.y, 0.0f );
              glVertex3f( pos.x - size.x, pos.y + size.y, 0.0f );
            }
          }
          if( this->debug.selectedObjects.size() ) {
            glColor4f( 1.0f, 0.0f, 0.0f, alpha );
            for( auto &obj: this->debug.selectedObjects ) {
              if( obj->IsRenderable() ) {
                quad = &( ( *__coreRenderableList )[ static_cast< RenderableQuad* >( obj->GetRenderable() )->GetIndexInList() ] );
                pos = quad->GetPosition() - camera;
                size = quad->GetSize() * 0.5f;
                size.x *= quad->GetScale().x;
                size.y *= quad->GetScale().y;
                glVertex3f( pos.x - size.x, pos.y - size.y, 0.0f );
                glVertex3f( pos.x + size.x, pos.y - size.y, 0.0f );
                glVertex3f( pos.x + size.x, pos.y + size.y, 0.0f );
                glVertex3f( pos.x - size.x, pos.y + size.y, 0.0f );
              }
            }
          }
          glEnd();
          glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }//renderable

        Vec3 camera = this->GetCamera()->GetPosition() - Vec3( this->GetWindowHalfSize().x, this->GetWindowHalfSize().y, 0.0f );
        //collision
        if( this->debug.renderCollision && __collisionList->size() )
        {
          glLineWidth( 1.0f );
          CollisionList::iterator iter, iterEnd = __collisionList->end();
          Vec3 pos, size;
          for( iter = __collisionList->begin(); iter != iterEnd; ++iter ) {
            ( *iter )->Render( alpha, camera, false /* this->debug.selectedObject && this->debug.selectedObject->GetCollision() == *iter */ );
          }
            for( auto &obj: this->debug.selectedObjects ) {
              if( obj->IsCollision() ) {
                obj->GetCollision()->Render( alpha, camera, true );
              }
            }
        }//collisions

        //trigger
        if( this->debug.renderTrigger && __triggerList->size() )
        {
          glLineWidth( 1.0f );
          ObjectTriggerList::iterator iter, iterEnd = __triggerList->end();
          for( iter = __triggerList->begin(); iter != iterEnd; ++iter ) {
            ( *iter )->Redraw( alpha, camera, false /* this->debug.selectedObject && this->debug.selectedObject->GetTrigger() == *iter */ );
          }
          for( auto &obj: this->debug.selectedObjects ) {
            if( obj->IsTrigger() ) {
              obj->GetTrigger()->Redraw( alpha, camera, true );
            }
          }
          /*
          glBegin( GL_QUADS );
          Vec3 pos, camera = this->GetCamera()->GetPosition() - Vec3( this->GetWindowHalfSize().x, this->GetWindowHalfSize().y, 0.0f ), size;
          for( iter = __triggerList->begin(); iter != iterEnd; ++iter ) {
            pos = ( *iter )->GetPosition() - camera;
            size = ( *iter )->GetSize() * 0.5f;
            if( this->debug.selectedObject && this->debug.selectedObject->GetTrigger() == *iter ) {
              glColor4f( 1.0f, 0.0f, 0.0f, Math::Sqrt16( alpha ) );
            } else {
              glColor4f( 0.0f, 1.0f, 0.0f, alpha );
            }
            glVertex3f( pos.x - size.x, pos.y - size.y, 0.0f );
            glVertex3f( pos.x + size.x, pos.y - size.y, 0.0f );
            glVertex3f( pos.x + size.x, pos.y + size.y, 0.0f );
            glVertex3f( pos.x - size.x, pos.y + size.y, 0.0f );
          }
          glEnd();
          */
        }//trigger

        glEnable( GL_DEPTH_TEST );
      }

      if( __debugRender ) {
        glUseProgram( 0 );
        __debugRender->Render();
      }

    }

    //GUI
    //__log.PrintInfo( Filelevel_DEBUG, "test gui: __coreGUI[x%X] size[%d]", __coreGUI, __coreGUI->size() );
    if( __coreGUI->size() )
    {
      glUseProgram( this->_shaders.programm );
      __textureAtlas->Bind();
      //glLoadIdentity();
      Mat4 matrix;
      matrix.Identity();
      matrix[ 0 ][ 0 ] = 2.0f / 100.0f;
      matrix[ 1 ][ 1 ] = -2.0f / 100.0f;
      matrix[ 2 ][ 2 ] = -2.0f / 20.0f;
      matrix[ 3 ][ 0 ] = -1.0f;
      matrix[ 3 ][ 1 ] = 1.0f;
      glUniformMatrix4fv( this->_shaders.matrProjectionLoc, 1, false, &matrix[ 0 ][ 0 ] );
      matrix.Identity();
      glUniformMatrix4fv( this->_shaders.matrModelLoc, 1, false, &matrix[ 0 ][ 0 ] );

      glBindVertexArray( this->_buffers.vao );
      GL_CHECK_ERROR;

      glBindBuffer( GL_ARRAY_BUFFER, this->_buffers.vbo );
      glBufferData( GL_ARRAY_BUFFER, sizeof( RenderableQuad ) * __coreGUI->size(), ( *__coreGUI->begin() ).GetPointerToVertex(), GL_DYNAMIC_DRAW );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_buffers.iboGUI );
      GL_CHECK_ERROR;
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( *__coreGUIIndicies->begin() ) * __coreGUIIndicies->size(), &*__coreGUIIndicies->begin(), GL_DYNAMIC_DRAW );
      GL_CHECK_ERROR;
  
      //glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( 0 ) );
      //glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( 4 * sizeof( float ) ) );
      //glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( 8 * sizeof( float ) ) );
      //glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( 12 * sizeof( float ) ) );

      glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), 0 );
      GL_CHECK_ERROR;
      glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 4 ) );
      GL_CHECK_ERROR;
      glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 8 ) );
      GL_CHECK_ERROR;
      glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( RenderableQuad ), BUFFER_OFFSET( sizeof( float ) * 12 ) );
      GL_CHECK_ERROR;

      glDrawElements( GL_POINTS, __coreGUIIndicies->size(), GL_UNSIGNED_SHORT, NULL );

      //glDrawElements( GL_POINTS, __coreGUIIndicies->size(), GL_UNSIGNED_SHORT, &( *( __coreGUIIndicies->begin() ) ) );
      //glDrawArrays( GL_POINTS, 0, ( *renderableListsIter )->size() );
      if( glGetError() )
      {
        __log.PrintInfo( Filelevel_CRITICALERROR, "Core::Redraw => glDrawArrays failed" );
          this->SetState( CORE_STATE_EXIT );
      }
    }
    //__log.PrintInfo( Filelevel_DEBUG, "end test gui" );
    //End GUI

    __textureAtlas->Unbind();

    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glDisableVertexAttribArray( 2 );
    glDisableVertexAttribArray( 3 );

    glBindBuffer( GL_ARRAY_BUFFER, NULL );

    glDisableClientState( GL_VERTEX_ARRAY );
    glUseProgram( NULL );
  }

  SwapBuffers( this->_window.dc );

  return true;
}//Redraw



/*
=============
  SetCamera
=============
*/
Object* Core::SetCamera( Object* newCamera )
{
  Object *oldCamera = ( this->camera ? this->camera->GetObject< Object >() : NULL );
  DEF_DELETE( this->camera );
  this->camera = new Pointer( newCamera );
  return oldCamera;
}//SetCamera



/*
=============
  Signal
  Некая функция, используемая только движком, но доступная public (аналог виндозного WndProc)
  Получает некоторые коды и в зависимости от них что-либо делает
=============
*/
LRESULT Core::Signal( DWORD code, LPARAM lParam, WPARAM wParam, void *pointer )
{
  switch( code )
  {
  case CORE_SIGNAL_SETWINDOWISACTIVE: //установка _window.isActive
    {
      this->_window.isActive = ( lParam != 0 );
    }
  break;  //1
  case CORE_SIGNAL_RESIZE:  //Вызов Core::Resize
    {
      this->_Resize( LOWORD( lParam ), HIWORD( lParam ) );
    }
  break;
  case CORE_SIGNAL_UPDATEPALETTE:
    {
      return ( LRESULT ) this->_UpdatePalette( HWND( lParam ) );
    }
  break;
  }

  return 0;
}//Signal



/*
=============
  Update
=============
*/
bool Core::Update()
{
  //__log.PrintInfo( Filelevel_DEBUG, "Core::Update" );
  GL_CHECK_ERROR;
  sTimer.Update();
  this->keyboard.Update();
  this->mouse.Update();
  float delta = ( this->_state == CORE_STATE_PAUSED ? 0.0f : sTimer.GetDeltaF() );

  MSG msg;
  while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
  if( GetMessage( &msg, NULL, 0, 0 ) == TRUE )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
  if( this->_state == CORE_STATE_EXIT )
    return false;

  Animation::Update( delta );

  //обновляем физ-объекты. они обновляют положения объектов
  if( this->collisionManager )
    this->collisionManager->Update( delta );

  //триггеры
  if( this->triggerManager )
    this->triggerManager->Update();

  //обновляем объекты. они обновляют положения спрайтов
  this->_rootObject->Update( delta );
  //this->_rootGUIObject->Update( sTimer.GetDeltaF() );

  if( this->camera ) {
    //this->lightRenderer->SetRect( Vec2( 0.0f, 0.0f ), Vec2( 1024.0f, 768.0f ) );
    this->lightRenderer->SetRect(
      Vec2( this->camera->GetObject< Object >()->GetPosition().x - ( this->_window.windowSize.width >> 1 ), -this->camera->GetObject< Object >()->GetPosition().y - ( this->_window.windowSize.height >> 1 ) ),
      Vec2( this->camera->GetObject< Object >()->GetPosition().x + ( this->_window.windowSize.width >> 1 ), -this->camera->GetObject< Object >()->GetPosition().y + ( this->_window.windowSize.height >> 1 ) )
      );
    /*
    ( *this->lightRenderer->GetLightManager()->lightList->begin() )->position.Set(
      this->GetObject( "player" )->GetPosition().x,
      -this->GetObject( "player" )->GetPosition().y
      );
      */
  }
  this->lightRenderer->Update();

  //__log.PrintInfo( Filelevel_DEBUG, "============" );
  return true;
}//Update



/*
=============
  SetWindowTitle
=============
*/
void Core::SetWindowTitle( const std::string &title )
{
  if( this->_window.hwnd )
    SetWindowText( this->_window.hwnd, title.c_str() );
}//SetWindowTitle





/*
=============
  CreateObject
=============
*/
Object* Core::CreateObject( const std::string& name, Object *parent )
{
  if( !parent )
    parent = this->_rootObject;
  Object* newObject = new Object( name, parent ); //parent автоматом аттачит this как дочерний объект
  //__log.PrintInfo( Filelevel_DEBUG, "Core::CreateObject => name['%s'] parent[x%X] parentName['%s']", name.c_str(), parent, parent->GetNameFull().c_str() );

  return newObject;
}//CreateObject





/*
=============
  CreateFont
=============
*/
Font* Core::CreateFont( const std::string& name, Object *parent )
{
  if( !parent )
    parent = this->_rootObject;
  Font* newFont = new Font( name, parent ); //parent автоматом аттачит this как дочерний объект

  return newFont;
}//CreateFont





/*
=============
  CreateGUIObject
=============
*/
/*
Object* Core::CreateGUIObject( const std::string& name, Object *parent )
{
  return this->CreateObject( name, this->_rootGUIObject );
}//CreateGUIObject
*/




/*
=============
  GetObject
  4 варианта возвращаемого результата (основной вариант 1, остальные - побочные):
  1. null -> '/name1/name2/name3' => возвращает /root/name1/name2/name3
  2. null -> 'name' => возвращает root->name
  3. parent -> '/name1/name2/name3' => возвращает parent->name1->name2->name3
  4. parent -> 'name' => возвращает parent->name
  При этом нет возможности достучаться до "root"
  Имя не должно начинаться с "/root"
=============
*/
Object* Core::GetObject( const std::string& name, Object *parent )
{
  //__log.PrintInfo( Filelevel_DEBUG, "Core::GetObject => name[%s] parent[%s]", name.c_str(), ( parent ? parent->GetNameFull().c_str() : "<NULL>" ) );
  if( !name.length() )
  {
    __log.PrintInfo( Filelevel_WARNING, "Core::GetObject => name is NULL" );
    return NULL;
  }

  if( !parent )
    parent = this->_rootObject;

  long slashPos = name.find_first_of( "/" );
  if( slashPos == 0 ) //начинается со слеша => 1 или 3
    return this->GetObject( &name[ 1 ], parent );
  else
  {
    if( slashPos < 0 )  //нет слешей => 2 или 4
    {
      return parent->GetChild( name );
    }
    else  //слеши есть => 1 или 3
    {
      std::string nextLevel = &name[ slashPos ];
      std::string currentLevel = name.substr( 0, slashPos );
      if( !parent )
        parent = this->_rootObject;
      Object *obj = parent->GetChild( currentLevel );
      if( !obj )
        return NULL;
      return this->GetObject( nextLevel, obj );
    }
  }

  return NULL;
}//GetObject




/*
=============
  GetGUIObject
=============
*/
/*
Object* Core::GetGUIObject( const std::string& name, Object *parent )
{
  return this->GetObject( name, this->_rootGUIObject );
}//GetGUIObject
*/



/*
=============
  RemoveObject
=============
*/
bool Core::RemoveObject( const std::string& name, bool useLockToDeleteFlag )
{
  Object *obj = this->GetObject( name );
  if( !obj ) {
    return false;
  }

  //всё это делается в деструкторе Object
  //obj->ClearChilds();
  //obj->DisableRenderable();
  //obj->DisableCollision();

  if( useLockToDeleteFlag && obj->IsLockedToDelete() ) {
    return false;
  }
  delete obj; //анаттач происходит здесь
  return true;
}//RemoveObject




/*
=============
  CheckGLError
=============
*/
bool Core::CheckGLError( int line, const std::string& fileName )
{
  GLenum error = glGetError();
  if( error )
  {
    __log.PrintInfo( Filelevel_ERROR, "OpenGL error at %s:%d: x%X [%d]", fileName.c_str(), line, error, error );
    return true;
  }
  return false;
}//CheckGLError




/*
=============
  getObjectInPoint
=============
*/
Object* Core::getObjectInPoint( const Vec2& pos )
{
  return this->_rootObject->GetObjectInPoint( pos );
}//getObjectInPoint




/*
=============
  GetObjectsInRect
=============
*/
void Core::GetObjectsInRect( int type, const Vec2 &leftTop, const Vec2 &rightBottom, ObjectList& result )
{
  this->_rootObject->GetObjectsInRect( type, leftTop, rightBottom, result );
}//GetObjectsInRect



/*
=============
  GetObjectByTrigger
=============
*/
Object* Core::GetObjectByTrigger( ObjectTrigger *trigger )
{
  ObjectByTriggerList::iterator iterTrigger, iterEndTrigger = __objectByTrigger->end();
  for( iterTrigger = __objectByTrigger->begin(); iterTrigger != iterEndTrigger; ++iterTrigger )
    if( ( *iterTrigger )->object.GetIsValid() && ( *iterTrigger )->trigger == trigger )
      return ( *iterTrigger )->object.GetObject< Object >();
  return NULL;
}//GetObjectByTrigger



/*
=============
  GetObjectByCollision
=============
*/
Object* Core::GetObjectByCollision( Collision *collision )
{
  ObjectByCollisionList::iterator iterCollision, iterEndCollision = __objectByCollision->end();
  //__log.PrintInfo( Filelevel_DEBUG, "Core::GetObjectByCollision => count[%d]", __objectByCollision->size() );
  for( iterCollision = __objectByCollision->begin(); iterCollision != iterEndCollision; ++iterCollision ) {
    //__log.PrintInfo( Filelevel_DEBUG, "Core::GetObjectByCollision => match collision[x%p] with[x%p]", collision, ( *iterCollision )->collision );
    if( ( *iterCollision )->object.GetIsValid() && ( *iterCollision )->collision == collision )
      return ( *iterCollision )->object.GetObject< Object >();
  }
  return NULL;
}//GetObjectByCollision



/*
=============
  GetObjectByRenderableIndex
=============
*/
Object* Core::GetObjectByRenderableIndex( GLushort index )
{
  return this->_rootObject->GetObjectByRenderableIndex( __coreRenderableList, index );
}//GetObjectByRenderableIndex



/*
=============
  GetObjectByGui
=============
*/
/*
Object* Core::GetObjectByGui( g2Controller *controller )
{
  ObjectByGuiList::iterator iterGui, iterEndGui = __objectByGui->end();
  for( iterGui = __objectByGui->begin(); iterGui != iterEndGui; ++iterGui )
    if( ( *iterGui )->object.GetValid() && ( *iterGui )->gui == controller )
      return ( *iterGui )->object.GetObject< Object >();
  return NULL;
}//GetObjectByGui
*/




/*
=============
  ClearScene
=============
*/
void Core::ClearScene()
{
  __log.PrintInfo( Filelevel_DEBUG, "Core::ClearScene" );
  this->_rootObject->ClearChilds( false );
  this->camera = NULL;
  __log.PrintInfo( Filelevel_DEBUG, "Core::ClearScene => done" );
}//ClearScene




/*
=============
  GetCollisionInPoint
=============
*/
Object* Core::GetCollisionInPoint( const Vec2& pos, const std::string &afterObject )
{
  CollisionList::reverse_iterator iter, iterEnd = __collisionList->rend();
  bool returnFirst = ( afterObject.length() ? false : true );
  Object *object;
  for( iter = __collisionList->rbegin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->TestInPoint( pos ) ) {
      object = this->GetObjectByCollision( *iter );
      if( returnFirst ) {
        return object;
      } else if( object->GetNameFull() == afterObject ) {
        returnFirst = true;
      }
    }
  }
  return NULL;
}//GetCollisionInPoint




/*
=============
  GetTriggerInPoint
=============
*/
Object* Core::GetTriggerInPoint( const Vec2& pos, const std::string &afterObject )
{
  ObjectTriggerList::reverse_iterator iter, iterEnd = __triggerList->rend();
  bool returnFirst = ( afterObject.length() ? false : true );
  Object *object;
  for( iter = __triggerList->rbegin(); iter != iterEnd; ++iter ) {
    if( ( *iter )->TestInPoint( pos ) ) {
      object = this->GetObjectByTrigger( *iter );
      if( returnFirst ) {
        return object;
      } else if( object->GetNameFull() == afterObject ) {
        returnFirst = true;
      }
    }
  }
  return NULL;
}//GetTriggerInPoint




/*
=============
  GetRenderableInPoint
=============
*/
Object* Core::GetRenderableInPoint( const Vec2& pos, const std::string &afterObject )
{
  __log.PrintInfo( Filelevel_DEBUG, "Core::GetRenderableInPoint => pos[%3.3f; %3.3f]", pos.x, pos.y );
  CoreRenderableListIndicies::reverse_iterator iter, iterEnd = __coreRenderableListIndicies->rend();
  RenderableQuad *quad;
  Vec2 leftTop, rightBottom;
  bool returnFirst = ( afterObject.length() ? false : true );
  Object *object;

  for( iter = __coreRenderableListIndicies->rbegin(); iter != iterEnd; ++iter ) {
    quad = &( *__coreRenderableList )[ *iter ];
    __log.PrintInfo( Filelevel_DEBUG, ". index[%d] quad[x%p]", *iter, quad );
    quad->CalculateRect( leftTop, rightBottom );
    __log.PrintInfo( Filelevel_DEBUG, ". . rect[%3.3f; %3.3f]-[%3.3f; %3.3f]", leftTop.x, leftTop.y, rightBottom.x, rightBottom.y );
    if( !(
      rightBottom.x < pos.x ||
      leftTop.x       > pos.x ||
      rightBottom.y   < pos.y ||
      leftTop.y       > pos.y
      ) ) {
      __log.PrintInfo( Filelevel_DEBUG, ". . . test passed, this in" );
      object = this->GetObjectByRenderableIndex( *iter );
      if( returnFirst ) {
        return object;
      } else if( object->GetNameFull() == afterObject ) {
        returnFirst = true;
      }
    }
  }//for iter

  return NULL;
}//GetRenderableInPoint






/*
=============
  RemoveGUIObject
=============
*/
/*
void Core::RemoveGUIObject( const std::string& name )
{
  Object *obj = this->GetGUIObject( name );
  if( !obj )
  {
    __log.PrintInfo( Filelevel_WARNING, "Core::RemoveGUIObject => object not found '%s'", name.c_str() );
    return;
  }

  //всё это делается в деструкторе Object
  //obj->ClearChilds();
  //obj->DisableRenderable();
  //obj->DisableCollision();

  delete obj; //анаттач происходит здесь
}//RemoveGUIObject
*/



/*
=============
  _GluiMouseFunc
=============
*/
void Core::_GluiMouseFunc( int button, int state, int x, int y )
{
}//_GluiMouseFunc



/*
=============
  _GluiKeyboardFunc
=============
*/
void Core::_GluiKeyboardFunc( unsigned char key, int x, int y )
{
}//_GluiKeyboardFunc



/*
=============
  _GluiHoverFunc
=============
*/
void Core::_GluiHoverFunc( int x, int y )
{
}//_GluiHoverFunc



LRESULT APIENTRY Core::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  Core *core = ( Core* ) GetWindowLong( hWnd, GWL_USERDATA );
  switch (message)
  {
    case WM_CLOSE:
      if( core )
      {
        core->SetState( CORE_STATE_EXIT );
        return 0;
      }
    break;

    case WM_DESTROY:
      PostQuitMessage( 0 );
    return 0;

    case WM_SIZE:
      if( core )
        core->Signal( CORE_SIGNAL_RESIZE, lParam );
    return 0;

    case WM_PALETTECHANGED:
      if( core && ( HWND ) wParam != hWnd )
        core->Signal( CORE_SIGNAL_UPDATEPALETTE, ( LPARAM ) hWnd );
    break;

    case WM_QUERYNEWPALETTE:
      if( core && core->Signal( CORE_SIGNAL_UPDATEPALETTE, ( LPARAM ) hWnd ) )
        return TRUE;
    break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR: {
      bool altPressed = ( ( lParam&( 1<<29 ) ) == ( 1<<29) );
      if( wParam == VK_F4 && altPressed ) { //Alt+F4
        core->SetState( CORE_STATE_EXIT );
      }
      if( message == WM_SYSKEYDOWN ) {
        if( !( lParam & 0x40000000 ) ) {
          core->keyboard.DoPress( wParam );
        }
      } else {
        core->keyboard.DoRelease( wParam );
      }
      return 0;
      break;
    }

    case WM_ACTIVATE:
      core->Signal( 1, !HIWORD( wParam ) );
      return 0;
    break;

    case WM_SYSCOMMAND: // Перехватываем системную команду
    {
      switch( wParam )   // Останавливаем системный вызов
      {
        case SC_SCREENSAVE:   // Пытается ли запустится скринсейвер?
        case SC_MONITORPOWER: // Пытается ли монитор перейти в режим сбережения энергии?
        return 0;
      }
      break;
    }

    case WM_KEYDOWN:
    {
      if( !( lParam & 0x40000000 ) ) {  //prevent auto-repeating
        __log.PrintInfo( Filelevel_DEBUG, "WM_KEYDOWN: %d[x%X] => %d mods[%d]", wParam, wParam, Keyboard::KeyCodeToGlut( wParam ), KeyboardGetModifiers() );
        /*
        if( core->gui.show ) {
          Glui2::__KeyboardFunc( Keyboard::KeyCodeToGlut( wParam ), core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
        }
        if( !core->gui.show || !Glui2::__KeyboardInteracted ) {
        */
          core->keyboard.DoPress( wParam );
      }
      //}
      return 0;
    }

    case WM_KEYUP:
    {
      core->keyboard.DoRelease( wParam );
      return 0;
    }

    case WM_MOUSEMOVE:
    {
      /*
      if( core->gui.show ) {
        Glui2::__HoverFunc( core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      */
      core->mouse.MoveCursor( Vec2( float( LOWORD( lParam ) ), float( HIWORD( lParam ) ) ) );
      return 0;
    }

    case WM_LBUTTONDOWN:
      /*
      if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_LEFT_BUTTON, GLUT_DOWN, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoPress( VK_LBUTTON );
      //}
      return 0;
    break;

    case WM_LBUTTONUP:
      /*
      if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_LEFT_BUTTON, GLUT_UP, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoRelease( VK_LBUTTON );
      //}
      return 0;
    break;

    case WM_RBUTTONDOWN:
      /*if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_RIGHT_BUTTON, GLUT_DOWN, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoPress( VK_RBUTTON );
      //}
      return 0;
    break;

    case WM_RBUTTONUP:
      /*
      if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_RIGHT_BUTTON, GLUT_UP, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoRelease( VK_RBUTTON );
      //}
      return 0;
    break;

    case WM_MBUTTONDOWN:
      /*
      if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_MIDDLE_BUTTON, GLUT_DOWN, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoPress( VK_MBUTTON );
      //}
      return 0;
    break;

    case WM_MBUTTONUP:
      /*
      if( core->gui.show ) {
        Glui2::__MouseFunc( GLUT_MIDDLE_BUTTON, GLUT_UP, core->mouse.GetCursorPosition().x, core->mouse.GetCursorPosition().y );
      }
      if( !core->gui.show || !Glui2::__MouseInteracted ) {
      */
        core->mouse.DoRelease( VK_MBUTTON );
      //}
      return 0;
    break;

    case WM_SETCURSOR:
      //if( core )
      //  core->mouse.ResetCursor();
      return 0;
    break;

/*    case WM_CHAR:
      switch ((int)wParam)
      {
        case VK_ESCAPE:
          if(core)
            core->Destroy();
          return 0;
        default:
          break;
      }
    break;*/
    default:
    break;
  }
  return DefWindowProc( hWnd, message, wParam, lParam );
}//WndProc
