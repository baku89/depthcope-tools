#version 120


varying float opacity;
varying float luminance;
varying float velocity;

void main() {

	vec4 color = vec4(1.0, 1.0, 1.0, opacity);

	if (velocity > 0) {
		color.gb -= velocity;
	} else {
		color.rg += velocity;
	}

	gl_FragColor = color;
}