#include "config.h"
#include "filemanager.h"
#include "file.h"

#include "pcre/pcre.h"



ConfigFile::ConfigFile()
{
}//constructor


ConfigFile::~ConfigFile()
{
  ConfigFileValuesList::iterator iter, iterEnd = this->values.end();
  for( iter = this->values.begin(); iter != iterEnd; ++iter )
    delete ( *iter ).second;
  this->values.clear();
}//destructor




/*
=============
  LoadFromFile
=============
*/
bool ConfigFile::LoadFromFile( const std::string &fileName )
{
  memory data;
  if( !__fileManager->GetFile( fileName, data ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "ConfigFile::LoadFromFile => file '%s' not found", fileName.c_str() );
    return false;
  }

  const char *error;
  int errorOffset;
  pcre *re = pcre_compile(
    "(([+\\-]?[0-9]+(\\.[0-9]+)?)|([a-z_][a-z0-9_]*)|(\\\".*?\\\")|(\\'.*?\\')|(\\/\\/.*?\\n)|(\\/\\*.*?\\*\\/)|([!@#$%^&*\\(\\)\\-=+\\[\\]\\{\\};:.,<>?\\\\\\/])|(\\n))",
    PCRE_CASELESS /* | PCRE_MULTILINE */ ,
    &error,
    &errorOffset,
    NULL
  );

  return true;
}//LoadFromFile
