#pragma once

#include <string>
#include <unordered_map>

namespace Engine {

class ShaderProgram;

class ShaderManager {
public:
  ShaderManager();
  virtual ~ShaderManager();
  ShaderProgram* AddShaderProgram( const std::string& name );
  ShaderProgram* GetShaderProgram( const std::string& name );
  bool DeleteShaderProgram( const std::string& name );
  bool ShaderExists( const std::string& name ) const;

private:
  ShaderManager( ShaderManager& );
  ShaderManager& operator=( ShaderManager& );

  typedef std::unordered_map< std::string, ShaderProgram* > ShaderProgramList;
  ShaderProgramList shaderProgramsList;
};

};
