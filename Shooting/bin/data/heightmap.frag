#version 120

varying vec4 color;

void main()
{
  gl_FragColor = vec4(color);
}