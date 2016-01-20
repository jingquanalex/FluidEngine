#version 450

in vec2 texcoord;
in vec4 color;

out vec4 outColor;

uniform sampler2D tex;

void main()
{
	float radius = 0.5;
	float dist = length(texcoord.xy - vec2(0.5, 0.5));
	float alpha = 1 - step(radius, dist);
	
	if (alpha == 0.0) discard;
	
	dist = 1 - dist;

	//texture(tex, Texcoord).rgb
	outColor = color * vec4(dist, dist/2, dist/4, 1.0);
}