#include "lightmgr.h"
#include "interngl.h"
#include "file.h"
#include "vertexarray.h"


Vec2 LightMgr::_ambientLightVertices[ 4 ] = {
  Vec2( -1.0f, -1.0f ),
  Vec2( 1.0f, -1.0f ),
  Vec2( -1.0f, 1.0f ),
  Vec2( 1.0f, 1.0f )
};


LightMgr::LightMgr()
:fboLight( NULL ), lightAmbient( Vec4One ), shader( NULL ), vao( new VertexArray() )
{
}


LightMgr::~LightMgr() {
  if( this->fboLight ) {
    delete this->fboLight;
    this->fboLight = NULL;
  }
  if( this->shader ) {
    delete this->shader;
    this->shader = NULL;
  }
  delete this->vao;
}


void LightMgr::Init( const Vec2& setTextureSize, ShaderProgram *setShader, LightsListPtr setLightList ) {
  if( this->fboLight ) {
    return;
  }
  this->lightList = setLightList;

  this->textureSize.width = int( setTextureSize.x );
  this->textureSize.height = int( setTextureSize.y );
  this->oneByWindowSize.Set( 1.0f / setTextureSize.x, 1.0f / setTextureSize.y );
  this->window.leftTop = Vec2Null;
  this->window.rightBottom = Vec2One;
  this->fboLight = new Fbo( int( setTextureSize.x ), int( setTextureSize.y ) );
  this->fboLight->AddTexture( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
  this->fboLight->AddTexture( GL_R16F, GL_RED, GL_UNSIGNED_BYTE );
  this->shader = setShader;

  this->_shaderUniforms.textureColor = this->shader->GetUniformLocation( "texture0" );
  this->_shaderUniforms.textureBrightness = this->shader->GetUniformLocation( "texture1" );
}//Init


void LightMgr::SetRect( const Vec2& setLeftTop, const Vec2& setRightBottom ) {
  this->window.leftTop = setLeftTop;
  this->window.rightBottom = setRightBottom;
  LightMgr::_ambientLightVertices[ 0 ].Set( this->window.leftTop.x, this->window.leftTop.y );
  LightMgr::_ambientLightVertices[ 1 ].Set( this->window.rightBottom.x, this->window.leftTop.y );
  LightMgr::_ambientLightVertices[ 2 ].Set( this->window.leftTop.x, this->window.rightBottom.y );
  LightMgr::_ambientLightVertices[ 3 ].Set( this->window.rightBottom.x, this->window.rightBottom.y );
}//SetRect


void LightMgr::Update() {
  for( auto &block: this->lightBlocks ) {
    block->Update();
  }

  for( auto &light: *this->lightList ) {
    if( light->CheckInRect( this->window.leftTop, this->window.rightBottom ) ) {
      light->Update( this->lightBlocks );
    }
  }
}//Update


void LightMgr::Render() {
  this->shader->UseProgram();

  glDisable( GL_DEPTH_TEST );

  this->fboLight->Bind();
  //glDrawBuffer( GL_NONE );
  //this->fboLight->Draw();

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glEnable( GL_BLEND );
  glBlendFunc( GL_ONE, GL_ZERO );

  glEnable( GL_TEXTURE_2D );
  glActiveTexture( GL_TEXTURE0 + 0 );
  this->BindColorTexture();
  glUniform1i( this->_shaderUniforms.textureColor, 0 );
  glActiveTexture( GL_TEXTURE0 + 1 );
  this->BindBrightnessTexture();
  glUniform1i( this->_shaderUniforms.textureBrightness, 1 );

  //ambients
  glVertexAttrib4f( 1, this->lightAmbient.x, this->lightAmbient.y, this->lightAmbient.z, this->lightAmbient.w );
  glVertexAttrib1f( 2, this->lightAmbient.w );
  glVertexAttrib2f( 3, this->oneByWindowSize.x, this->oneByWindowSize.y );
  glVertexAttrib1f( 5, this->lightAmbient.w );
  glVertexAttrib4f( 4, float( this->window.rightBottom.x + this->window.leftTop.x ) * 0.5f, float( this->window.rightBottom.y + this->window.leftTop.y ) * 0.5f, this->window.rightBottom.x - this->window.leftTop.x, this->window.rightBottom.y - this->window.leftTop.y );

  this->vao->SetData( LightMgr::_ambientLightVertices, 4 * sizeof( Vec2 ) );
  this->vao->Bind();
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
  glDisableVertexAttribArray( 0 );

  //point lights
  for( auto &light: *this->lightList ) {
    if( light->CheckInRectPassed() ) {
      glDrawBuffer( GL_NONE );
      this->fboLight->Draw();

      glEnableVertexAttribArray( 0 );
      glEnableVertexAttribArray( 1 );
      glEnableVertexAttribArray( 5 );
      glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->vertices[ 0 ].position );
      glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->vertices[ 0 ].color );
      glVertexAttribPointer( 5, 1, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->vertices[ 0 ].alpha );
      glDrawArrays( GL_TRIANGLE_FAN, 0, light->vertices.size() );
      glDisableVertexAttribArray( 0 );
      glDisableVertexAttribArray( 1 );
      glDisableVertexAttribArray( 5 );

      if( !light->verticesSoftEdges.empty() ) {
        glDrawBuffer( GL_NONE );
        this->fboLight->Draw();
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 5 );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->verticesSoftEdges[ 0 ].position );
        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->verticesSoftEdges[ 0 ].color );
        glVertexAttribPointer( 5, 1, GL_FLOAT, GL_FALSE, sizeof( LightMap::LightVertex ), &light->verticesSoftEdges[ 0 ].alpha );
        glDrawArrays( GL_TRIANGLES, 0, light->verticesSoftEdges.size() );
        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        glDisableVertexAttribArray( 5 );
        /*
        glBegin( GL_POLYGON );
          for( auto &vertex: light->verticesSoftEdges ) {
            //glVertexAttrib4f( 1, light->color.x, light->color.y, light->color.z, vertex.alpha );
            glVertexAttrib4f( 1, light->color.x, light->color.y, light->color.z, vertex.color.w );
            glVertex3f( vertex.position.x, vertex.position.y, 0.0f );
          }
        glEnd();
        */
      }
    }
  }

  this->fboLight->Unbind();
  //clear states
  this->vao->UnBind();
  glDrawBuffer( GL_BACK );
  glUseProgram( 0 );
  glActiveTexture( GL_TEXTURE0 + 1 );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glActiveTexture( GL_TEXTURE0 + 0 );
  glBindTexture( GL_TEXTURE_2D, 0 );
}//Render


void LightMgr::BindColorTexture() {
  if( this->fboLight ) {
    this->fboLight->BindTexture( 0 );
  }
}//BindColorTexture


void LightMgr::BindBrightnessTexture() {
  if( this->fboLight ) {
    this->fboLight->BindTexture( 1 );
  }
}//BindBrightnessTexture
