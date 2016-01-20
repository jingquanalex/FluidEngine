#include "Particles.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Particles::Particles()
{
	count = 0;
	maxCount = 0;
}

Particles::~Particles()
{

}

void Particles::load(int maxParticleCount)
{
	// Set max particles count and load particle shaders
	maxCount = maxParticleCount;
	shader = new Shader("particle");
	solver = new PCISPH(&particles);

	// Load a box model to contain the particles
	box = new Object();
	box->setScale(vec3(10));
	box->setWireframeMode(true);
	box->load("cube.obj");

	// Add some particles temp
	addParticles(20);

	// Initialize vertex buffer memory and vao
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, maxCount * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
		(GLvoid*)offsetof(Particle, Particle::Color));

	glBindVertexArray(0);

	/*texid = SOIL_load_OGL_texture((g_mediaDirectory + "cat.png").c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);*/
}

// Update particle positions and send to vbo
void Particles::update(float dt)
{
	solver->compute(dt);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Particle), &particles[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Render particles
void Particles::draw()
{
	glUseProgram(shader->getProgram());
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texid);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glBindVertexArray(0);
	glUseProgram(0);

	box->draw();
}

// Add a specified number of Particles with a random position
void Particles::addParticles(int value)
{
	for (int i = 0; i < value; i++)
	{
		Particle particle;
		particle.Position.x = static_cast<float>(rand() % 20 - 10) / 5;
		particle.Position.y = static_cast<float>(rand() % 20 - 10) / 5;
		particle.Position.z = static_cast<float>(rand() % 20 - 10) / 5;
		particle.Color = vec4(1.0);
		particles.push_back(particle);
	}
	count += value;
}

// Remove a specified number of Particles
void Particles::removeParticles(int value)
{
	if (value < count)
	{
		particles.resize(count - value);
		count -= value;
	}
	else
	{
		particles.clear();
		count = 0;
	}
}