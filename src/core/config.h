#pragma once

#include <map>



class ConfigFile
{
private:
  struct ConfigFileValue
  {
    float f;
    std::string s;
    bool b;
  };
  typedef std::map< std::string, ConfigFileValue* > ConfigFileValuesList;
  ConfigFileValuesList values;
public:
  ConfigFile();
  virtual ~ConfigFile();

  bool  LoadFromFile( const std::string &fileName );
};
