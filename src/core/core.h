#pragma once

/*
- Hoist c.end() out of for() loops when container end doesn't change
*/

#include "../gl/gl3.h"
#include "../gl/gl.h"
#include "object.h"
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
#include <string>
#include <deque>
#include <list>


//
extern CoreRenderableList *__coreRenderableList;
extern CoreRenderableList *__coreGUI;
//

/*
  ������� ��������� Core
*/
enum CoreStates
{
  CORE_STATE_UNKNOWN,     //����������� ���������
  CORE_STATE_RUN,         //������������� ������� ���������
  CORE_STATE_EXIT,        //������������ ���������� ������
  CORE_STATE_INIT,        //���������� �������������


  CORE_STATE_UNDEFINED    //dummy
};

//���� �������� ��� Core::Signal
#define CORE_SIGNAL_SETWINDOWISACTIVE     ( 0x00000001 )
#define CORE_SIGNAL_RESIZE                ( 0x00000002 )
#define CORE_SIGNAL_UPDATEPALETTE         ( 0x00000003 )


class Core
{
private:
  struct
  {
    HDC                       dc;         //����� ��������� ����
    HWND                      hwnd;       //����� ����
    HGLRC                     glRC;       //����� OpenGL
    HPALETTE                  palette;    //������� ����
    bool                      isActive;   //���������� ����
    Vec3                      windowToWorld;  //������������ ��������� ������� ��������� (0-100) � ������� (����������)
    Vec2                      windowCenter; //����� ����, � ��������
    Size                      windowSize;   //������� ����
  } _window;                //��, ��� ��������� � ����

  struct                    //����� � ���� �����
  {
    std::string time;
    std::string date;
  } _build;

  CoreStates  _state;       //��������� ����
  std::deque< std::string >   _extensions;  //�������� ���������� OpenGL
  //std::list< Object* >        _objects;
  Object                      *_rootObject; //�������� ������ �����
  //Object                      *_rootGUIObject;  //�������� ������ GUI

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
  Object                *camera;  //������� ������

public:
  Keyboard  keyboard;
  Mouse     mouse;

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

  bool    Init              ( WORD screenWidth, WORD screenHeight, bool isFullScreen, const std::string &windowName );
  void    SetWindowTitle    ( const std::string &title );
  inline const Vec2& GetWindowHalfSize () { return this->_window.windowCenter; }
  inline const Size& GetWindowSize() { return this->_window.windowSize; }
  bool    Destroy           ();
  inline
  Object* GetRootObject     () { return this->_rootObject; }

  void    SetState          ( CoreStates newState );
  bool    Update            ();
  bool    Redraw            ();

  Object* CreateObject      ( const std::string& name, Object *parent = NULL );
  Object* GetObject         ( const std::string& name, Object *parent = NULL );
  bool    RemoveObject      ( const std::string& name );
  Object* SetCamera         ( Object* newCamera );
  inline Object* GetCamera  () { return this->camera; }
  Object* getObjectInPoint  ( const Vec2& pos );

  bool    LoadExtension     ( const std::string &name, void** function );
  bool    IsExtensionExist  ( const std::string extensionName );
  Vec3    PixelToTexel      ( const Vec2& pos ) { return Vec3( pos.x * this->_window.windowToWorld.x, pos.y * this->_window.windowToWorld.y, 0.0f ); }

  LRESULT Signal            ( DWORD code, LPARAM lParam = 0, WPARAM wParam = 0, void *pointer = NULL ); //Don't use this function!

  bool    CheckGLError      ( int line = -1, const std::string& fileName = "" );
};


LRESULT APIENTRY CoreWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );