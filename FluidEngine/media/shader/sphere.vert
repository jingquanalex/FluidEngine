#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in float radius;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	gl_Position =  vec4(position, 1.0);
}