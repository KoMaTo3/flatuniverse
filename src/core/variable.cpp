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
  SetNumber
=============
*/
void Variable::SetNumber( float value )
{
  char t[ 1024 ];
  sprintf_s( t, sizeof( t ), "%3.6f", value );
  this->f = value;
  this->b = ( this->f != 0.0f );
  this->s = t;
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




/*
=============
  SetNull
=============
*/
void Variable::SetNull()
{
  this->f     = 0.0f;
  this->b     = false;
  this->s     = "";
  this->type  = VariableType_NULL;
}//SetNull



/*
=============
  SetVec2
=============
*/
void Variable::SetVec2( const Vec2 &value )
{
  char t[ 1024 ];
  sprintf_s( t, sizeof( t ), "%3.6f %3.6f", value.x, value.y );
  this->f = value.x;
  this->b = ( this->f != 0.0f );
  this->s = t;
  this->v2 = value;
  this->v3 = Vec3( value.x, value.y, 0.0f );
  this->v4 = Vec4( value.x, value.y, 0.0f, 0.0f );
  this->type  = VariableType_VEC2;
}//SetVec2



/*
=============
  SetVec3
=============
*/
void Variable::SetVec3( const Vec3 &value )
{
  char t[ 1024 ];
  sprintf_s( t, sizeof( t ), "%3.6f %3.6f %3.6f", value.x, value.y, value.z );
  this->f = value.x;
  this->b = ( this->f != 0.0f );
  this->s = t;
  this->v2 = Vec2( value.x, value.y );
  this->v3 = value;
  this->v4 = Vec4( value.x, value.y, value.z, 0.0f );
  this->type  = VariableType_VEC3;
}//SetVec3



/*
=============
  SetVec4
=============
*/
void Variable::SetVec4( const Vec4 &value )
{
  char t[ 1024 ];
  sprintf_s( t, sizeof( t ), "%3.6f %3.6f %3.6f %3.6f", value.x, value.y, value.z, value.w );
  this->f = value.x;
  this->b = ( this->f != 0.0f );
  this->s = t;
  this->v2 = Vec2( value.x, value.y );
  this->v3 = Vec3( value.x, value.y, value.z );
  this->v4 = value;
  this->type  = VariableType_VEC4;
}//SetVec4
