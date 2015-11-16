#version 450

in vec3 EyeDir;

out vec4 outColor;

uniform samplerCube cubemap;

void main()
{
    outColor = texture(cubemap, EyeDir);
}