#version 120

uniform sampler2DRect depth;
 
void main()
{
	vec4 color = texture2DRect(depth, vec2(0.5));
	vec2 uv = gl_TexCoord[0].xy;

	gl_FragColor = vec4(uv.x, uv.y, 1.0, 1.0);

}