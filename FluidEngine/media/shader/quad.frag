#version 450

in vec3 EyeDir;

out vec4 outColor;

uniform samplerCube cubemap;

void main()
{
	vec3 envmap = textureLod(cubemap, EyeDir, 1).xyz;
	
	
    outColor = vec4(envmap, 1.0);
}