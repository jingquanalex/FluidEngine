#version 450

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D samplemap;
uniform vec2 blurDir;
uniform float weight[5] = float[]( 0.382928, 0.241732, 0.060598, 0.005977, 0.000229 );

void main()
{
    vec3 sum = texture(samplemap, Texcoord).rgb * weight[0];
	
    for(int i = 1; i < 5; i++)
	{
		sum += texture(samplemap, Texcoord + i * blurDir).rgb * weight[i];
		sum += texture(samplemap, Texcoord - i * blurDir).rgb * weight[i];
	}
	
	outColor = vec4(sum, 1.0);
}