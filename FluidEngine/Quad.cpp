#include "Quad.h"

using namespace glm;
using namespace std;

Quad::Quad(vec3 position) : Object(position)
{

}

Quad::~Quad()
{

}

void Quad::load(string shadername)
{
	// Make a quad mesh
	float quadVertices[] = { -0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f };

	quad = new Mesh(vector<float>(quadVertices, quadVertices + sizeof(quadVertices) / sizeof(float)));
	shader = new Shader(shadername);
}

void Quad::draw()
{
	glUseProgram(shader->getProgram());
	quad->draw();
}