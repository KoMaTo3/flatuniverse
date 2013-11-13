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
  //float brightness = texture( texture2, texCoords ).r;

  //smooth
  /*
  vec2 offset = oneByWindowCoeff.xy * 2.0f;
  float brightnessOriginal = texture( texture2, texCoords ).r;
  float brightness = (
    brightnessOriginal
    + texture( texture2, vec2( texCoords.x - offset.x, texCoords.y - offset.y ) ).r
    + texture( texture2, vec2( texCoords.x, texCoords.y - offset.y ) ).r
    + texture( texture2, vec2( texCoords.x + offset.x, texCoords.y - offset.y ) ).r
    + texture( texture2, vec2( texCoords.x - offset.x, texCoords.y ) ).r
    + texture( texture2, vec2( texCoords.x + offset.x, texCoords.y ) ).r
    + texture( texture2, vec2( texCoords.x - offset.x, texCoords.y + offset.y ) ).r
    + texture( texture2, vec2( texCoords.x, texCoords.y + offset.y ) ).r
    + texture( texture2, vec2( texCoords.x + offset.x, texCoords.y + offset.y ) ).r
    ) / 9.0
    ;
  brightness = min( brightness, brightnessOriginal );
  */
  
  color0 += vec4( 0.1, 0.1, 0.1, 0.0 );

  //vec4 br = normalize( color1 ) * brightness;
  //fragData0 = vec4( color0.rgb * br.rgb, 1 );
  //fragData0 = color1;
  //fragData0 = vec4(brightness,0,0,1);
  //fragData0 = color0 * color1;
  vec2 offset = oneByWindowCoeff.xy * 1.5f;
  fragData0 = color0 * (
    texture( texture1, texCoords )
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y - offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x, texCoords.y - offset.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y - offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y ) )
    + texture( texture1, vec2( texCoords.x - offset.x, texCoords.y + offset.y ) ) * 0.7
    + texture( texture1, vec2( texCoords.x, texCoords.y + offset.y ) )
    + texture( texture1, vec2( texCoords.x + offset.x, texCoords.y + offset.y ) ) * 0.7
  ) / ( 4 + 4 * 0.7 );
}
