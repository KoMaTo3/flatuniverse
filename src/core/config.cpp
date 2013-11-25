#include "config.h"
#include "filemanager.h"
#include "file.h"
#include "tools.h"



ConfigFile::ConfigFile() {
  const char *error;
  int errorOffset;
  this->regexp = pcre_compile(
    "\\s*?((false|true|yes|no)|([a-zA-Z_][a-zA-Z_0-9]*)|([+\\-]?([0-9]*\\.)?[0-9]+)|([=])|('(.*?)')|(\"(.*?)\"))\\s*",
    PCRE_MULTILINE | PCRE_DOTALL,//PCRE_CASELESS,// | PCRE_MULTILINE ,
    &error,
    &errorOffset,
    NULL
  );
}//constructor


ConfigFile::~ConfigFile()
{
  ConfigFileValuesList::iterator iter, iterEnd = this->values.end();
  for( iter = this->values.begin(); iter != iterEnd; ++iter )
    delete ( *iter ).second;
  this->values.clear();
  pcre_free( this->regexp );
  this->regexp = NULL;
}//destructor




/*
=============
  LoadFromFile
=============
*/
bool ConfigFile::LoadFromFile( const std::string &fileName )
{
  //__log.PrintInfo( Filelevel_DEBUG, "ConfigFile::LoadFromFile( '%s' )", fileName.c_str() );

  //берём файл из файлового менеджера
  memory dataMem, res;
  std::string data;
  if( !__fileManager->GetFile( fileName, dataMem, true ) )
  {
    //__log.PrintInfo( Filelevel_WARNING, "ConfigFile::LoadFromFile => file '%s' not found", fileName.c_str() );
    return false;
  }
  data = dataMem.getData();

  //удаляем комментарии и данных
  this->RemoveComments( data );

  //инициализируем PCRE
  int reRes[ ( 20 /* кол-во пар скобок */ + 1 ) * 3 ];
  bool finded = false;
  Dword offset = 0;

  //типы лексем
  enum
  {
    ConfigFile_LEXEMTYPE_UNKNOWN,
    ConfigFile_LEXEMTYPE_WORD,    //названия переменных
    ConfigFile_LEXEMTYPE_NUMBER,  //числа
    ConfigFile_LEXEMTYPE_EQUAL,   //оператор (знак "=")
    ConfigFile_LEXEMTYPE_BOOLEAN, //булевое значение (true/false, yes/no)
    ConfigFile_LEXEMTYPE_QUOTES   //строка или текст
  } lexemType;

  //операнды. в конфиге вряд ли что-то кроме "=" может быть
  enum
  {
    ConfigFile_OPERATION_UNKNOWN,
    ConfigFile_OPERATION_ASSIGN   //=
  } operation = ConfigFile_OPERATION_UNKNOWN;

  int resultIndex;
  std::string paramName, paramValue;

  //парсим данные
  do
  {
    int result = pcre_exec( this->regexp, NULL, &data[ offset ], data.size() - offset, 0, 0, reRes, sizeof( reRes ) / sizeof( int ) );

    //по числу найденных блоков определяем, что мы нашли
    lexemType = ConfigFile_LEXEMTYPE_UNKNOWN;
    if( result > 0 )
    {
      switch( result )
      {
      case 3:
        lexemType = ConfigFile_LEXEMTYPE_BOOLEAN;
        resultIndex = 2;
      break;
      case 4:
        lexemType = ConfigFile_LEXEMTYPE_WORD;
        resultIndex = 3;
      break;
      case 5:
      case 6:
        lexemType = ConfigFile_LEXEMTYPE_NUMBER;
        resultIndex = 4;
      break;
      case 7:
        lexemType = ConfigFile_LEXEMTYPE_EQUAL;
        resultIndex = 6;
      break;
      case 8:
      case 9:
        lexemType = ConfigFile_LEXEMTYPE_QUOTES;
        resultIndex = 8;
      break;
      case 10:
      case 11:
        lexemType = ConfigFile_LEXEMTYPE_QUOTES;
        resultIndex = 10;
      break;
      default:
        __log.PrintInfo( Filelevel_WARNING, "ConfigFile::LoadFromFile => unknown match result %d", result );
        break;
      }

      //вырезаем найденную строку
      switch( lexemType )
      {
      case ConfigFile_LEXEMTYPE_WORD:
      case ConfigFile_LEXEMTYPE_NUMBER:
      case ConfigFile_LEXEMTYPE_BOOLEAN:
      case ConfigFile_LEXEMTYPE_QUOTES:
        {
          paramValue = data.substr( reRes[ resultIndex * 2 ] + offset, reRes[ resultIndex * 2 + 1 ] - reRes[ resultIndex * 2 ] );
          //__log.PrintInfo( Filelevel_DEBUG, "result[%d] paramValue['%s']", result, paramValue.c_str() );

          //в зависимости от текущего оператора делаем что-то
          switch( operation )
          {
          case ConfigFile_OPERATION_ASSIGN:
            {
              switch( lexemType )
              {
              case ConfigFile_LEXEMTYPE_NUMBER:
                this->SetNumber( paramName, paramValue );
                operation = ConfigFile_OPERATION_UNKNOWN;
                break;
              case ConfigFile_LEXEMTYPE_BOOLEAN:
                this->SetBoolean( paramName, paramValue );
                operation = ConfigFile_OPERATION_UNKNOWN;
                break;
              default:
                this->SetString( paramName, paramValue );
                operation = ConfigFile_OPERATION_UNKNOWN;
              }
            }
          break;
          default:  //оператора нет - ничего не делаем, считаем это именем переменной
            paramName = paramValue;
            paramValue = "";
          }
        }
      break;
      case ConfigFile_LEXEMTYPE_EQUAL:
        operation = ConfigFile_OPERATION_ASSIGN;
      break;
      default:
        __log.PrintInfo( Filelevel_WARNING, "Bad file format: offset %d [x%X]", offset, offset );
        paramValue = "";
      break;
      }

      finded = true;
      offset += reRes[ 1 ] - reRes[ 0 ];
      if( offset >= data.size() )
        break;
    }
    else
      __log.PrintInfo( Filelevel_WARNING, "Bad file format: no matches" );
  } while( finded );

  return true;
}//LoadFromFile



/*
=============
  RemoveComments
  Удаление комментариев вида /** / и //
=============
*/
void ConfigFile::RemoveComments( std::string &str )
{
  int pos = 0, maxPos = str.length(), posEnd;
  enum {
    ConfigFile_BLOCK_COMMENTBLOCK,  //блок комментариев /**/
    ConfigFile_BLOCK_COMMENTSIDE,   //боковой комментарий //
    ConfigFile_BLOCK_QUOTESINGLE,   //одинарные кавычки '
    ConfigFile_BLOCK_QUOTEDOUBLE,   //двойные кавычки "
    ConfigFile_BLOCK_NONE
  } blockType;

  do
  {
    int posBlock, posSide, posSingleQuote, posDoubleQuote;
    posBlock        = str.find( "/*", pos );
    posSide         = str.find( "//", pos );
    posSingleQuote  = str.find( "'", pos );
    posDoubleQuote  = str.find( "\"", pos );

    pos = min( min( min( posBlock < 0 ? maxPos : posBlock, posSide < 0 ? maxPos : posSide ), posSingleQuote < 0 ? maxPos : posSingleQuote ), posDoubleQuote < 0 ? maxPos : posDoubleQuote );
    if( pos == maxPos )
      break;

    if( pos == posBlock )
      blockType = ConfigFile_BLOCK_COMMENTBLOCK;
    else if( pos == posSide )
      blockType = ConfigFile_BLOCK_COMMENTSIDE;
    else if( pos == posSingleQuote )
      blockType = ConfigFile_BLOCK_QUOTESINGLE;
    else
      blockType = ConfigFile_BLOCK_QUOTEDOUBLE;

    switch( blockType )
    {
    case ConfigFile_BLOCK_COMMENTSIDE:
      {
        posEnd = str.find( "\n", pos );
        posEnd = min( posEnd, maxPos );
        if( posEnd <= 0 )
          posEnd = maxPos - 1;
        for( int q = pos; q <= posEnd; ++q )
          str[ q ] = ' ';
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_COMMENTBLOCK:
      {
        posEnd = str.find( "*/", pos );
        posEnd = min( posEnd + 1, maxPos );
        if( posEnd <= 0 )
          posEnd = maxPos - 1;
        for( int q = pos; q <= posEnd; ++q )
          str[ q ] = ' ';
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_QUOTEDOUBLE:
      {
        posEnd = str.find( "\"", pos + 1 );
        if( posEnd <= 0 )
          posEnd = maxPos - 1;
        pos = min( posEnd + 1, maxPos );
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_QUOTESINGLE:
      {
        posEnd = str.find( "'", pos + 1 );
        if( posEnd <= 0 )
          posEnd = maxPos - 1;
        pos = min( posEnd + 1, maxPos );
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    }//switch blockType

  } while( pos != maxPos );
}//RemoveComments



/*
=============
  _GetValue
=============
*/
ConfigFile::ConfigFileValue* ConfigFile::_GetValue( const std::string &name, bool createIfNotExist )
{
  ConfigFileValuesList::iterator iter = this->values.find( name );
  if( iter == this->values.end() )
  {
    if( createIfNotExist )
    {
      ConfigFileValue *newValue = new ConfigFileValue();
      this->values.insert( make_pair( name, newValue ) );
      iter = this->values.find( name );
      return iter->second;
    }
    return NULL;
  }
  return iter->second;
}//_GetValue




/*
=============
  SetNumber
=============
*/
void ConfigFile::SetNumber( const std::string &name, const std::string &value )
{
  ConfigFileValue *val = this->_GetValue( name, true );
  float f;
  sscanf_s( value.c_str(), "%f", &f );
  val->b    = ( f != 0.0f );
  val->f    = f;
  val->s    = value;
  val->type = ConfigFileValueType_NUMBER;
}//SetNumber




/*
=============
  SetBoolean
=============
*/
void ConfigFile::SetBoolean( const std::string &name, const std::string &value )
{
  ConfigFileValue *val = this->_GetValue( name, true );
  std::string valueNoCase = tools::StrToLowerCase( value );
  val->b    = ( valueNoCase == "true" || valueNoCase == "yes" );
  val->f    = ( val->b ? 1.0f : 0.0f );
  val->s    = valueNoCase;
  val->type = ConfigFileValueType_BOOLEAN;
}//SetBoolean




/*
=============
  SetString
=============
*/
void ConfigFile::SetString( const std::string &name, const std::string &value )
{
  ConfigFileValue *val = this->_GetValue( name, true );
  val->b    = false;
  val->f    = 0.0f;
  val->s    = value;
  val->type = ConfigFileValueType_STRING;
}//SetString



/*
=============
  Dump
=============
*/
void ConfigFile::Dump()
{
  __log.PrintInfo( Filelevel_DEBUG, "\n============== ConfigFile::Dump ==============" );
  ConfigFileValuesList::iterator iter, iterEnd = this->values.end();
  for( iter = this->values.begin(); iter != iterEnd; ++iter )
  {
    switch( iter->second->type )
    {
    case ConfigFileValueType_NUMBER:
      __log.PrintInfo( Filelevel_DEBUG, ". [number]  %s = %3.3f", iter->first.c_str(), iter->second->f );
      break;
    case ConfigFileValueType_STRING:
      __log.PrintInfo( Filelevel_DEBUG, ". [string]  %s = %s", iter->first.c_str(), iter->second->s.c_str() );
      break;
    case ConfigFileValueType_BOOLEAN:
      __log.PrintInfo( Filelevel_DEBUG, ". [boolean] %s = %s", iter->first.c_str(), iter->second->b ? "true" : "false" );
      break;
    default:
      __log.PrintInfo( Filelevel_DEBUG, ". [unknown] %s", iter->first.c_str() );
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "\n============== EndDump ==============" );
}//Dump



/*
=============
  GetNumber
=============
*/
float ConfigFile::GetNumber( const std::string &name, float defaultValue )
{
  ConfigFileValue *val = this->_GetValue( name );
  return ( val ? val->f : defaultValue );
}//GetNumber



/*
=============
  GetBoolean
=============
*/
bool ConfigFile::GetBoolean( const std::string &name, bool defaultValue )
{
  ConfigFileValue *val = this->_GetValue( name );
  return ( val ? val->b : defaultValue );
}//GetBoolean



/*
=============
  GetString
=============
*/
const std::string& ConfigFile::GetString( const std::string &name, const std::string &defaultValue )
{
  ConfigFileValue *val = this->_GetValue( name );
  return ( val ? val->s : defaultValue );
}//GetString
