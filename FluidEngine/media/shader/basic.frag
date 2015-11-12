#version 450

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D diffuse1;
uniform sampler2D normal1;
uniform sampler2D specular1;

void main()
{
	outColor = vec4(texture(normal1, Texcoord).rgb, 1.0);
}