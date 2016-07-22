#version 120

uniform int	isPreview;

varying float luminance;

void main() {

	vec4 color = vec4(vec3(luminance), 1.0);

	if (isPreview == 1) {
		if (luminance > 0.99) {
			color.rgb = vec3(1.0, 0.0, 0.0);
		} else if (luminance < 0.01) {
			color.rgb = vec3(0.0, 0.0, 1.0);
		}
	}

	gl_FragColor = color;
}