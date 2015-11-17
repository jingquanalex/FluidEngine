#version 450

in vec3 Normal;
in vec2 Texcoord;
in vec3 FragPos;

out vec4 outColor;

layout (std140, binding = 1) uniform Lighting
{
	vec3 viewPos;
    vec3 lightPos;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

uniform sampler2D diffuse1;

struct Material
{
	vec3 emissive;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
	vec3 N = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	
	vec3 diffuseMap = vec3(texture(diffuse1, Texcoord));
	
	vec3 ambient = ambientColor * material.ambient * diffuseMap;
	
	float diffuseMag = max(dot(N, lightDir), 0.0);
	vec3 diffuse = diffuseMag * diffuseColor * material.diffuse * diffuseMap;
	
	float specularMag = pow(max(dot(N, halfDir), 0.0), material.shininess);
	vec3 specular = specularMag * specularColor * material.specular;
	
	outColor = vec4(material.emissive + ambient + diffuse + specular, 1.0);
}