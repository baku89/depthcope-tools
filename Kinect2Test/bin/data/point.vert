#version 120

uniform vec2 resolution;
uniform sampler2DRect depth;
uniform float near;
uniform float far;
uniform vec2 fov;

varying float opacity;

void main()
{
	vec4 color = texture2DRect(depth, gl_Vertex.xy + vec2(256.0, 212.0));

	float d = (1.0 - color.r) * (far - near) + near;
	opacity = 1.0 - color.g;

	

	vec4 position = vec4(
			gl_Vertex.x * d / fov.x,
			-gl_Vertex.y * d / fov.y,
			d,
			1.0
		);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}