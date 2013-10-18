#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

layout(location = 0) out vec4 fragData0;

in vec2 texCoordV;
in vec2 invertWindowSize;

void main()
{
  vec2 texCoords = texCoordV;
  vec2 globalTexCoords = vec2( gl_FragCoord[ 0 ] * invertWindowSize.x, 1.0 - gl_FragCoord[ 1 ] * invertWindowSize.y );
  vec4 color0 = texture( texture0, texCoords );
  vec4 color1 = texture( texture1, globalTexCoords );

  color0 += vec4( 0.1, 0.1, 0.1, 0.0 );

  vec4 br = normalize( color1 ) * texture( texture2, globalTexCoords ).r;
  fragData0 = vec4( color0.rgb * br.rgb, 1 );
}
