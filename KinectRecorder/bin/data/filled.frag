#version 120

uniform sampler2DRect tex;

void main()
{
	float d = texture2DRect(tex, gl_TexCoord[0].xy).b;
	gl_FragColor = vec4(vec3(d), 1.0);
}