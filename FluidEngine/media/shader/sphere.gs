#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec2 Texcoord;

void main()
{
	vec4 position = gl_in[0].gl_Position;

	gl_Position = projection * (position + vec4(-0.5, -0.5, 0.0, 0.0));
	Texcoord = vec2(0.0, 0.0);
    EmitVertex();

	gl_Position = projection * (position + vec4(0.5, -0.5, 0.0, 0.0));
	Texcoord = vec2(1.0, 0.0);
    EmitVertex();
	
	gl_Position = projection * (position + vec4(-0.5, 0.5, 0.0, 0.0));
	Texcoord = vec2(0.0, 1.0);
    EmitVertex();
	
	gl_Position = projection * (position + vec4(0.5, 0.5, 0.0, 0.0));
	Texcoord = vec2(1.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}  