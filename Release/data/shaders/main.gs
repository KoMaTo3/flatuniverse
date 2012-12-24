#version 150
#extension GL_EXT_gpu_shader4 : enable

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VertexData
{
  vec4 pos;
  vec2 size;
  vec2 scale;
  float alpha;
  vec4 colorV;
  vec4 texcoord;
  mat4 matrixModel;
  mat4 matrixProjection;
} vert[];

out FragData
{
  vec4 colorVertex;
  vec2 texCoord;
};

void main()
{
  float cosa = cos( vert[ 0 ].alpha );
  float sina = sin( vert[ 0 ].alpha );

  mat4 matrix =
  vert[ 0 ].matrixProjection * vert[ 0 ].matrixModel * mat4(  //translate
    vec4(1,0,0,0),
    vec4(0,1,0,0),
    vec4(0,0,1,0),
    vec4(vert[ 0 ].pos.x,vert[ 0 ].pos.y,vert[ 0 ].pos.z,1)
  ) * mat4(             //rotate
	vec4(cosa,-sina,0,0),
	vec4(sina,cosa,0,0),
	vec4(0,0,1.0,0),
	vec4(0,0,0,1.0)
  ) * mat4(             //scale
    vec4(vert[ 0 ].scale.x,0,0,0),
    vec4(0,vert[ 0 ].scale.y,0,0),
    vec4(0,0,1,0),
    vec4(0,0,0,1)
  );
  vec2 half_size = vert[ 0 ].size * 0.5;

  colorVertex = vert[ 0 ].colorV;

  gl_Position = matrix * vec4( -half_size.x, -half_size.y, 0.0, 1 );
  gl_PrimitiveID = gl_PrimitiveIDIn;
  texCoord = vert[ 0 ].texcoord.xy;
  EmitVertex();
  gl_Position = matrix * vec4( -half_size.x, half_size.y, 0.0, 1 );
  gl_PrimitiveID = gl_PrimitiveIDIn;
  texCoord = vert[ 0 ].texcoord.xw;
  EmitVertex();
  gl_Position = matrix * vec4( half_size.x, -half_size.y, 0.0, 1 );
  gl_PrimitiveID = gl_PrimitiveIDIn;
  texCoord = vert[ 0 ].texcoord.zy;
  EmitVertex();
  gl_Position = matrix * vec4( half_size.x, half_size.y, 0.0, 1 );
  gl_PrimitiveID = gl_PrimitiveIDIn;
  texCoord = vert[ 0 ].texcoord.zw;
  EmitVertex();
  EndPrimitive();
}
