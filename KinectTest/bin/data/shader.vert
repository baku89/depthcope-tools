#version 150

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform sampler2DRect depth;

in vec4 position;

// the time value is passed into the shader by the OF app.
uniform float time;


void main()
{
	gl_Position = modelViewProjectionMatrix * position;
}