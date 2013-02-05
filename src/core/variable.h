#pragma once

#include <string>

enum VariableType
{
  VariableType_UNDEFINED,
  VariableType_NUMBER,
  VariableType_STRING,
  VariableType_BOOLEAN
};




class Variable {
public:
  Variable();
  virtual ~Variable();
  void  SetNumber   ( const std::string &value );
  void  SetNumber   ( float value );
  void  SetBoolean  ( const std::string &value );
  void  SetBoolean  ( bool value );
  void  SetString   ( const std::string &value );
  inline float GetNumber   ( float defaultValue = 0.0f ) { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->f ); }
  inline bool  GetBoolean  ( bool defaultValue = false )  { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->b ); }
  inline const std::string& GetString( const std::string &defaultValue = "" )  { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->s ); }
  inline VariableType       GetType() { return this->type; }

private:
  float f;
  bool  b;
  std::string s;
  VariableType type;

};
