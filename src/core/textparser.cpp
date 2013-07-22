#include "textparser.h"
#include "file.h"


#include "../pcre/pcre.h"


#ifndef min
#define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#endif


TextParser::TextParser( const char *textBuffer, int textLength )
:_buffer( textBuffer ), _length( textLength ), _currentPos( 0 )
{
  this->RemoveComments( this->_buffer );
}


TextParser::~TextParser() {
}


bool TextParser::GetNext( Result& result ) {
  if( this->_currentPos >= this->_length ) {
    result.Reset();
    return false;
  }

  const char *error;
  int errorOffset;
  static pcre *re = pcre_compile(
    "\\s*?("
    "([a-zA-Z_][a-zA-Z_0-9]*)"      //3
    "|([+\\-]?([0-9]*\\.)?[0-9]+)"  //4,5
    "|('(.*?)')"                    //6,7
    "|(\"(.*?)\")"                  //8,9
    "|(.)"                          //10
    ")\\s*",
    PCRE_MULTILINE | PCRE_DOTALL,
    &error,
    &errorOffset,
    NULL
  );
  if( !re ) {
    return false;
  }
  int reRes[ ( 20 /* кол-во пар скобок */ + 1 ) * 3 ];

  //LOGD( "next..." );
  int resultIndex = 0, pcreResult = pcre_exec( re, NULL, this->_buffer.c_str() + this->_currentPos, this->AllowedToRead(), 0, 0, reRes, sizeof( reRes ) / sizeof( int ) );
  TextParserLexem type = TPL_ERROR;
  //LOGD( "pcreResult[%d] firstByte[%d][%c]\n", pcreResult, this->_buffer[ this->_currentPos ], this->_buffer[ this->_currentPos ] );

  if( pcreResult > 0 ) {
    switch( pcreResult ) {
    case 3: //name
      resultIndex = 2;
      type = TPL_STRING;
    break;
    case 4: //number
    case 5:
      resultIndex = 3;
      type = TPL_NUMBER;
    break;
    case 6: //'string'
    case 7:
      resultIndex = 6;
      type = TPL_STRING;
    break;
    case 8: //"string"
    case 9:
      resultIndex = 8;
      type = TPL_STRING;
    break;
    case 10:  //symbols
      resultIndex = 9;
      type = TPL_SYMBOL;
    break;
    default:
      __log.PrintInfo( Filelevel_ERROR, "TextParser::GetNext => bad pcre_exec result[%d]", pcreResult );
    break;
    }//switch

    if( resultIndex > 0 ) {
      result.Set( this->_buffer.substr( reRes[ resultIndex * 2 ] + this->_currentPos, reRes[ resultIndex * 2 + 1 ] - reRes[ resultIndex * 2 ] ), type, this->_currentPos );
      //LOGD( "> %s\n", result.value.c_str() );
      this->_currentPos += reRes[ 1 ] - reRes[ 0 ];
      return true;
    }
  }

  result.Reset();
  return false;
}//GetNext



/*
=============
  RemoveComments
  Удаление комментариев вида /** / и //
=============
*/
void TextParser::RemoveComments( std::string &str )
{
  int posBlock, posSide, posSingleQuote, posDoubleQuote, pos = 0, maxPos = str.length(), posEnd;
  enum {
    ConfigFile_BLOCK_COMMENTBLOCK,  //блок комментариев /**/
    ConfigFile_BLOCK_COMMENTSIDE,   //боковой комментарий //
    ConfigFile_BLOCK_QUOTESINGLE,   //одинарные кавычки '
    ConfigFile_BLOCK_QUOTEDOUBLE,   //двойные кавычки "
    ConfigFile_BLOCK_NONE
  } blockType;

  do
  {
    posBlock        = str.find( "/*", pos );
    posSide         = str.find( "//", pos );
    posSingleQuote  = str.find( "'", pos );
    posDoubleQuote  = str.find( "\"", pos );

    pos = min( min( min( posBlock < 0 ? maxPos : posBlock, posSide < 0 ? maxPos : posSide ), posSingleQuote < 0 ? maxPos : posSingleQuote ), posDoubleQuote < 0 ? maxPos : posDoubleQuote );
    if( pos == maxPos ) {
      break;
    }

    if( pos == posBlock ) {
      blockType = ConfigFile_BLOCK_COMMENTBLOCK;
    } else if( pos == posSide ) {
      blockType = ConfigFile_BLOCK_COMMENTSIDE;
    } else if( pos == posSingleQuote ) {
      blockType = ConfigFile_BLOCK_QUOTESINGLE;
    } else {
      blockType = ConfigFile_BLOCK_QUOTEDOUBLE;
    }

    switch( blockType )
    {
    case ConfigFile_BLOCK_COMMENTSIDE:
      {
        posEnd = str.find( "\n", pos );
        posEnd = min( posEnd, maxPos );
        if( posEnd <= 0 ) {
          posEnd = maxPos - 1;
        }
        for( int q = pos; q <= posEnd; ++q ) {
          str[ q ] = ' ';
        }
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_COMMENTBLOCK:
      {
        posEnd = str.find( "*/", pos );
        posEnd = min( posEnd + 1, maxPos );
        if( posEnd <= 0 ) {
          posEnd = maxPos - 1;
        }
        for( int q = pos; q <= posEnd; ++q ) {
          str[ q ] = ' ';
        }
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_QUOTEDOUBLE:
      {
        posEnd = str.find( "\"", pos + 1 );
        if( posEnd <= 0 ) {
          posEnd = maxPos - 1;
        }
        pos = min( posEnd + 1, maxPos );
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    case ConfigFile_BLOCK_QUOTESINGLE:
      {
        posEnd = str.find( "'", pos + 1 );
        if( posEnd <= 0 ) {
          posEnd = maxPos - 1;
        }
        pos = min( posEnd + 1, maxPos );
      }
    break;  //ConfigFile_BLOCK_COMMENTBLOCK
    }//switch blockType

  } while( pos != maxPos );
}//RemoveComments


int TextParser::GetLineAtByte( int byteNum ) {
  int result = 0;
  for( int num = 0; num < byteNum && num < this->_length; ++num ) {
    if( this->_buffer[ num ] == '\n' ) {
      ++result;
    }
  }

  return result;
}//GetLineAtByte


int TextParser::GetColumnAtByte( int byteNum ) {
  int result = -1;
  if( byteNum < 0 || byteNum >= this->_length ) {
    return 0;
  }
  for( int num = byteNum; num >= 0; --num ) {
    if( this->_buffer[ num ] == '\n' ) {
      break;
    }
    ++result;
  }
  return result;
}//GetColumnAtByte


void TextParser::PopLastValue( const Result &value ) {
  if( value.lastPointer >= 0 && value.lastPointer < this->_length ) {
    this->_currentPos = value.lastPointer;
  }
}
