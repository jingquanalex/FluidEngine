#version 450

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D depthmap;
uniform vec2 blurDir;

const float filterRadius = 10.0;
const float blurScale = 0.1;
const float blurFalloff = 500.0;

void main()
{
	float depth = texture(depthmap, Texcoord).r;
	
	if (depth <= 0.0)
	{
		gl_FragDepth = 0.0;
		return;
	}

	if (depth >= 1.0)
	{
		gl_FragDepth = depth;
		return;
	}
	
	float sum = 0.0;
	float wsum = 0.0;
	
	for(float x = -filterRadius; x < filterRadius; x += 1.0)
	{
		float sampleDepth = texture(depthmap, Texcoord + x * blurDir).r;
		
		if (sampleDepth >= 1.0) continue;
		
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