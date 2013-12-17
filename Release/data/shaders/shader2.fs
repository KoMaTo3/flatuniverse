#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform vec2 oneByWindowCoeff;

layout(location = 0) out vec4 fragData0;

in vec2 texCoords;

void main()
{
  vec4 color0 = texture( texture0, texCoords );
  vec4 color1 = texture( texture1, texCoords );
  vec4 coeff = vec4( 0.05, 0.05, 0.05, 0.0 );
  vec2 offset = oneByWindowCoeff.xy * 1.5f;
  fragData0 = ( color0 + coeff ) * (
    color1
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y - offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x, texCoords.y - offset.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y - offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y ) )
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y + offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x, texCoords.y + offset.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y + offset.y ) ) * 0.7
  ) / ( 5 + 4 * 0.7 );

  //lightness
  //float lightness = 0.8;
  //fragData0 = fragData0 * ( 1.0 - lightness ) + vec4( 1.0, 1.0, 1.0, 1.0 ) * lightness;
}
