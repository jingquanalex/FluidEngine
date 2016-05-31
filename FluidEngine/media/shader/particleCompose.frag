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

uniform int renderMode = 1;
uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D depthBlurMap;
uniform sampler2D sceneMap;
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
	float depthBlur = texture(depthBlurMap, Texcoord).r;
	vec4 scene = texture(sceneMap, Texcoord);
	
	if (depthBlur >= 1.0)
	{
		outColor = scene;
		return;
	}
	
	// Calculate eye-space position from depthBlur
	vec3 eyePos = uvToEye(Texcoord, depthBlur);
	
	// Calculate normal
	vec2 texelSize = vec2(1.0 / texSize.x, 1.0 / texSize.y);
	
	vec3 ddx = uvToEye(Texcoord + vec2(texelSize.x, 0.0), texture(depthBlurMap, Texcoord + vec2(texelSize.x, 0.0)).x) - eyePos;
	vec3 ddx2 = eyePos - uvToEye(Texcoord - vec2(texelSize.x, 0.0), texture(depthBlurMap, Texcoord - vec2(texelSize.x, 0.0)).x);
	
	if (abs(ddx.z) > abs(ddx2.z))
	{
		ddx = ddx2;
	}
	
	vec3 ddy = uvToEye(Texcoord + vec2(0.0, texelSize.y), texture(depthBlurMap, Texcoord + vec2(0.0, texelSize.y)).x) - eyePos;
	vec3 ddy2 = eyePos - uvToEye(Texcoord - vec2(0.0, texelSize.y), texture(depthBlurMap, Texcoord - vec2(0.0, texelSize.y)).x);
	
	if (abs(ddy.z) > abs(ddy2.z))
	{
		ddy = ddy2;
	}

	vec3 N = normalize(cross(ddx, ddy));
	
	
	// TESTING DEBUG
	/*float depth = texture(depthMap, Texcoord).r;
	vec4 normals = texture(normalMap, Texcoord);
	eyePos = uvToEye(Texcoord, depth);
	N = normals.xyz;*/
	
	float thickness = texture(thickMap, Texcoord).r;
	
	vec3 lightPos = vec3(0.0, 5.0, 0.0);
	vec3 lightDir = vec3(0.0, 1.0, 0.0);
	
	vec3 L = (view * vec4(lightDir, 0.0)).xyz;
    vec3 V = -normalize(eyePos);
    vec3 H = normalize(V + L);
    float specular = pow(max(dot(N, H), 0.0), 2000.0);
	
	float diffuse = max(dot(N, L), 0.0) * 0.5 + 0.5;
	
	float fresPower = 4.0f;
	float fresBias = 0.01;
	float fresnel = fresBias + (1.0 - fresBias) * pow(1.0 - dot(N, V), fresPower);
	
	mat3 invView = mat3(transpose(view));
	vec3 I = normalize(invView * eyePos - viewPos);
	vec4 envReflect = texture(envMap, reflect(I, invView * N));
	
	vec3 refractVal = vec3(0.38, 0.3, 0.34);
	vec4 envRefract = texture(sceneMap, Texcoord + N.xy * thickness * refractVal.r);
	
	// Chromatic abberation
	if (renderMode == 15)
	{
		envRefract.r = texture(sceneMap, Texcoord + N.xy * thickness * refractVal.r).r;
		envRefract.g = texture(sceneMap, Texcoord + N.xy * thickness * refractVal.g).g;
		envRefract.b = texture(sceneMap, Texcoord + N.xy * thickness * refractVal.b).b;
	}
	
	vec4 color = texture(colorMap, Texcoord);
	vec4 colorAbsorption = exp(-(vec4(1.0) - color) * thickness);
	vec4 colorRefract = colorAbsorption * envRefract;
	
	float rThickness = clamp(1.2 - thickness, 0.0, 1.0);
	vec4 finalColor = mix(diffuse * colorRefract, colorRefract, rThickness);
	
	// Limit sparse particles (splashes)
	if (thickness < 0.1)
	{
		specular = 0.0;
	}
	
	switch(renderMode)
	{
		case 1:
			outColor = finalColor + specular + fresnel * envReflect;
			break;
		case 2:
			outColor = vec4(depthBlur);
			break;
		case 3:
			outColor = vec4(N, 1.0);
			break;
		case 4:
			outColor = vec4(diffuse);
			break;
		case 5:
			outColor = diffuse * color + specular;
			break;
		case 6:
			outColor = colorAbsorption;
			break;
		case 7:
			outColor = envRefract;
			break;
		case 8:
			outColor = finalColor + specular;
			break;
		case 9:
			outColor = fresnel * envReflect;
			break;
		case 0:
			break;
		case 10: // Paint
			outColor = diffuse * color + specular;
			break;
		case 11: // Cloudy water
			float rT = clamp(1.0 - thickness, 0.0, 1.0);
			vec4 fColor = mix(colorAbsorption, envRefract, rT);
			outColor = fColor + specular + fresnel * envReflect;
			break;
		case 12: // Alien fluid
			float rT2 = clamp(thickness, 0.0, 1.0);
			vec4 fColor2 = mix(colorAbsorption, envRefract, rT2);
			outColor = vec4(0.5, 0.0, 0.0, 1.0) * fColor2 + specular + fresnel * envReflect;
			outColor *= max(dot(N, L), 0.0) * 0.5 + 0.3;
			break;
		case 13: // Mercury
			outColor = envReflect + specular;
			break;
		case 14: // Oil
			outColor = vec4(max(dot(N, L), 0.0));
			outColor.r *= texture(envMap, sin(2 * 3.14 * 0.90 / vec3(0.7) * reflect(I, invView * N))).r;
			outColor.g *= texture(envMap, sin(2 * 3.14 * 0.90 / vec3(0.55) * reflect(I, invView * N))).g;
			outColor.b *= texture(envMap, sin(2 * 3.14 * 0.90 / vec3(0.4) * reflect(I, invView * N))).b;
			outColor *= pow(max(dot(N, H), 0.0), 90.0);;
			break;
		case 15: // Oil slick (chromatic abberation)
			outColor = finalColor + specular + fresnel * envReflect;
			break;
	}
}