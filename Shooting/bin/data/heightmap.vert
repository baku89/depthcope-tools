#version 120

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

uniform mat4 discInvMat;

varying vec4 color;


void main() {

	// get height and calc color
	vec4 dCoord = discInvMat * gl_Vertex;
	color = vec4(dCoord.x, dCoord.y, 1.0, 1.0);

	// fron ofxKinectProjectorToolkitV2
	vec3 p = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z * -1.0);
	float a = coff0 * p.x + coff1 * p.y + coff2 * p.z + coff3;
	float b = coff4 * p.x + coff5 * p.y + coff6 * p.z + coff7;
	float c = coff8 * p.x + coff9 * p.y + coff10 * p.z + 1.0;
	vec2 pCoord = vec2(a / c, b / c) * projectorSize + projectorOffset;

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pCoord, 0.0, 1.0);
}