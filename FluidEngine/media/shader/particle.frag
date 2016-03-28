#version 450

in vec2 texcoord;
in vec4 color;

out vec4 outColor;

uniform sampler2D tex;
uniform float radius;

void main()
{
	float dist = length(texcoord.xy - vec2(0.5, 0.5));
	float alpha = 1.0 - step(radius, dist);
	if (alpha == 0.0) discard;
	
	dist = pow(1.0 - dist, 2);

	//texture(tex, Texcoord).rgb
	outColor = color * vec4(dist, dist, dist, 1.0);
}