#version 450

in vec2 Texcoord;

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

uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform vec2 texSize;
uniform mat4 invProj;

vec3 uvToEye(vec2 texCoord, float z)
{
	float x = texCoord.x * 2.0 - 1.0;
	float y = texCoord.y * 2.0 - 1.0;
	vec4 clipPos = vec4(x, y, z, 1.0f);
	vec4 eyePos = invProj * clipPos;
	return eyePos.xyz / eyePos.w;
}

void main()
{
	vec2 texelSize = vec2(1.0 / texSize.x, 1.0 / texSize.y); // depthmap size

	float depth = texture(depthMap, Texcoord).r;
	if (depth > 0.99999) discard;
	
	// Calculate eye-space position from depth
	vec3 eyePos = uvToEye(Texcoord, depth);
	
	// Calculate normal
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
	
	vec3 lightDir = vec3(0.0, 1.0, 0.0);
	
	vec3 L = (view * vec4(lightDir, 0.0)).xyz;
    vec3 viewDir = -normalize(eyePos);
    vec3 halfVec = normalize(viewDir + L);
    float specular = pow(max(0.0f, dot(N, halfVec)), 1000.0);
	
	float diffuse = max(dot(N, L), 0.0) * 0.5 + 0.5;
	
	float fresPower = 2.0f;
	float fresScale = 0.9;
	float fresBias = 0.1;
	
	float fresnel = fresBias + fresScale * pow(1.0f - max(dot(N, viewDir), 0.0), fresPower);
	
	vec4 color = texture(colorMap, Texcoord);
	//color = vec4(1,0,0,1);
	
	
	outColor = (diffuse + specular + fresnel) * color;
	//outColor = texture(depthMap, Texcoord);
}