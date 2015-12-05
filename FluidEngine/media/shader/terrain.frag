#version 450

in vec3 Normal;
in vec2 Texcoord;
in vec3 FragPos;
in vec4 FragPosLight;
in float OccFactor;

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
uniform sampler2DShadow shadowmap1;

uniform float maxheight;

vec3 checker(vec2 uv, vec3 color1, vec3 color2)
{
	float checkSize = 4.0;
	float fmodResult = mod(floor(checkSize * uv.x) + floor(checkSize * uv.y), 2.0);
	if (fmodResult < 1.0)
		return color1;
	else
		return color2;
}

float LinearizeDepth(float depth)
{
	float nearPlane = 0.1;
	float farPlane = 1000.0;
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane)) / farPlane;
}

float PCFShadow(vec4 fragPosLight, float NdL)
{
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	float bias = 0.0002;
	projCoords.z *= 1.0 + bias;
	float shadow = texture(shadowmap1, projCoords);
	
	return shadow;
}

void main()
{
	vec3 N = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	//vec3 I = normalize(FragPos - viewPos);
    //vec3 R = reflect(I, N);
	
	vec3 envMapA = texture(cubemap1, N).xyz;
	//vec3 envMapS = textureLod(cubemap1, R, 8).xyz;
	
	//vec3 diffuseMap = vec3(texture(diffuse1, Texcoord));
	//diffuseMap = checker(Texcoord, vec3(0.2), vec3(0.3));
	
	float hz = FragPos.y / maxheight;
	//vec3 colorGrade = vec3(hz * 0.15 + (hz * hz * hz) * 0.23, hz * 1.0 + (hz * hz) * 0.31, (hz * hz * hz) * 0.3);
	vec3 colorGrade = vec3(hz * hz * 1.0 + hz * hz * 0.3, hz * 1.0 + hz * hz * 0.7, (hz * hz * hz) * 2.5);
	colorGrade = (colorGrade);
	
	float shadow = PCFShadow(FragPosLight, dot(N, lightDir));
	shadow = clamp(shadow, 0.6, 1.0);
	//float shadow = LinearizeDepth(PCFShadow(FragPosLight, dot(N, lightDir)));
	vec3 color = envMapA * OccFactor * colorGrade * shadow;
	//vec3 color = vec3(shadow);
	
	outColor = vec4(color, 1.0);
}