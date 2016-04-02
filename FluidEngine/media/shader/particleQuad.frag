#version 450

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D depthmap;

void main()
{
	float maxDepth = 0.99;

	float depthValue = texture(depthmap, Texcoord).r;
	if (depthValue > maxDepth) discard;
	
	
    outColor = vec4(vec3(depthValue), 1.0);
}