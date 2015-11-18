#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texcoord;

out vec3 Normal;
out vec2 Texcoord;
out vec3 FragPos;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (location = 10) uniform mat4 matModel;

void main()
{
	Normal = normals;
	Texcoord = texcoord;
	FragPos = (matModel * vec4(position, 1.0)).xyz;
	
	gl_Position = projection * view * matModel * vec4(position, 1.0);
}