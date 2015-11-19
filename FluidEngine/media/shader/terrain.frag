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
uniform samplerCube cubemap1;

vec3 checker(vec2 uv, vec3 color1, vec3 color2)
{
	float checkSize = 4.0;
	float fmodResult = mod(floor(checkSize * uv.x) + floor(checkSize * uv.y), 2.0);
	if (fmodResult < 1.0)
		return color1;
	else
		return color2;
}

void main()
{
	vec3 N = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	vec3 diffuseMap = vec3(texture(diffuse1, Texcoord));
	
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, N);
	
	float hz = FragPos.y / 10;
	
	vec3 envMapA = texture(cubemap1, N).xyz;
	vec3 envMapS = textureLod(cubemap1, R, 8).xyz;
	
	diffuseMap = checker(Texcoord, vec3(0.2), vec3(0.3));
	
	vec3 colorGrade = vec3(hz * 0.15 + (hz * hz * hz) * 0.03, hz * 1.0 + (hz * hz) * 0.11, (hz * hz * hz) * 0.3);
	vec3 color = envMapA * colorGrade;// * diffuseMap;
	
	outColor = vec4(color, 1.0);
}