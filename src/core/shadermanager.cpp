#include "shadermanager.h"
#include "shaderprogram.h"
#include "tools.h"

using namespace Engine;

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
  for( auto& shader: this->shaderProgramsList ) {
    delete shader.second;
  }
}

ShaderProgram* ShaderManager::AddShaderProgram( const std::string& name ) {
  auto finded = this->shaderProgramsList.find( name );
  if( finded == this->shaderProgramsList.end() ) {
    ShaderProgram *newProgram = new ShaderProgram();
    this->shaderProgramsList.insert( std::make_pair( name, newProgram ) );
    return newProgram;
  }
  return finded->second;
}//AddShaderProgram

ShaderProgram* ShaderManager::GetShaderProgram( const std::string& name ) {
  auto finded = this->shaderProgramsList.find( name );
  return ( finded == this->shaderProgramsList.end() ? NULL : finded->second );
}//GetShaderProgram

bool ShaderManager::ShaderExists( const std::string& name ) const {
  return this->shaderProgramsList.find( name ) != this->shaderProgramsList.end();
}//ShaderExists

bool ShaderManager::DeleteShaderProgram( const std::string& name ) {
  auto finded = this->shaderProgramsList.find( name );
  if( finded == this->shaderProgramsList.end() ) {
    LOGW( "ShaderManager::DeleteShaderProgram => shader['%s'] not found", name.c_str() );
    return false;
  }
  delete finded->second;
  this->shaderProgramsList.erase( finded );

  return true;
}//DeleteShaderProgram
