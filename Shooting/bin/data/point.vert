#version 120

uniform sampler2DRect depth;
uniform sampler2DRect irTex;
uniform float near;
uniform float far;
uniform vec2 focus;
uniform float planeMaskThreshold;

varying float opacity;
varying float velocity;

void main() {
	
	vec2 uv = gl_Vertex.xy + vec2(256.0, 212.0);
	uv.x = 512.0 - uv.x;
	uv.y = 424.0 - uv.y;
	vec4 color = texture2DRect(depth, uv);

	float d = (1.0 - color.r) * (far - near) + near;
	opacity = 1.0 - color.g;

	if (texture2DRect(irTex, uv).r < planeMaskThreshold) {
		opacity = 0.0;
	}

	velocity = color.b / 100.0;

	vec4 position = vec4(
			gl_Vertex.x * d / focus.x,
			gl_Vertex.y * d / focus.y,
			-d,
			1.0
		);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}