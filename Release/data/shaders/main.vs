#version 150
#extension GL_ARB_explicit_attrib_location : enable

uniform mat4 matProj, matModel;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 size_local2;
layout(location = 3) in vec4 texcoordLocal2;

out VertexData
{
  vec4 pos;
  vec2 size;
  vec2 scale;
  float alpha;
  vec4 colorV;
  vec4 texcoord;
  mat4 matrixModel;
  mat4 matrixProjection;
} outData;

void main( void )
{
  outData.pos = vec4( position.xyz, 1 );
  gl_Position = vec4(0,0,0,1);
  outData.size = size_local2.xy;
  outData.scale = size_local2.zw;
  outData.alpha = position.w;
  outData.colorV = color;
  outData.texcoord = texcoordLocal2;
  outData.matrixModel = matModel;
  outData.matrixProjection = matProj;
}
