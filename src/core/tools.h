#pragma once

#include "klib.h"
#include <deque>


#define RGBA( r, g, b, a ) ( (r) | (g<<8) | (b<<16) | a<<24 )
#define ARGB( a, r, g, b ) ( (a<<24) | (r<<16) | (g<<8) | b )
#define GL_CHECK_ERROR this->CheckGLError( __LINE__, __FILE__ )

//Выполнение кода a только 1 раз
#define DO_CODE_ONCE( a ) { static bool q = true; if( q ) { q = false; ##a } }


namespace tools
{

std::deque< std::string >  Explode ( const std::string& source, const std::string& separator );

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define DEF_DELETE(a) ((a) ? delete (a), (a) = NULL : false )
#define DEF_DELETEARRAY(a) ((a) ? delete[] (a), (a) = NULL : false )
//#define DEF_RELEAS(a) ((a) ? (a)->Release(), (a) = 0 : 0)

long        FileSize            ( FILE *stream );
std::string GetCorePath         ();
std::string GetFileNameFromPath ( const std::string& path );
std::string GetPathFromFilePath ( const std::string& fullPath );
std::string GetFileExtension    ( const std::string& file );
std::string GetCurDirectory     ();
void        SetCurDirectory     ( const std::string& file );
std::string GetSubDirectory     ();
std::string ModifyPathToMine    ( const std::string& path );
std::string ModifyPathToTheir   ( const std::string& path );
Int64       GetFileWriteTime    ( const std::string& file );
void        Dump                ( void *mem, Dword size, const std::string& comment );


}//namespace tools
