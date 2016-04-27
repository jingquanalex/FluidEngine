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
out vec4 outNormal;

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
	
		1. Output sphere eye space depth
		2. Blur with bilateral filtering
		3. ddx ddy for smooth normals
		4. Profit
	
	*/
	
	// Discard fragments outside of sphere
	vec3 N;
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	N.z = sqrt(1.0 - r2);
	
	if (colorThickness != 0.0)
	{
		outColor = vec4(N.z * colorThickness);
		return;
	}
	
	/*vec4 fragPos = vec4(eyepos + N * radius, 1.0);
	vec4 clipPos = projection * fragPos;
	float z = clipPos.z / clipPos.w;
	gl_FragDepth = z;*/
	
	// Eye space depth
	float z = eyepos.z + N.z * radius;
	z = far * (z + near) / (z * (far - near));
	gl_FragDepth = z;
	
	outColor = color;
	
	mat3 invv = mat3(transpose(view));
	outNormal = vec4(N, 1.0);
}