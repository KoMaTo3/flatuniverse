#version 330

layout(location = 0)in vec4 vert;
layout(location = 1)in vec2 tex;

out vec2 texCoords;

void main( void )
{
  gl_Position = vert;
  texCoords = vec2( tex.x, 1 - tex.y );
}
