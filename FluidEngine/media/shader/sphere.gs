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
	mat4 viewproj = projection * view;
	vec4 position = gl_in[0].gl_Position;
	vec4 right = vec4(view[0][0], view[1][0], view[2][0], 0.0);
	vec4 up = vec4(view[0][1], view[1][1], view[2][1], 0.0);
	
    gl_Position = viewproj * (position - right - up);
	Texcoord = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = viewproj * (position + right - up);
	Texcoord = vec2(1.0, 0.0);
    EmitVertex();
	
	gl_Position = viewproj * (position - right + up);
	Texcoord = vec2(0.0, 1.0);
    EmitVertex();
	
	gl_Position = viewproj * (position + right + up);
	Texcoord = vec2(1.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}  