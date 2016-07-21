#version 120


varying float opacity;
varying float luminance;

void main() {

	gl_FragColor = vec4(vec3(luminance), opacity);
}