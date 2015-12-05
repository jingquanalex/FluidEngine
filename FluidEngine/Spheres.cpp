#include "Spheres.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

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
		sphere.position.x = static_cast<float> (rand() % 20 - 10) / 5;
		sphere.position.y = static_cast<float> (rand() % 20 - 10) / 5;
		sphere.position.z = static_cast<float> (rand() % 20 - 10) / 5;
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

	glBindVertexArray(0);

	texid = SOIL_load_OGL_texture((g_mediaDirectory + "cat.png").c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Spheres::update(float dt)
{
	shader->update(dt);
}

void Spheres::draw()
{
	glUseProgram(shader->getProgram());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 10);
	glBindVertexArray(0);
	glUseProgram(0);
}