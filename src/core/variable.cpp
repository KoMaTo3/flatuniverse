#include "variable.h"
#include "tools.h"


Variable::Variable() :type( VariableType_UNDEFINED ) {
}//constructor



Variable::~Variable() {
}//destructor





/*
=============
  SetNumber
=============
*/
void Variable::SetNumber( const std::string &value )
{
  float f;
  sscanf_s( value.c_str(), "%f", &f );
  this->b     = ( f != 0.0f );
  this->f     = f;
  this->s     = value;
  this->type  = VariableType_NUMBER;
}//SetNumber




/*
=============
  SetBoolean
=============
*/
void Variable::SetBoolean( const std::string &value )
{
  std::string valueNoCase = tools::StrToLowerCase( value );
  this->b     = ( valueNoCase == "true" || valueNoCase == "yes" );
  this->f     = ( this->b ? 1.0f : 0.0f );
  this->s     = valueNoCase;
  this->type  = VariableType_BOOLEAN;
}//SetBoolean




/*
=============
  SetBoolean
=============
*/
void Variable::SetBoolean( bool value )
{
  this->b     = value;
  this->f     = ( this->b ? 1.0f : 0.0f );
  this->s     = ( value ? "true" : "false" );
  this->type  = VariableType_BOOLEAN;
}//SetBoolean




/*
=============
  SetString
=============
*/
void Variable::SetString( const std::string &value )
{
  std::string valueNoCase = tools::StrToLowerCase( value );
  sscanf_s( value.c_str(), "%f", &this->f );
  this->b     = ( valueNoCase == "true" || valueNoCase == "yes" );
  this->s     = value;
  this->type  = VariableType_STRING;
}//SetString
