#version 450

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D tex;

void main()
{
    //outColor = texture(tex, Texcoord) * vec4(Color, 1.0);
	//outColor = vec4(Color, 1.0);
	outColor = texture(tex, Texcoord) * vec4(1.0);
}