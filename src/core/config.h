#pragma once

#include <map>
#include <pcre/pcre.h>

class ConfigFile;
extern ConfigFile* __config;



class ConfigFile
{
public:
  enum ConfigFileValueType
  {
    ConfigFileValueType_NUMBER,
    ConfigFileValueType_STRING,
    ConfigFileValueType_BOOLEAN
  };

private:
  struct ConfigFileValue
  {
    float f;
    std::string s;
    bool b;

    ConfigFileValueType type;
  };
  typedef std::map< std::string, ConfigFileValue* > ConfigFileValuesList;
  ConfigFileValuesList values;
public:
  ConfigFile();
  virtual ~ConfigFile();

  bool  LoadFromFile  ( const std::string &fileName );

  void  SetNumber     ( const std::string &name, const std::string &value );
  void  SetNumber     ( const std::string &name, float value );
  void  SetBoolean    ( const std::string &name, const std::string &value );
  void  SetString     ( const std::string &name, const std::string &value );
  float GetNumber     ( const std::string &name, float defaultValue = 0.0f );
  bool  GetBoolean    ( const std::string &name, bool defaultValue = false );
  const std::string& GetString( const std::string &name, const std::string &defaultValue = "" );

  void  RemoveComments( std::string &str );
  void  Dump          ();

private:
  ConfigFileValue* _GetValue( const std::string &name, bool createIfNotExist = false );
  pcre *regexp;
};
