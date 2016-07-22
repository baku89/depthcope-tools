#version 120

#define DEPTH_WIDTH		512.0
#define DEPTH_HEIGHT	424.0
uniform sampler2DRect depth;
uniform float near;
uniform float far;
uniform float scale;
uniform float captureNear;
uniform float captureFar;
uniform vec2 focus;
uniform vec2 captureSize;
uniform vec2 translation;

// varying float opacity;
varying float luminance;



void main() {
	
	vec2 uv = gl_Vertex.xy + vec2(DEPTH_WIDTH / 2, DEPTH_HEIGHT / 2);
	vec4 color = texture2DRect(depth, uv);

	float d = (1.0 - color.r) * (far - near) + near;
	luminance = 1.0 - (d - captureNear) / (captureFar - captureNear);

	vec2 position = vec2(
			gl_Vertex.x * d / focus.x,
			gl_Vertex.y * d / focus.y
	);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(position * vec2(scale) + (0.5 + translation) * captureSize, 0.0, 1.0);
}