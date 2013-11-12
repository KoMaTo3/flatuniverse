#pragma once

#include <string>
#include <deque>
#include <vector>
#include "klib.h"

struct VariableAttribute;
typedef std::deque< VariableAttribute* > VariableAttributesList;
typedef std::vector< VariableAttributesList > VariableAttributesListByPrioritet;



enum VariableType
{
  VariableType_UNDEFINED,
  VariableType_NUMBER,
  VariableType_STRING,
  VariableType_BOOLEAN,
  VariableType_NULL,
  VariableType_VEC2,
  VariableType_VEC3,
  VariableType_VEC4
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
  void  SetNull     ();
  void  SetVec2     ( const Vec2 &value );
  void  SetVec3     ( const Vec3 &value );
  void  SetVec4     ( const Vec4 &value );
  inline float GetNumber   ( float defaultValue = 0.0f ) const { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->f ); }
  inline bool  GetBoolean  ( bool defaultValue = false ) const { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->b ); }
  inline const std::string& GetString( const std::string &defaultValue = "" ) const { return ( this->type == VariableType_UNDEFINED ? defaultValue : this->s ); }
  inline const Vec2&        GetVec2() { return this->v2; }
  inline const Vec3&        GetVec3() { return this->v3; }
  inline const Vec4&        GetVec4() { return this->v4; }
  inline VariableType       GetType() const { return this->type; }
  inline bool  IsNull       () const { return this->type == VariableType_NULL; }

private:
  float f;
  bool  b;
  std::string s;
  VariableType type;
  Vec2  v2;
  Vec3  v3;
  Vec4  v4;
};


struct VariableAttribute {
  std::string name;
  Variable value;
};
