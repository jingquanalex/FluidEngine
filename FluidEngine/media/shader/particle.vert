#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec4 gsColor;
out vec3 gsEyePos;

void main()
{
	gl_Position = view * vec4(position, 1.0);
	gsEyePos = gl_Position.xyz;
	gsColor = color;
}