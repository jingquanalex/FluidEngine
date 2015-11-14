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
	
	float diffuseMag = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseColor * diffuseMag;
	vec3 ambient = ambientColor;
	
	outColor = vec4(ambient + diffuse, 1.0);
}