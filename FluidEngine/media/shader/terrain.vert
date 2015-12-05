#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in float occlusion;

out vec3 Normal;
out vec2 Texcoord;
out vec3 FragPos;
out vec4 FragPosLight;
out float OccFactor;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (location = 10) uniform mat4 matModel;
layout (location = 11) uniform mat4 matLight;

void main()
{
	Normal = normals;
	Texcoord = texcoord;
	FragPos = (matModel * vec4(position, 1.0)).xyz;
	OccFactor = occlusion;
	FragPosLight = matLight * matModel * vec4(position, 1.0);
	
	gl_Position = projection * view * matModel * vec4(position, 1.0);
}