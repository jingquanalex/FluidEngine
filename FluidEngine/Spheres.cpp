#include "Spheres.h"

using namespace glm;
using namespace std;

Spheres::Spheres()
{

}

Spheres::~Spheres()
{

}

void Spheres::load()
{
	shader = new Shader("sphere");

	for (int i = 0; i < 10; i++)
	{
		Sphere sphere;
		sphere.position.x = static_cast<float> (rand() % 20 - 10) / 3;
		sphere.position.y = static_cast<float> (rand() % 20 - 10) / 3;
		sphere.position.z = static_cast<float> (rand() % 20 - 10) / 3;
		sphere.radius = static_cast<float> (rand() % 9 + 1) / 3;
		spheres.push_back(sphere);
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, spheres.size() * sizeof(Sphere), &spheres[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Sphere),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Sphere),
		(GLvoid*)offsetof(Sphere, Sphere::radius));

	glBindVertexArray(0);
}

void Spheres::update(float dt, Camera* camera)
{
	shader->update(dt, camera);
}

void Spheres::draw()
{
	glUseProgram(shader->getProgram());
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 10);
	glBindVertexArray(0);
	glUseProgram(0);
}