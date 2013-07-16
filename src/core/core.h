#pragma once

/*
- Hoist c.end() out of for() loops when container end doesn't change
*/

#include "../gl/gl3.h"
#include "../gl/gl.h"
#include <string>
#include <deque>
#include <list>
//#include "glui2/glui2.h"


#include "object.h"
#include "font.h"
#include "renderable.h"
#include "externgl.h"
#include "timer.h"
#include "file.h"
#include "tools.h"
#include "collisionmgr.h"
#include "keyboard.h"
#include "mouse.h"
#include "filemanager.h"
#include "textureatlas.h"

//
extern CoreRenderableList *__coreRenderableList;
extern CoreRenderableList *__coreGUI;
//extern GuiList *__guiList;
//

/*
  Текущее состояние Core
*/
enum CoreStates
{
  CORE_STATE_UNKNOWN,     //неизвестное состояние
  CORE_STATE_RUN,         //промежуточное рабочее состояние
  CORE_STATE_EXIT,        //производится завершение работы
  CORE_STATE_INIT,        //происходит инициализация
  CORE_STATE_PAUSED,      //игровая пауза


  CORE_STATE_UNDEFINED    //dummy
};

//Коды сигналов для Core::Signal
#define CORE_SIGNAL_SETWINDOWISACTIVE     ( 0x00000001 )
#define CORE_SIGNAL_RESIZE                ( 0x00000002 )
#define CORE_SIGNAL_UPDATEPALETTE         ( 0x00000003 )


namespace Animation {
  class Manager;
};


class Core
{
private:
  struct
  {
    HDC                       dc;         //хендл контекста окна
    HWND                      hwnd;       //хендл окна
    HGLRC                     glRC;       //хендл OpenGL
    HPALETTE                  palette;    //палитра окна
    bool                      isActive;   //активность окна
    Vec3                      windowToWorld;  //коэффициенты пересчета оконных координат (0-100) в мировые (пиксельные)
    Vec2                      windowCenter; //центр окна, в пикселях
    Size                      windowSize;   //размеры окна
  } _window;                //всё, что относится к окну

  struct                    //время и дата билда
  {
    std::string time;
    std::string date;
  } _build;

  CoreStates  _state;       //состояние ядра
  std::deque< std::string >   _extensions;  //перечень расширений OpenGL
  //std::list< Object* >        _objects;
  Object                      *_rootObject; //корневой объект сцены
  //Object                      *_rootGUIObject;  //корневой объект GUI

  //gui
  /*
  struct {
    Glui2                   *context;
    bool                    show;
  } gui;
  */

  struct shaders
  {
    GLuint programm;
    GLuint vertex;
    GLuint fragment;
    GLuint geometry;

    GLuint locVertexPos;
    GLuint locColor;
    GLuint locSize;
    GLuint locTexCoords;
    GLuint matrModelLoc;
    GLuint matrProjectionLoc;
  } _shaders;

  struct buffers
  {
    GLuint  vbo;
    GLuint  vao;
    GLuint  ibo;
    GLuint  iboGUI;
  } _buffers;

  CollisionManager      *collisionManager;
  ObjectTriggerManager  *triggerManager;
  Pointer               *camera;  //текущая камера
  Animation::Manager    *animationMgr;

public:
  Keyboard  keyboard;
  Mouse     mouse;

  //специальный рендер объектов (подсветка в редакторе)
  struct {
    bool  renderRenderable,
          renderCollision,
          renderTrigger;
    Object *selectedObject;
  } debug;

private:
  bool  _UpdatePalette  ( const HWND window );
  void  _Resize         ( WORD screenWidth, WORD screenHeight );
  bool  _InitGraphics   ();
  void  _InitPixelFormat();
  void  _InitPalette    ();
  void  _InitViewport   ();
  void  _InitExtensions ();
  bool  _CheckShaderError( const std::string& text, GLuint shader );

public:
  Core();
  virtual ~Core();

  bool    Init                ( WORD screenWidth, WORD screenHeight, bool isFullScreen, const std::string &windowName );
  void    SetWindowTitle      ( const std::string &title );
  inline const Vec2& GetWindowHalfSize () { return this->_window.windowCenter; }
  inline const Size& GetWindowSize() { return this->_window.windowSize; }
  bool    Destroy             ();
  inline
  Object* GetRootObject       () { return this->_rootObject; }

  void    SetState            ( CoreStates newState );
  bool    Update              ();
  bool    Redraw              ();

  Object* CreateObject        ( const std::string& name, Object *parent = NULL );
  Font*   CreateFont          ( const std::string& name, Object *parent = NULL );
  Object* GetObject           ( const std::string& name, Object *parent = NULL );
  bool    RemoveObject        ( const std::string& name );
  Object* SetCamera           ( Object* newCamera );
  inline Object* GetCamera    () { return ( this->camera ? this->camera->GetObject< Object >() : NULL ); }
  Object* getObjectInPoint    ( const Vec2& pos );
  Object* GetObjectByTrigger  ( ObjectTrigger *trigger );
  Object* GetObjectByCollision( Collision *collision );
  //Object* GetObjectByGui      ( g2Controller *controller );
  Object* GetObjectByRenderableIndex  ( GLushort index );
  Object* GetCollisionInPoint ( const Vec2& pos, const std::string &afterObject );
  Object* GetTriggerInPoint   ( const Vec2& pos, const std::string &afterObject );
  Object* GetRenderableInPoint( const Vec2& pos, const std::string &afterObject );
  void    ClearScene          ();
  inline HWND GetWindowHandle() const {
    return this->_window.hwnd;
  }
  inline CoreStates GetState() {
    return this->_state;
  }
  /*
  inline
  void    SetGuiVisibility    ( bool setShow ) { this->gui.show = setShow; }
  */

  bool    LoadExtension       ( const std::string &name, void** function );
  bool    IsExtensionExist    ( const std::string extensionName );
  Vec3    PixelToTexel        ( const Vec2& pos ) { return Vec3( pos.x * this->_window.windowToWorld.x, pos.y * this->_window.windowToWorld.y, 0.0f ); }

  LRESULT Signal              ( DWORD code, LPARAM lParam = 0, WPARAM wParam = 0, void *pointer = NULL ); //Don't use this function!

  static LRESULT APIENTRY WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
  static void _GluiMouseFunc  ( int button, int state, int x, int y );
  static void _GluiKeyboardFunc ( unsigned char key, int x, int y );
  static void _GluiHoverFunc  ( int x, int y );

  bool    CheckGLError        ( int line = -1, const std::string& fileName = "" );
};


