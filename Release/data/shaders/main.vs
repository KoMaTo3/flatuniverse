#version 150

uniform mat4 matProj, matModel;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 size_local2;
layout(location = 3) in vec4 texcoordLocal2;

in vec4 vertexPosition_local;
//in vec4 colorLocal;
//in vec4 size_local;
//in vec4 texcoordLocal;

/*
out vec4 pos;
out vec2 size;
out vec2 scale;
out float alpha;
out vec4 colorV;
out vec4 texcoord;
out mat4 matrixModel;
out mat4 matrixProjection;
*/

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
  vec3 v = vertexPosition_local.xyz;
  //gl_Position = vec4( vertexPosition_local.xyz / 50 - 1, 1 );
  outData.pos = vec4( position.xyz, 1 );
  gl_Position = vec4(0,0,0,1);//vec4( position.xyz, 1 ) /* * matrProj * matrModel */ ;
  outData.size = size_local2.xy;
  outData.scale = size_local2.zw;
  outData.alpha = position.w;
  outData.colorV = color;
  outData.texcoord = texcoordLocal2;
  outData.matrixModel = matModel;
  outData.matrixProjection = matProj;
}
