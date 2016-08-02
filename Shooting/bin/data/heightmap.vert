#version 120

#define HEIGHTMAP_DEPTH 30.0
#define HEIGHTMAP_WIDTH 1024.0

uniform float coff0;
uniform float coff1;
uniform float coff2;
uniform float coff3;
uniform float coff4;
uniform float coff5;
uniform float coff6;
uniform float coff7;
uniform float coff8;
uniform float coff9;
uniform float coff10;

uniform vec2 projectorSize;
uniform vec2 projectorOffset;
uniform float tolerance;

uniform mat4 discInvMat;

uniform sampler2DRect heightmap;
uniform sampler2DRect overlay;

uniform int showOverlay;

// uniform sampler2DRect depth;
// uniform vec2 focus;
// uniform float near;
// uniform float far;

varying vec4 color;


// ------------------------------------------

vec3 postMult(mat4 mat, vec3 v) {
	float d = 1.0 / (mat[3][0] * v.x + mat[3][1] * v.y + mat[3][2] * v.z + mat[3][3]);
	return vec3( (mat[0][0]*v.x + mat[0][1]*v.y + mat[0][2]*v.z + mat[0][3])*d,
	             (mat[1][0]*v.x + mat[1][1]*v.y + mat[1][2]*v.z + mat[1][3])*d,
	             (mat[2][0]*v.x + mat[2][1]*v.y + mat[2][2]*v.z + mat[2][3])*d );
}


float map(float value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

vec2 map(vec2 value, vec2 inMin, vec2 inMax, vec2 outMin, vec2 outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

// ------------------------------------------


void main() {

	// ------------------------------------------
	// 0. get world coord

	// ------------------------------------------
	// 1. get height and calc color

	vec3 dc = postMult(discInvMat, gl_Vertex.xyz);

	vec2 duv = map(dc.xy, vec2(-30.0), vec2(30.0), vec2(0.0, HEIGHTMAP_WIDTH), vec2(HEIGHTMAP_WIDTH, 0.0));
	// duv.y = HEIGHTMAP_WIDTH - duv.y;

	float th = texture2DRect(heightmap, duv).r * 30.0;
	float h  = dc.z;

	if (0.0 <= h && h <= HEIGHTMAP_DEPTH) {
		if (th < h) {
			float m = map(h, th, th + tolerance, 0.0, 1.0);
			color = vec4(m, 1.0 - m, 0.0, 1.0);
		} else {
			float m = map(h, th, th - tolerance, 0.0, 1.0);
			color = vec4(0.0, 1.0 - m, m, 1.0);
		}

		if (showOverlay == 1) {
			color += texture2DRect(overlay, duv);
		}

	} else {
		color = vec4(0.0);
	}


	// ------------------------------------------
	// 2. project to projector coordinates

	// fron ofxKinectProjectorToolkitV2
	vec3 p = gl_Vertex.xyz * vec3(1.0, 1.0, -1.0);

	float a = coff0 * p.x + coff1 * p.y + coff2 * p.z + coff3;
	float b = coff4 * p.x + coff5 * p.y + coff6 * p.z + coff7;
	float c = coff8 * p.x + coff9 * p.y + coff10 * p.z + 1.0;
	vec2 pCoord = vec2(a / c, b / c) * projectorSize + projectorOffset;

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pCoord, 0.0, 1.0);
}