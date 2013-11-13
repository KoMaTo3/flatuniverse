#version 330

layout(location = 0) out vec4 fragData0;
//layout(location = 1) out vec4 fragData1;

//uniform sampler2D texture0;
//uniform sampler2D texture1;

smooth in vec4 vColor;
smooth in float brightness;
in vec2 invertWindowSize;

void main()
{
  //vec2 texCoords = vec2( gl_FragCoord[ 0 ] * invertWindowSize.x, gl_FragCoord[ 1 ] * invertWindowSize.y );
  //vec4 oldColor = texture2D( texture0, texCoords );
  //fragData0 = oldColor + vColor * brightness * vColor.a;
  fragData0 = vColor * vColor.a * brightness;

  //float br = texture2D( texture1, texCoords ).r + brightness * vColor.a;
  //fragData1.r = br;
}
