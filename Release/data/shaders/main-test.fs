#version 150

in FragData
{
  vec4 colorVertex;
  vec2 texCoord;
};
out vec4 fragColour;

uniform sampler2D textureMap;

void main ( void )
{
  vec4 v = vec4( texture2D( textureMap, texCoord ) );
  fragColour = v * colorVertex * vec4(1,1,1,0.1);
}
