#version 450

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

in vec2 texcoord;
in vec4 color;
in vec3 eyepos;
in float radius;

out vec4 outColor;

uniform float colorThickness = 0.0;
uniform int renderMode;
uniform float near;
uniform float far;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near) / (far + near - z * (far - near));
}

void main()
{
	/*vec2 v = texcoord - vec2(0.5);
	float dist = dot(v, v);
	if (dist > radius * radius) discard;
	
	dist = pow(1 - sqrt(dist), 3);
	outColor = color * vec4(dist, dist, dist, 1.0);*/
	
	/* 
	
		Screen space fluid rendering
		http://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf
	
		1. Output particle viewspace depth
		2. Blur with bilateral filtering
		3. Get eye-space position from depth
	
	*/
	
	// Calculate eye space normal
	vec3 N;
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	
	if (colorThickness != 0.0)
	{
		float dist = 1.0 - sqrt(r2);
		outColor = vec4(dist * colorThickness);
		return;
	}
	
	N.z = sqrt(1.0 - r2);
	
	// Calculate depth
	vec4 fragPos = vec4(eyepos + N * radius, 1.0);
	vec4 clipPos = projection * fragPos;
	float depth = clipPos.z / clipPos.w;
	gl_FragDepth = LinearizeDepth(depth);
	
	outColor = color;
}