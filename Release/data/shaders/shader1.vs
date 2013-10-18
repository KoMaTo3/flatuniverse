#version 330

layout(location = 0)in vec4 vert;
layout(location = 1)in vec2 texCoord;
layout(location = 2) in vec4 oneByWindowCoeff;

out vec2 texCoordV;
out vec2 invertWindowSize;

void main( void )
{
  gl_Position = vert;
  texCoordV = texCoord;
  invertWindowSize.xy = oneByWindowCoeff.xy;
}
