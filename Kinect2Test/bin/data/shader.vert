#version 120

// uniform mat4 modelViewProjectionMatrix;
uniform sampler2DRect depth;

// uniform float time;



void main()
{
	vec4 position = ftransform();
	gl_Position = position;
}