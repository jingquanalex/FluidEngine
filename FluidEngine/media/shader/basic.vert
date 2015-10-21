#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texcoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (location = 10) uniform mat4 model;

out vec3 Color;
out vec2 Texcoord;

void main()
{
	Color = color;
	Texcoord = texcoord;
	
	gl_Position =  vec4(position, 1.0);
}