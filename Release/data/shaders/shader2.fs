#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

layout(location = 0) out vec4 fragData0;

in vec2 texCoords;

void main()
{
  //vec4 color0 = texture( texture0, texCoords );
  //vec4 color1 = texture( texture1, texCoords );
  //vec4 color2 = texture( texture2, texCoords );
  //fragData0 = color1;

  vec4 color0 = texture( texture0, texCoords );
  vec4 color1 = texture( texture1, texCoords );
  float brightness = texture( texture2, texCoords ).r;

  color0 += vec4( 0.1, 0.1, 0.1, 0.0 );

  vec4 br = normalize( color1 ) * brightness;
  fragData0 = vec4( color0.rgb * br.rgb, 1 );
  //fragData0 = color1;
  //fragData0 = vec4(brightness,0,0,1);
}
