#version 150
#extension GL_EXT_gpu_shader4 : enable

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

/*
out VertexData
{
  vec2 texCoord;
} VertexOut;
*/

//in mat4 gl_ModelViewMatrix;
//in mat4 gl_ProjectionMatrix;

/*
in vec4 pos[];
in vec2 size[];
in vec2 scale[];
in float alpha[];
in vec4 colorV[];
in vec4 texcoord[];
in mat4 matrixModel[];
in mat4 matrixProjection[];
out vec4 colorVertex;
*/

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
  /*
  colorVertex = color[ 0 ];
  vec2 half_size = size[ 0 ] * 0.5;
  float cosa = cos( alpha[ 0 ] );
  float sina = sin( alpha[ 0 ] );
  mat4 objectMatrixTranslation = mat4(
    vec4( 1.0,0.0,0.0,0.0 ),
    vec4( 0.0,1.0,0.0,0.0 ),
    vec4( 0.0,0.0,1.0,0.0 ),
    vec4( gl_in[ 0 ].gl_Position.x,gl_in[ 0 ].gl_Position.y,gl_in[ 0 ].gl_Position.z,1.0 ));
  mat4 objectMatrixRotation = mat4(
    vec4( cosa,-sina,0.0,0.0 ),
    vec4( sina,cosa,0.0,0.0 ),
    vec4( 0.0,0.0,1.0,0.0 ),
    vec4( 0.0,0.0,0.0,1.0 ));
  mat4 objectMatrixScale = mat4(
    vec4( scale[ 0 ].x,0.0,0.0,0.0 ),
    vec4( 0.0,scale[ 0 ].y,0.0,0.0 ),
    vec4( 0.0,0.0,1.0,0.0 ),
    vec4( 0.0,0.0,0.0,1.0 ));
  mat4 matrix = // gl_ProjectionMatrix * gl_ModelViewMatrix *
   objectMatrixTranslation * objectMatrixRotation * objectMatrixScale;
  vec4 vec0 = vec4( gl_in[ 0 ].gl_Position.xyz, 1.0 );

  gl_Position = ( vec4( -half_size.x, -half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  VertexOut.texCoord = texcoord[ 0 ].xy;
  EmitVertex();
  gl_Position = ( vec4( -half_size.x, half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  VertexOut.texCoord = texcoord[ 0 ].xw;
  EmitVertex();
  gl_Position = ( vec4( half_size.x, -half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  VertexOut.texCoord = texcoord[ 0 ].zy;
  EmitVertex();
  gl_Position = ( vec4( half_size.x, half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  VertexOut.texCoord = texcoord[ 0 ].zw;
  EmitVertex();
  EndPrimitive();
  */

  /*
  colorVertex = color[ 0 ];
  vec2 half_size = size[ 0 ] * 0.5;
  float cosa = cos( alpha[ 0 ] );
  float sina = sin( alpha[ 0 ] );
  
  float left = 0.0;
  float right = 100.0;
  float top = 0.0;
  float bottom = 100.0;
  float near = -10.0;
  float far = 10.0;
	float r_l = right - left;
	float t_b = top - bottom;
	float f_n = far - near;
	float tx = - (right + left) / (right - left);
	float ty = - (top + bottom) / (top - bottom);
	float tz = - (far + near) / (far - near);
  
  mat4 _ProjectionMatrix = transpose( mat4(
    vec4(2.0f / r_l,0.0,0.0,tx),
    vec4(0.0,2.0 / t_b,0.0,ty),
    vec4(0.0,0.0,2.0 / f_n,tz),
    vec4(0.0,0.0,0.0,1.0)
  ) );
  mat4 _ModelViewMatrix = mat4(
	vec4(20.0,0,0,0),
	vec4(0,20.0,0,0),
	vec4(0,0,1.0,0),
	vec4(0.0,0.0,0,1.0)
  );
  mat4 objectMatrixTranslation = mat4(
	vec4(1.0,0,0,0),
	vec4(0,1.0,0,0),
	vec4(0,0,1.0,0),
	vec4(gl_in[ 0 ].gl_Position.x,gl_in[ 0 ].gl_Position.y,gl_in[ 0 ].gl_Position.z,1.0)
  );
  mat4 objectMatrixRotation = mat4(
	vec4(cosa,-sina,0,0),
	vec4(sina,cosa,0,0),
	vec4(0,0,1.0,0),
	vec4(0,0,0,1.0)
  );
  mat4 objectMatrixScale = mat4(
	vec4(scale[ 0 ].x,0,0,0),
	vec4(0,scale[ 0 ].y,0,0),
	vec4(0,0,1.0,0),
	vec4(0,0,0,1.0)
  );
  mat4 matrix = _ProjectionMatrix * _ModelViewMatrix * objectMatrixTranslation * objectMatrixRotation * objectMatrixScale;

  gl_Position = matrix * ( vec4( -half_size.x, -half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  EmitVertex();
  gl_Position = matrix * ( vec4( -half_size.x, half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  EmitVertex();
  gl_Position = matrix * ( vec4( half_size.x, -half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  EmitVertex();
  gl_Position = matrix * ( vec4( half_size.x, half_size.y, gl_in[ 0 ].gl_Position.z, 1 ) );
  EmitVertex();
  */

  float cosa = cos( vert[ 0 ].alpha );
  float sina = sin( vert[ 0 ].alpha );
  float left = 0.0;
  float right = 320.0;
  float top = 0.0;
  float bottom = 200.0;
  float near = -10.0;
  float far = 10.0;
	float r_l = right - left;
	float t_b = top - bottom;
	float f_n = far - near;
	float tx = - (right + left) / (right - left);
	float ty = - (top + bottom) / (top - bottom);
	float tz = - (far + near) / (far - near);
  mat4 _ProjectionMatrix = transpose( mat4(
    vec4(2.0f / r_l,0.0,0.0,tx),
    vec4(0.0,2.0 / t_b,0.0,ty),
    vec4(0.0,0.0,2.0 / f_n,tz),
    vec4(0.0,0.0,0.0,1.0)
  ) );
  mat4 matrix = vert[ 0 ].matrixProjection * vert[ 0 ].matrixModel * mat4(	//<=
	  vec4(1.0,0,0,0),
	  vec4(0,1.0,0,0),
	  vec4(0,0,1.0,0),
	  vec4(vert[ 0 ].pos.x,vert[ 0 ].pos.y,vert[ 0 ].pos.z,1.0)
	) * mat4(
	vec4(cosa,-sina,0,0),
	vec4(sina,cosa,0,0),
	vec4(0,0,1.0,0),
	vec4(0,0,0,1.0)
  ) * mat4(
	vec4(vert[ 0 ].scale.x,0,0,0),
	vec4(0,vert[ 0 ].scale.y,0,0),
	vec4(0,0,1.0,0),
	vec4(0,0,0,1.0)
  );

  matrix = /* vert[ 0 ].matrixProjection * vert[ 0 ].matrixModel * */
  vert[ 0 ].matrixProjection * vert[ 0 ].matrixModel * mat4(
    vec4(1,0,0,0),
    vec4(0,1,0,0),
    vec4(0,0,1,0),
    vec4(vert[ 0 ].pos.x,vert[ 0 ].pos.y,vert[ 0 ].pos.z,1)
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
