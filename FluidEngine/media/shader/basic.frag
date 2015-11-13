#version 450

in vec3 Normal;
in vec2 Texcoord;
in vec3 FragPos;

out vec4 outColor;

layout (std140, binding = 1) uniform Lighting
{
	vec3 camPos;
    vec3 lightPos;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

uniform sampler2D diffuse1;
uniform sampler2D normal1;
uniform sampler2D specular1;

layout (location = 20) uniform vec3 matColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	outColor = vec4(matColor * diffuseColor, 1.0);
}