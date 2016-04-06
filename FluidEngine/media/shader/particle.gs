#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

in vec4 gsColor[];

out vec2 texcoord;
out vec4 color;
out vec3 eyepos;
out float radius;

uniform float fRadius;
uniform float colorThickness = 0.0;
uniform int renderMode;

void main()
{
	vec4 position = gl_in[0].gl_Position;
	eyepos = position.xyz;
	color = gsColor[0];
	radius = fRadius;
	if (renderMode != 0) radius *= 2.5; // Increase radius for ss fluid
	if (colorThickness != 0.0) radius *= 2.0;

	gl_Position = projection * (position + vec4(-radius, -radius, 0.0, 0.0));
	texcoord = vec2(0.0, 0.0);
    EmitVertex();

	gl_Position = projection * (position + vec4(radius, -radius, 0.0, 0.0));
	texcoord = vec2(1.0, 0.0);
    EmitVertex();
	
	gl_Position = projection * (position + vec4(-radius, radius, 0.0, 0.0));
	texcoord = vec2(0.0, 1.0);
    EmitVertex();
	
	gl_Position = projection * (position + vec4(radius, radius, 0.0, 0.0));
	texcoord = vec2(1.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}  