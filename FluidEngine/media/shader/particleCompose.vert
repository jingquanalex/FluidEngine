#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 Texcoord;
out vec3 Fragpos;

void main()
{
    gl_Position = vec4(position.xy, 0.0, 1.0);
	Texcoord = texcoord;
	Fragpos = vec3(position.xy, 0.0);
}