#version 450

layout (location = 0) in vec3 position;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	gl_Position = view * vec4(position, 1.0);
}