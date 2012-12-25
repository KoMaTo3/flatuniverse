#include "config.h"
#include "filemanager.h"
#include "file.h"

#include "pcre/pcre.h"



ConfigFile::ConfigFile()
{
}//constructor


ConfigFile::~ConfigFile()
{
  /*
  ConfigFileValuesList::iterator iter, iterEnd = this->values.end();
  for( iter = this->values.begin(); iter != iterEnd; ++iter )
    delete ( *iter ).second;
  this->values.clear();
  */
}//destructor




/*
=============
  LoadFromFile
=============
*/
bool ConfigFile::LoadFromFile( const std::string &fileName )
{
  memory data, res;
  if( !__fileManager->GetFile( fileName, data ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "ConfigFile::LoadFromFile => file '%s' not found", fileName.c_str() );
    return false;
  }

  const char *error;
  int errorOffset;
  static pcre *re = pcre_compile(
    //"(([+\\-]?[0-9]+(\\.[0-9]+)?)|([a-z_][a-z0-9_]*)|(\\\".*?\\\")|(\\'.*?\\')|(\\/\\/.*?\\n)|(\\/\\*.*?\\*\\/)|([!@#$%^&*\\(\\)\\-=+\\[\\]\\{\\};:.,<>?\\\\\\/])|(\\n))",
    "()",
    PCRE_CASELESS,// | PCRE_MULTILINE ,
    &error,
    &errorOffset,
    NULL
  );
  int resMaxCount = data.getLength();
  res.alloc( resMaxCount * sizeof( int ) );
  int *reRes = ( int* ) res.getData();
  int result = pcre_exec( re, NULL, ( char* ) data.getData(), data.getLength(), 0, 0, reRes, resMaxCount );
  __log.PrintInfo( Filelevel_DEBUG, "ConfigFile::LoadFromFile => result %d", result );

  return true;
}//LoadFromFile
