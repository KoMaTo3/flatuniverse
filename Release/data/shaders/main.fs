#version 150

//in vec4 colorVertex;
in FragData
{
  vec4 colorVertex;
  vec2 texCoord;
};
out vec4 fragColour;


uniform sampler2D textureMap;
//in vec4 gl_FragCoord;
/*
in VertexData
{
  vec2 texCoord;
} VertexIn;
*/

void main ( void )
{
  //fragColour = vec4( v.r * colorVertex.r, v.g * colorVertex.g, v.b * colorVertex.b, v.a * 0.1 );
  //fragColour = colorVertex;

  vec4 v = vec4( texture2D( textureMap, texCoord ) );
  fragColour = v * colorVertex;
  //fragColour = vec4(1,0,0,0.2);
}
