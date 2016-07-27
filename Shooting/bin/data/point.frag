#version 120


varying float opacity;
varying float luminance;

void main() {
	gl_FragColor = vec4(luminance, luminance, luminance, opacity);
}