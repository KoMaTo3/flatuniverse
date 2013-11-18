#version 330

layout(location = 0) out vec4 fragData0;
//layout(location = 1) out vec4 fragData1;

smooth in vec4 vColor;
in vec2 invertWindowSize;

void main()
{
  fragData0 = vColor * vColor.a;
}
