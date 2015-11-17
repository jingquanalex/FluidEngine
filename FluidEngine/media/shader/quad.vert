#version 450

layout (location = 0) in vec3 position;

out vec3 EyeDir;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

//layout (location = 10) uniform mat4 matInverseProj;

void main()
{
	mat4 inverseProj = inverse(projection);
    mat4 inverseView = view;
	
	// Remove position from view matrix
    inverseView[3][0] = 0.0;
    inverseView[3][1] = 0.0;
    inverseView[3][2] = 0.0;
	
	inverseView = inverse(inverseView);

    EyeDir = vec3(inverseView * inverseProj * vec4(position, 1.0));

    gl_Position = vec4(position, 1.0);
}