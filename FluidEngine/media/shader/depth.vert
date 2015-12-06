#version 450

layout (location = 0) in vec3 position;

layout (location = 10) uniform mat4 matModel;
layout (location = 11) uniform mat4 matLight;

void main()
{
	gl_Position = matLight * matModel * vec4(position, 1.0);
}