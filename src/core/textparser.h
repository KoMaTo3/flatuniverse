#ifndef __TEXTPARSER_H__
#define __TEXTPARSER_H__


#include <string>


enum TextParserLexem {
  TPL_ERROR,
  TPL_NUMBER,
  TPL_STRING,
  TPL_SYMBOL
};


class TextParser
{
public:
  struct Result {
    std::string value;
    TextParserLexem type;
    int lastPointer;

    void Set( const std::string& setValue, TextParserLexem setType, int setLastPointer = 0 ) {
      this->value = setValue;
      this->type = setType;
      this->lastPointer = setLastPointer;
    }
    void Reset() {
      this->value = "";
      this->type = TPL_ERROR;
      this->lastPointer = 0;
    }
    float GetFloat() {
      float result;
      sscanf_s( this->value.c_str(), "%f", &result );
      return result;
    }
  };

  TextParser( const char *textBuffer, int textLength );
  virtual ~TextParser();
  bool GetNext( Result& result );
  __forceinline int AllowedToRead() {
    return this->_length - this->_currentPos;
  }
  void RemoveComments( std::string &str );
  int GetLineAtByte( int byteNum );
  int GetColumnAtByte( int byteNum );
  void PopLastValue( const Result &value );

private:
  TextParser();
  TextParser( const TextParser& );
  TextParser& operator=( const TextParser& );

  std::string _buffer;
  int _length;
  int _currentPos;
};


#endif
