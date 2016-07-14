#version 120

uniform vec2 resolution;
uniform vec2 center;
uniform vec2 fov;
uniform sampler2DRect depth;

// varying vec2 uv;

varying float d;

void main()
{
	vec2 uv = gl_MultiTexCoord0.xy * resolution;

	vec4 color = texture2DRect(depth, uv);
	


	// float value = color.r;
	// float low1 = 500.0;
	// float high1 = 5000.0;
	// float low2 = 1.0;
	// float high2 = 0.0;
	
	// d = clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), 0.0, 1.0);
	// if (d == 1.0) {
	//     d = 0.0;
	// }

	// d = 1.0 - d;

	float z = clamp(color.r, 500.0, 5000.0);

	vec4 pos = vec4(
		(uv.x - center.x) * z / fov.x,
		-(uv.y - center.y) * z / fov.y,
		z,
		1.0
	);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	// gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (gl_Vertex + vec4(0.0, 0.0, d * 300.0, 0.0));
}