#version 120

varying float opacity;

void main()
{
	

	gl_FragColor = vec4(vec3(1.0), opacity);
}