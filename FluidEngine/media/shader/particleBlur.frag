#version 450

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D depthmap;
uniform vec2 blurDir;

void main()
{
	float depth = texture(depthmap, Texcoord).r;
	//if (depth > 0.99999) discard;
	
	float filterRadius = 10.0;
	float blurScale = 0.1;
	float blurFalloff = 100.0;
	
	float sum = 0.0;
	float wsum = 0.0;
	
	for(float x = -filterRadius; x < filterRadius; x += 1.0)
	{
		float sampleDepth = texture(depthmap, Texcoord + x * blurDir).r;
		
		// Spatial
		float r = x * blurScale;
		float w = exp(-r * r);
	
		// Range
		float r2 = (sampleDepth - depth) * blurFalloff;
		float g = exp(-r2 * r2);
	
		sum += sampleDepth * w * g;
		wsum += w * g;
    }
	
	if (wsum > 0.0)
	{
		sum /= wsum;
	}
	
	gl_FragDepth = sum;
}