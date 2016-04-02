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

//uniform sampler2D tex;

void main()
{
	/*vec2 v = texcoord - vec2(0.5);
	float dist = dot(v, v);
	if (dist > radius * radius) discard;
	
	dist = pow(1 - sqrt(dist), 3);
	outColor = color * vec4(dist, dist, dist, 1.0);
	
	*/
	
	// Rendering particles
	// http://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf
	// Screenspace metaballs
	// http://www.gamedev.net/topic/564607-image-space-3d-metaballs/
	
	// Calculate eye sphere space normal
	vec3 N = vec3(texcoord.xy, 0.0);
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	N.z = -sqrt(1.0 - r2);
	
	// Calculate depth
	vec4 pixelPos = vec4(eyepos + N * radius, 1.0);
	vec4 clipPos = projection * pixelPos;
	gl_FragDepth = clipPos.z / clipPos.w;
	
	float colorMag = -N.z;
	vec3 lightDir = vec3(0.0, 1.0, 0.0);
	float diffuse = max(dot(N, lightDir), 0.0) * colorMag;
	
	//outColor = vec4(LinearizeDepth(gl_FragDepth));
	outColor = color * colorMag + diffuse;
}