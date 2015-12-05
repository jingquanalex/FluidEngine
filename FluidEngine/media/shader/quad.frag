#version 450

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D shadowmap;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane)) / farPlane;
}

void main()
{
	float depthValue = texture(shadowmap, Texcoord).r;
	outColor = vec4(vec3(LinearizeDepth(depthValue)), 1.0);
    //outColor = vec4(vec3(depthValue), 1.0);
}