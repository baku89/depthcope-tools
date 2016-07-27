#version 120

uniform sampler2DRect tex;

void main()
{
    vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
    float value = col.r / 65535.0;
    gl_FragColor = vec4(vec3(value), 1.0);
}