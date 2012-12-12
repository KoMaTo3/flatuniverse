#pragma once
#include "../gl/glext.h"
#include "../gl/wglext.h"

/*
  Импорт функций-расширений OpenGL из core.cpp
*/

/* Типы */
/*#ifndef GL_ARB_vertex_buffer_object

typedef ptrdiff_t GLsizeiptrARB;
typedef ptrdiff_t GLintptrARB;

#endif*/

/* GL_ARB_vertex_buffer_object */
extern PFNGLBINDBUFFERARBPROC           glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB;
extern PFNGLGENBUFFERSPROC              glGenBuffersARB;
extern PFNGLISBUFFERARBPROC             glIsBufferARB;
extern PFNGLBUFFERDATAARBPROC           glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC     glGetBufferSubDataARB;
extern PFNGLMAPBUFFERARBPROC            glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB;

/* GL_ARB_multitexture */
extern PFNGLACTIVETEXTUREARBPROC        glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB;
extern PFNGLMULTITEXCOORD1DARBPROC      glMultiTexCoord1dARB;
extern PFNGLMULTITEXCOORD1DVARBPROC     glMultiTexCoord1dvARB;
extern PFNGLMULTITEXCOORD1FARBPROC      glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1FVARBPROC     glMultiTexCoord1fvARB;
extern PFNGLMULTITEXCOORD1IARBPROC      glMultiTexCoord1iARB;
extern PFNGLMULTITEXCOORD1IVARBPROC     glMultiTexCoord1ivARB;
extern PFNGLMULTITEXCOORD1SARBPROC      glMultiTexCoord1sARB;
extern PFNGLMULTITEXCOORD1SVARBPROC     glMultiTexCoord1svARB;
extern PFNGLMULTITEXCOORD2DARBPROC      glMultiTexCoord2dARB;
extern PFNGLMULTITEXCOORD2DVARBPROC     glMultiTexCoord2dvARB;
extern PFNGLMULTITEXCOORD2FARBPROC      glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC     glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD2IARBPROC      glMultiTexCoord2iARB;
extern PFNGLMULTITEXCOORD2IVARBPROC     glMultiTexCoord2ivARB;
extern PFNGLMULTITEXCOORD2SARBPROC      glMultiTexCoord2sARB;
extern PFNGLMULTITEXCOORD2SVARBPROC     glMultiTexCoord2svARB;
extern PFNGLMULTITEXCOORD3DARBPROC      glMultiTexCoord3dARB;
extern PFNGLMULTITEXCOORD3DVARBPROC     glMultiTexCoord3dvARB;
extern PFNGLMULTITEXCOORD3FARBPROC      glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD3FVARBPROC     glMultiTexCoord3fvARB;
extern PFNGLMULTITEXCOORD3IARBPROC      glMultiTexCoord3iARB;
extern PFNGLMULTITEXCOORD3IVARBPROC     glMultiTexCoord3ivARB;
extern PFNGLMULTITEXCOORD3SARBPROC      glMultiTexCoord3sARB;
extern PFNGLMULTITEXCOORD3SVARBPROC     glMultiTexCoord3svARB;
extern PFNGLMULTITEXCOORD4DARBPROC      glMultiTexCoord4dARB;
extern PFNGLMULTITEXCOORD4DVARBPROC     glMultiTexCoord4dvARB;
extern PFNGLMULTITEXCOORD4FARBPROC      glMultiTexCoord4fARB;
extern PFNGLMULTITEXCOORD4FVARBPROC     glMultiTexCoord4fvARB;
extern PFNGLMULTITEXCOORD4IARBPROC      glMultiTexCoord4iARB;
extern PFNGLMULTITEXCOORD4IVARBPROC     glMultiTexCoord4ivARB;
extern PFNGLMULTITEXCOORD4SARBPROC      glMultiTexCoord4sARB;
extern PFNGLMULTITEXCOORD4SVARBPROC     glMultiTexCoord4svARB;


extern PFNWGLSWAPINTERVALEXTPROC        wglSwapIntervalEXT;

/* WGL_EXT_extensions_string */
//extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;


/*
extern void     (*glGenBuffersARB)          ( GLsizei n, GLuint *buffers );
extern void     (*glBindBufferARB)          ( GLenum target, GLuint buffer );
extern void     (*glBufferDataARB)          ( GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage );
extern void     (*glBufferSubDataARB)       ( GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data );
extern GLvoid*  (*glMapBufferARB)           ( GLenum target, GLenum access );
extern void     (*glGetBufferParameterivARB)( GLenum target, GLenum pname, GLint *params );
extern GLboolean(*glUnmapBufferARB)         ( GLenum target );
extern void     (*glDeleteBuffersARB)       ( GLsizei n, const GLuint *buffers );
extern GLboolean(*glIsBufferARB)            ( GLuint buffer );
*/
