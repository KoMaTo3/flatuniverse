#version 330

layout(location = 0)in vec4 vert;
layout(location = 1) in vec4 color;
layout(location = 3) in vec4 oneByWindowCoeff;
layout(location = 4) in vec4 cameraInfo;
layout(location = 5) in float alpha;


const vec4 constOffset = vec4( -1.0, -1.0, 0,0 );
smooth out vec4 vColor;
smooth out float brightness;
out vec2 invertWindowSize;

void main( void )
{
  vColor = color;
  brightness = alpha;
  invertWindowSize.xy = oneByWindowCoeff.xy;

  vec2 cameraPos = vec2( cameraInfo.x, cameraInfo.y );
  vec2 scale = cameraInfo.zw * 0.5f;

  //vec4 scale = vec4( 2.0, 2.0, 1,1 ) / vec4( cameraInfo.z - cameraInfo.x, cameraInfo.w - cameraInfo.y, 1,1 );
  //gl_Position = ( vert - vec4( cameraInfo.xy, 0,0 ) ) * scale - vec4(1,1,0,0);

  gl_Position = vec4(
    ( vert.x - cameraPos.x ) / scale.x,
    ( vert.y - cameraPos.y ) / scale.y,
    1,1
  );
}

/*
  320x200 => (-1;-1)
  352x220 => (-0.72;-0.25)
*/
