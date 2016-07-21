#version 120

uniform sampler2DRect depth;
uniform float near;
uniform float far;
uniform vec2 focus;

varying float opacity;
varying float luminance;

void main() {
	
	vec4 color = texture2DRect(depth, gl_Vertex.xy + vec2(256.0, 212.0));

	luminance = color.r;
	float d = (1.0 - luminance) * (far - near) + near;
	opacity = 1.0 - color.g;

	vec4 position = vec4(
			gl_Vertex.x * d / focus.x,
			-gl_Vertex.y * d / focus.y,
			d,
			1.0
		);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}