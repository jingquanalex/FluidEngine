#version 450

in vec2 Texcoord;
in vec3 Fragpos;

out vec4 outColor;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140, binding = 1) uniform Lighting
{
	vec3 viewPos;
    vec3 lightPos;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

uniform int outputMode = 1;
uniform sampler2D sceneMap;
uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform samplerCube envMap;
uniform sampler2D thickMap;
uniform vec2 texSize;
uniform mat4 invProj;

vec3 uvToEye(vec2 texCoord, float z)
{
	vec2 pos = texCoord * 2.0 - 1.0;
	vec4 clipPos = vec4(pos, z, 1.0);
	vec4 viewPos = invProj * clipPos;
	return viewPos.xyz / viewPos.w;
}

void main()
{
	vec4 scene = texture(sceneMap, Texcoord);
	float depth = texture(depthMap, Texcoord).r;
	
	if (depth > 0.999)
	{
		outColor = scene;
		return;
	}
	
	// Calculate eye-space position from depth
	vec3 eyePos = uvToEye(Texcoord, depth);
	
	// Calculate normal
	vec2 texelSize = vec2(1.0 / texSize.x, 1.0 / texSize.y);
	
	vec3 ddx = uvToEye(Texcoord + vec2(texelSize.x, 0.0), texture(depthMap, Texcoord + vec2(texelSize.x, 0.0)).x) - eyePos;
	vec3 ddx2 = eyePos - uvToEye(Texcoord - vec2(texelSize.x, 0.0), texture(depthMap, Texcoord - vec2(texelSize.x, 0.0)).x);
	
	if (abs(ddx.z) > abs(ddx2.z))
	{
		ddx = ddx2;
	}
	
	vec3 ddy = uvToEye(Texcoord + vec2(0.0, texelSize.y), texture(depthMap, Texcoord + vec2(0.0, texelSize.y)).x) - eyePos;
	vec3 ddy2 = eyePos - uvToEye(Texcoord - vec2(0.0, texelSize.y), texture(depthMap, Texcoord - vec2(0.0, texelSize.y)).x);
	
	if (abs(ddy.z) > abs(ddy2.z))
	{
		ddy = ddy2;
	}

	vec3 N = normalize(cross(ddx, ddy));
	
	vec3 lightPos = vec3(0.0, 5.0, 0.0);
	vec3 lightDir = vec3(0.0, 1.0, 0.0);
	
	vec3 L = (view * vec4(lightDir, 0.0)).xyz;
    vec3 V = -normalize(eyePos);
    vec3 H = normalize(V + L);
    float specular = pow(max(dot(N, H), 0.0), 3000.0);
	
	float diffuse = max(dot(N, L), 0.0) * 0.5 + 0.5;
	
	float fresPower = 2.0f;
	float fresBias = 0.01;
	float fresnel = fresBias + (1.0 - fresBias) * pow(1.0 - max(dot(N, V), 0.0), fresPower);
	
	float thickness = texture(thickMap, Texcoord).r;
	
	vec4 envReflect = texture(envMap, reflect(normalize(eyePos), N));
	
	float refractVal = 0.1;
	vec4 envRefract = texture(sceneMap, Texcoord + N.xy * thickness * refractVal);
	
	vec4 color = texture(colorMap, Texcoord);
	vec4 colorAbsorption = exp(-(vec4(1.0) - color) * thickness);
	
	vec4 finalColor = colorAbsorption * envRefract;
	
	if (outputMode == 1)
		outColor = diffuse * finalColor + specular + fresnel * envReflect;
	else if (outputMode == 2)
		outColor = vec4(depth);
	else if (outputMode == 3)
		outColor = vec4(N, 1.0);
	else if (outputMode == 4)
		outColor = vec4(diffuse);
	else if (outputMode == 5)
		outColor = colorAbsorption;
	else if (outputMode == 6)
		outColor = envRefract;
	else if (outputMode == 7)
		outColor = finalColor;
	else if (outputMode == 8)
		outColor = diffuse * finalColor + specular;
	else if (outputMode == 9)
		outColor = fresnel * envReflect;
}